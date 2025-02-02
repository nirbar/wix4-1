// Copyright (c) .NET Foundation and contributors. All rights reserved. Licensed under the Microsoft Reciprocal License. See LICENSE.TXT file in the project root for full license information.

#include "precomp.h"


// struct

struct BURN_EMBEDDED_CALLBACK_CONTEXT
{
    PFN_GENERICMESSAGEHANDLER pfnGenericMessageHandler;
    LPVOID pvContext;
    BOOL fStrLen64; // Does the other size read/write string size in SIZE_T or DWORD? (WiX3, WiX5+: DWORD; WiX4: SIZE_T)
};

// internal function declarations

static HRESULT ProcessEmbeddedMessages(
    __in PIPE_MESSAGE* pMsg,
    __in_opt LPVOID pvContext,
    __out DWORD* pdwResult
    );
static HRESULT OnEmbeddedErrorMessage(
    __in PFN_GENERICMESSAGEHANDLER pfnMessageHandler,
    __in BOOL fStrLen64,
    __in LPVOID pvContext,
    __in_bcount(cbData) BYTE* pbData,
    __in SIZE_T cbData,
    __out DWORD* pdwResult
    );
static HRESULT OnEmbeddedProgress(
    __in PFN_GENERICMESSAGEHANDLER pfnMessageHandler,
    __in LPVOID pvContext,
    __in_bcount(cbData) BYTE* pbData,
    __in SIZE_T cbData,
    __out DWORD* pdwResult
    );
static HRESULT OnEmbeddedCustomMessage(
    __in PFN_GENERICMESSAGEHANDLER pfnMessageHandler,
    __in BOOL fStrLen64,
    __in LPVOID pvContext,
    __in_bcount(cbData) BYTE* pbData,
    __in SIZE_T cbData,
    __out DWORD* pdwResult
    );
// function definitions

/*******************************************************************
 EmbeddedRunBundle -

*******************************************************************/
extern "C" HRESULT EmbeddedRunBundle(
    __in BOOL fPipeStrLen64,
    __in_z LPCWSTR wzExecutablePath,
    __in_z LPWSTR sczBaseCommand,
    __in_z_opt LPCWSTR wzUserArgs,
    __in PFN_GENERICMESSAGEHANDLER pfnGenericMessageHandler,
    __in LPVOID pvContext,
    __out DWORD* pdwExitCode
    )
{
    HRESULT hr = S_OK;
    BURN_PIPE_CONNECTION connection = {};
    DWORD dwCurrentProcessId = ::GetCurrentProcessId();
    LPWSTR sczCommand = NULL;
    PROCESS_INFORMATION pi = { };
    BURN_PIPE_RESULT result = { };

    BurnPipeConnectionInitialize(&connection);
    connection.fStrLen64 = fPipeStrLen64;

    BURN_EMBEDDED_CALLBACK_CONTEXT context = { };
    context.pfnGenericMessageHandler = pfnGenericMessageHandler;
    context.pvContext = pvContext;
    context.fStrLen64 = connection.fStrLen64;

    hr = BurnPipeCreateNameAndSecret(&connection.sczName, &connection.sczSecret);
    ExitOnFailure(hr, "Failed to create embedded pipe name and client token.");

    hr = BurnPipeCreatePipes(&connection, FALSE);
    ExitOnFailure(hr, "Failed to create embedded pipe.");

    hr = StrAllocFormatted(&sczCommand, L"%ls -%ls %ls %ls %u -%ls %u", sczBaseCommand, BURN_COMMANDLINE_SWITCH_EMBEDDED, connection.sczName, connection.sczSecret, dwCurrentProcessId, BURN_COMMANDLINE_SWITCH_EMBEDDED_CAPABILITIES, BURN_PIPE_CAPABILITIES_ALL);
    ExitOnFailure(hr, "Failed to append embedded args.");

    // Always add user supplied arguments last.
    if (wzUserArgs)
    {
        hr = StrAllocConcatFormattedSecure(&sczCommand, L" %ls", wzUserArgs);
        ExitOnFailure(hr, "Failed to append user args.");
    }

    hr = CoreCreateProcess(wzExecutablePath, sczCommand, TRUE, CREATE_NO_WINDOW, NULL, 0, &pi);
    ExitOnFailure(hr, "Failed to create embedded process at path: %ls", wzExecutablePath);

    connection.dwProcessId = ::GetProcessId(pi.hProcess);
    connection.hProcess = pi.hProcess;
    pi.hProcess = NULL;

    hr = BurnPipeWaitForChildConnect(&connection);
    ExitOnFailure(hr, "Failed to wait for embedded process to connect to pipe.");

    hr = BurnPipePumpMessages(connection.hPipe, ProcessEmbeddedMessages, &context, &result);
    ExitOnFailure(hr, "Failed to process messages from embedded message.");

    // Get the return code from the embedded process.
    hr = CoreWaitForProcCompletion(connection.hProcess, INFINITE, pdwExitCode);
    ExitOnFailure(hr, "Failed to wait for embedded executable: %ls", wzExecutablePath);

LExit:
    ReleaseHandle(pi.hThread);
    ReleaseHandle(pi.hProcess);

    StrSecureZeroFreeString(sczCommand);
    BurnPipeConnectionUninitialize(&connection);

    return hr;
}


// internal function definitions

