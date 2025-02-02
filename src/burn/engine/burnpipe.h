#pragma once
// Copyright (c) .NET Foundation and contributors. All rights reserved. Licensed under the Microsoft Reciprocal License. See LICENSE.TXT file in the project root for full license information.


#ifdef __cplusplus
extern "C" {
#endif

enum BURN_PIPE_CAPABILITIES
{
    BURN_PIPE_CAPABILITIES_NONE = 0,
    BURN_PIPE_CAPABILITIES_CUSTOM_MESSAGE = 1,

    // All the capabilities that this engine supports, used when creating capabilities command line for the embedded bundle
    BURN_PIPE_CAPABILITIES_ALL = BURN_PIPE_CAPABILITIES_CUSTOM_MESSAGE,
};

typedef struct _BURN_PIPE_CONNECTION
{
    LPWSTR sczName;
    LPWSTR sczSecret;
    DWORD dwProcessId;
    BURN_PIPE_CAPABILITIES dwCapabilities; // Capabilities of the engine that created the pipe
    BOOL fStrLen64; // Does the other size read/write string size in SIZE_T or DWORD? (WiX3, WiX5+: DWORD; WiX4: SIZE_T)

    HANDLE hProcess;
    HANDLE hPipe;
    HANDLE hCachePipe;
    HANDLE hLoggingPipe;
    HANDLE hQuitRequested;
    HANDLE hQuitMonitorThread;
} BURN_PIPE_CONNECTION;

typedef enum _BURN_PIPE_MESSAGE_TYPE : DWORD
{
    BURN_PIPE_MESSAGE_TYPE_LOG = 0xF0000001,
    BURN_PIPE_MESSAGE_TYPE_COMPLETE = 0xF0000002,
    BURN_PIPE_MESSAGE_TYPE_TERMINATE = 0xF0000003,
} BURN_PIPE_MESSAGE_TYPE;

typedef struct _BURN_PIPE_RESULT
{
    DWORD dwResult;
    BOOL fRestart;
} BURN_PIPE_RESULT;


typedef HRESULT (*PFN_PIPE_MESSAGE_CALLBACK)(
    __in PIPE_MESSAGE* pMsg,
    __in_opt LPVOID pvContext,
    __out DWORD* pdwResult
    );


// Common functions.
void BurnPipeConnectionInitialize(
    __in BURN_PIPE_CONNECTION* pConnection
    );
void BurnPipeConnectionUninitialize(
    __in BURN_PIPE_CONNECTION* pConnection
    );
HRESULT BurnPipeSendMessage(
    __in HANDLE hPipe,
    __in DWORD dwMessage,
    __in_bcount_opt(cbData) LPVOID pvData,
    __in SIZE_T cbData,
    __in_opt PFN_PIPE_MESSAGE_CALLBACK pfnCallback,
    __in_opt LPVOID pvContext,
    __out DWORD* pdwResult
    );
HRESULT BurnPipePumpMessages(
    __in HANDLE hPipe,
    __in_opt PFN_PIPE_MESSAGE_CALLBACK pfnCallback,
    __in_opt LPVOID pvContext,
    __in BURN_PIPE_RESULT* pResult
    );

// Parent functions.
HRESULT BurnPipeCreateNameAndSecret(
    __out_z LPWSTR *psczConnectionName,
    __out_z LPWSTR *psczSecret
    );
HRESULT BurnPipeCreatePipes(
    __in BURN_PIPE_CONNECTION* pConnection,
    __in BOOL fCompanion
    );
HRESULT BurnPipeWaitForChildConnect(
    __in BURN_PIPE_CONNECTION* pConnection
    );
HRESULT BurnPipeTerminateLoggingPipe(
    __in HANDLE hLoggingPipe,
    __in DWORD dwParentExitCode
    );
HRESULT BurnPipeTerminateChildProcess(
    __in BURN_PIPE_CONNECTION* pConnection,
    __in DWORD dwParentExitCode,
    __in BOOL fRestart
    );

// Child functions.
HRESULT BurnPipeChildConnect(
    __in BURN_PIPE_CONNECTION* pConnection,
    __in BOOL fCompanion
    );

#ifdef __cplusplus
}
#endif
