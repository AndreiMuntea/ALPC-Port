#ifndef __ALPC_TRANSPORT_BASE_HPP__
#define __ALPC_TRANSPORT_BASE_HPP__ 

// 
//   Copyright (C) 2019 BitDefender S.R.L. 
//   Author(s)    : ANDREI-MARIUS MUNTEA (amuntea@bitdefender.com) 
//

#include "ntinclude.h"
#include "alpc_message.hpp"
#include "alpc_message_attributes.hpp"

#include <string>
#include <limits>

typedef enum _ALPC_DISCONNECT_PORT_FLAGS
{
    kAlpcDisconnectFlagNone = 0,
    kAlpcDisconnectFlagNoFlushOnClose = 1,
}ALPC_DISCONNECT_PORT_FLAGS, *PALPC_DISCONNECT_PORT_FLAGS;

typedef enum _ALPC_PORT_ATTRIBUTES_VALUES
{
    kAlpcPortAttributesNone = 0,
    kAlpcPortAttributesLpcPort = 0x1000, // Not accessible outside the kernel.
    kAlpcPortAttributesAllowImpersonation = 0x10000,
    kAlpcPortAttributesAllowLpcRequests = 0x20000,
    kAlpcPortAttributesWaitablePort = 0x40000,
    kAlpcPortAttributesAllowDupObject = 0x80000,
    kAlpcPortAttributesSystemProcess = 0x100000, // Not accessible outside the kernel.
    kAlpcPortAttributesLrpcWakePolicy1 = 0x200000,
    kAlpcPortAttributesLrpcWakePolicy2 = 0x400000,
    kAlpcPortAttributesLrpcWakePolicy3 = 0x800000,
    kAlpcPortAttributesDirectMessage = 0x1000000,
    kAlpcPortAttributesAllowMultiHandleAttribute = 0x2000000,  /// If set then object duplication won't complete. 
                                                               /// Used by RPC to ensure multi-handle attributes don't fail when receiving
}ALPC_PORT_ATTRIBUTES_VALUES, *PALPC_PORT_ATTRIBUTES_VALUES;

class AlpcTransportBase
{
public:
    AlpcTransportBase() = default;
    virtual ~AlpcTransportBase() = default;

protected:
    _Must_inspect_result_
    NTSTATUS
    SendAlpcMessage(
        _In_ const std::wstring& Message,
        _In_ HANDLE AlpcPortHandle
    );

    _Must_inspect_result_
    NTSTATUS
    ReceiveAlpcMessage(
        _Out_ PALPC_CUSTOM_MESSAGE* Message,
        _In_ HANDLE AlpcPortHandle
    );

    VOID
    DisconnectAlpcPort(
        _Inout_ PHANDLE AlpcPortHandle
    );

    VOID
    InitializeAlpcPortAttributes(
        _Inout_ ALPC_PORT_ATTRIBUTES& AlpcPortAttributes
    );

protected:
    SIZE_T m_MaxMessageLength = 0x1000;
    SIZE_T m_MemoryBandwith = 0x1000;
    SIZE_T m_MaxPoolUsage = 0x1000;
    SIZE_T m_MaxSectionSize = 0x1000;
    SIZE_T m_MaxViewSize = 0x1000;
    SIZE_T m_MaxTotalSectionSize = 0x1000;
    ULONG m_DupObjectTypes = 0x0;

private:
    _Must_inspect_result_
    NTSTATUS
    InitializeMessageAndAttributes(
        _In_ ULONG MessageAttributesFlags,
        _In_opt_ const BYTE* Buffer,
        _In_ size_t BufferSize,
        _Out_ PALPC_CUSTOM_MESSAGE* AlpcMessage,
        _Out_ PALPC_MESSAGE_ATTRIBUTES* AlpcMessageAttributes
    );
};

#endif //__ALPC_TRANSPORT_BASE_HPP__