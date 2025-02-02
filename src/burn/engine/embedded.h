#pragma once
// Copyright (c) .NET Foundation and contributors. All rights reserved. Licensed under the Microsoft Reciprocal License. See LICENSE.TXT file in the project root for full license information.


#ifdef __cplusplus
extern "C" {
#endif

typedef enum _BURN_EMBEDDED_MESSAGE_TYPE
{
    BURN_EMBEDDED_MESSAGE_TYPE_UNKNOWN,
    BURN_EMBEDDED_MESSAGE_TYPE_ERROR,
    BURN_EMBEDDED_MESSAGE_TYPE_PROGRESS,
    BURN_EMBEDDED_MESSAGE_TYPE_CUSTOM_MESSAGE = 1000, // Allow enough room for standard WiX values
} BURN_EMBEDDED_MESSAGE_TYPE;


HRESULT EmbeddedRunBundle(
    __in BOOL fPipeStrLen64,
    __in_z LPCWSTR wzExecutablePath,
    __in_z LPWSTR sczBaseCommand,
    __in_z_opt LPCWSTR wzUserArgs,
    __in PFN_GENERICMESSAGEHANDLER pfnGenericMessageHandler,
    __in LPVOID pvContext,
    __out DWORD* pdwExitCode
    );

#ifdef __cplusplus
}
#endif
