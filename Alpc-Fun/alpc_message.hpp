#ifndef __ALPC_MESSAGE_HPP__
#define __ALPC_MESSAGE_HPP__

#include "ntinclude.h"

#pragma pack(push, 1)
typedef struct _ALPC_CUSTOM_MESSAGE
{
    PORT_MESSAGE Header;
    BYTE Buffer[1];
}ALPC_CUSTOM_MESSAGE, *PALPC_CUSTOM_MESSAGE;
#pragma pack(pop)

typedef enum _ALPC_MESSAGE_TYPE
{
    kAlpcMessageTypeLpcRequest = 1,
    kAlpcMessageTypeLpcConnectionRequest = 10
}ALPC_MESSAGE_TYPE, *PALPC_MESSAGE_TYPE;

typedef enum _ALPC_MESSAGE_FLAGS
{
    kAlpcMessageFlagNone = 0,
    kAlpcMessageFlagReplyMessage = 0x1,
    kAlpcMessageFlagLpcMode = 0x2,
    kAlpcMessageFlagReleaseMessage = 0x10000,
    kAlpcMessageFlagSyncRequest = 0x20000,
    kAlpcMessageFlagTrackPortReferences = 0x40000,
    kAlpcMessageFlagWaitUserMode = 0x100000,
    kAlpcMessageFlagWaitAlertable = 0x200000,
    kAlpcMessageFlagWaitChargePolicy = 0x400000,
    kAlpcMessageFlagUnknown1000000 = 0x1000000,
    kAlpcMessageFlagWow64Call = 0x40000000,   /// When used all structures passed to kernel need to be 64 bit versions.
}ALPC_MESSAGE_FLAGS, *PALPC_MESSAGE_FLAGS;

_Must_inspect_result_
NTSTATUS
AlpcMessageInitialize(
    _In_opt_ const BYTE* Buffer,
    _In_ USHORT BufferSize,
    _Out_ PALPC_CUSTOM_MESSAGE* AlpcMessage
);

VOID
AlpcMessageUninitialize(
    _Inout_ PALPC_CUSTOM_MESSAGE* AlpcMessage
);

#endif //__ALPC_MESSAGE_HPP__