static HRESULT ProcessEmbeddedMessages(
    __in PIPE_MESSAGE* pMsg,
    __in_opt LPVOID pvContext,
    __out DWORD* pdwResult
    )
{
    HRESULT hr = S_OK;
    BURN_EMBEDDED_CALLBACK_CONTEXT* pContext = static_cast<BURN_EMBEDDED_CALLBACK_CONTEXT*>(pvContext);
    DWORD dwResult = 0;

    // Process the message.
    switch (pMsg->dwMessageType)
    {
    case BURN_EMBEDDED_MESSAGE_TYPE_ERROR:
        hr = OnEmbeddedErrorMessage(pContext->pfnGenericMessageHandler, pContext->fStrLen64, pContext->pvContext, static_cast<BYTE*>(pMsg->pvData), pMsg->cbData, &dwResult);
        ExitOnFailure(hr, "Failed to process embedded error message.");
        break;

    case BURN_EMBEDDED_MESSAGE_TYPE_PROGRESS:
        hr = OnEmbeddedProgress(pContext->pfnGenericMessageHandler, pContext->pvContext, static_cast<BYTE*>(pMsg->pvData), pMsg->cbData, &dwResult);
        ExitOnFailure(hr, "Failed to process embedded progress message.");
        break;

    case BURN_EMBEDDED_MESSAGE_TYPE_CUSTOM_MESSAGE:
        hr = OnEmbeddedCustomMessage(pContext->pfnGenericMessageHandler, pContext->fStrLen64, pContext->pvContext, static_cast<BYTE*>(pMsg->pvData), pMsg->cbData, &dwResult);
        ExitOnFailure(hr, "Failed to process embedded custom message.");
        break;

    default:
        LogStringLine(REPORT_DEBUG, "Unexpected embedded message received from child process, msg: %u", pMsg->dwMessageType);
        dwResult = (DWORD)E_NOTIMPL;
    }

    *pdwResult = dwResult;

LExit:
    return hr;
}

static HRESULT OnEmbeddedErrorMessage(
    __in PFN_GENERICMESSAGEHANDLER pfnMessageHandler,
    __in BOOL fStrLen64,
    __in LPVOID pvContext,
    __in_bcount(cbData) BYTE* pbData,
    __in SIZE_T cbData,
    __out DWORD* pdwResult
    )
{
    HRESULT hr = S_OK;
    SIZE_T iData = 0;
    GENERIC_EXECUTE_MESSAGE message = { };
    LPWSTR sczMessage = NULL;

    message.type = GENERIC_EXECUTE_MESSAGE_ERROR;

    hr = BuffReadNumber(pbData, cbData, &iData, &message.error.dwErrorCode);
    ExitOnFailure(hr, "Failed to read error code from buffer.");

    if (fStrLen64)
    {
        hr = BuffReadStringSize64(pbData, cbData, &iData, &sczMessage);
    }
    else
    {
        hr = BuffReadString(pbData, cbData, &iData, &sczMessage);
    }
    ExitOnFailure(hr, "Failed to read error message from buffer.");

    message.error.wzMessage = sczMessage;

    hr = BuffReadNumber(pbData, cbData, &iData, &message.dwUIHint);
    ExitOnFailure(hr, "Failed to read UI hint from buffer.");

    *pdwResult = (DWORD)pfnMessageHandler(&message, pvContext);

LExit:
    ReleaseStr(sczMessage);

    return hr;
}

static HRESULT OnEmbeddedCustomMessage(
    __in PFN_GENERICMESSAGEHANDLER pfnMessageHandler,
    __in BOOL fStrLen64,
    __in LPVOID pvContext,
    __in_bcount(cbData) BYTE* pbData,
    __in SIZE_T cbData,
    __out DWORD* pdwResult
    )
{
    HRESULT hr = S_OK;
    SIZE_T iData = 0;
    GENERIC_EXECUTE_MESSAGE message = { };
    LPWSTR sczMessage = NULL;

    message.type = GENERIC_EXECUTE_MESSAGE_CUSTOM;

    hr = BuffReadNumber(pbData, cbData, &iData, &message.custom.dwCode);
    ExitOnFailure(hr, "Failed to read custom code from buffer.");

    if (fStrLen64)
    {
        hr = BuffReadStringSize64(pbData, cbData, &iData, &sczMessage);
    }
    else
    {
        hr = BuffReadString(pbData, cbData, &iData, &sczMessage);
    }
    ExitOnFailure(hr, "Failed to read custom message from buffer.");

    message.custom.wzMessage = sczMessage;

    *pdwResult = (DWORD)pfnMessageHandler(&message, pvContext);

LExit:
    ReleaseStr(sczMessage);

    return hr;
}

static HRESULT OnEmbeddedProgress(
    __in PFN_GENERICMESSAGEHANDLER pfnMessageHandler,
    __in LPVOID pvContext,
    __in_bcount(cbData) BYTE* pbData,
    __in SIZE_T cbData,
    __out DWORD* pdwResult
    )
{
    HRESULT hr = S_OK;
    SIZE_T iData = 0;
    GENERIC_EXECUTE_MESSAGE message = { };

    message.type = GENERIC_EXECUTE_MESSAGE_PROGRESS;
    message.dwUIHint = MB_OKCANCEL;

    hr = BuffReadNumber(pbData, cbData, &iData, &message.progress.dwPercentage);
    ExitOnFailure(hr, "Failed to read progress from buffer.");

    *pdwResult = (DWORD)pfnMessageHandler(&message, pvContext);

LExit:
    return hr;
}
