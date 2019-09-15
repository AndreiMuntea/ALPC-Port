#include "alpc_transport_base.hpp"

#include <assert.h>

_Use_decl_annotations_
NTSTATUS 
AlpcTransportBase::SendAlpcMessage(
    _In_ const std::wstring & Message,
    _In_ HANDLE AlpcPortHandle
)
{
    PALPC_CUSTOM_MESSAGE message = nullptr;
    PALPC_MESSAGE_ATTRIBUTES messageAttributes = nullptr;

    auto status = InitializeMessageAndAttributes(
        ALPC_MESSAGE_ATTRIBUTES_VALUES::kAlpcMessageAttributesNone,     // [in] MessageAttributesFlags : ULONG
        reinterpret_cast<const BYTE*>(Message.c_str()),                 // [in opt] Buffer : const BYTE*
        Message.size() * sizeof(wchar_t),                               // [in] BufferSize : size_t
        &message,                                                       // [out] AlpcMessage : PALPC_CUSTOM_MESSAGE*
        &messageAttributes                                              // [out] AlpcMessageAttributes: PALPC_MESSAGE_ATTRIBUTES*
    );
    if (!NT_SUCCESS(status))
    {
        goto CleanUp;
    }

    status = NtAlpcSendWaitReceivePort(
        AlpcPortHandle,                             // [in] PortHandle : HANDLE
        ALPC_MESSAGE_FLAGS::kAlpcMessageFlagNone,   // [in] Flags : ULONG
        &message->Header,                           // [in opt] SendMessage : PPORT_MESSAGE
        messageAttributes,                          // [inout opt] SendMessageAttributes: PALPC_MESSAGE_ATTRIBUTES
        nullptr,                                    // [out opt] ReceiveMessage : PPORT_MESSAGE
        nullptr,                                    // [inout opt] BufferLength : PSIZE_T
        nullptr,                                    // [inout opt] ReceiveMessageAttributes : PALPC_MESSAGE_ATTRIBUTES
        nullptr                                     // [in opt] Timeout : PLARGE_INTEGER
    );
    if (!NT_SUCCESS(status))
    {
        goto CleanUp;
    }
   
CleanUp:
    AlpcMessageUninitialize(&message);
    AlpcMessageAttributesUninitialize(&messageAttributes);
    return status;
}

_Use_decl_annotations_
NTSTATUS 
AlpcTransportBase::ReceiveAlpcMessage(
    _Out_ PALPC_CUSTOM_MESSAGE* Message,
    _In_ HANDLE AlpcPortHandle
)
{
    *Message = nullptr;

    PALPC_CUSTOM_MESSAGE message = nullptr;
    SIZE_T messageLength = 0;
    PALPC_MESSAGE_ATTRIBUTES messageAttributes = nullptr;

    auto status = InitializeMessageAndAttributes(
        ALPC_MESSAGE_ATTRIBUTES_VALUES::kAlpcMessageAttributesNone,     // [in] MessageAttributesFlags : ULONG
        nullptr,                                                        // [in opt] Buffer : const BYTE*
        m_MaxMessageLength,                                             // [in] BufferSize : size_t
        &message,                                                       // [out] AlpcMessage : PALPC_CUSTOM_MESSAGE*
        &messageAttributes                                              // [out] AlpcMessageAttributes: PALPC_MESSAGE_ATTRIBUTES*
    );
    if (!NT_SUCCESS(status))
    {
        goto CleanUp;
    }
    messageLength = message->Header.u1.s1.TotalLength;

    status = NtAlpcSendWaitReceivePort(
        AlpcPortHandle,                             // [in] PortHandle : HANDLE
        ALPC_MESSAGE_FLAGS::kAlpcMessageFlagNone,   // [in] Flags : ULONG
        nullptr,                                    // [in opt] SendMessage : PPORT_MESSAGE
        nullptr,                                    // [inout opt] SendMessageAttributes: PALPC_MESSAGE_ATTRIBUTES
        &message->Header,                           // [out opt] ReceiveMessage : PPORT_MESSAGE
        &messageLength,                             // [inout opt] BufferLength : PSIZE_T
        messageAttributes,                          // [inout opt] ReceiveMessageAttributes : PALPC_MESSAGE_ATTRIBUTES
        nullptr                                     // [in opt] Timeout : PLARGE_INTEGER
    );
    if (!NT_SUCCESS(status))
    {
        goto CleanUp;
    }

    *Message = message;
    message = nullptr;

CleanUp:
    AlpcMessageUninitialize(&message);
    AlpcMessageAttributesUninitialize(&messageAttributes);
    return status;
}

VOID 
AlpcTransportBase::DisconnectAlpcPort(
    _Inout_ PHANDLE AlpcPortHandle
)
{
    if (*AlpcPortHandle)
    {
        auto status = NtAlpcDisconnectPort(*AlpcPortHandle, ALPC_DISCONNECT_PORT_FLAGS::kAlpcDisconnectFlagNone);
        assert(NT_SUCCESS(status));

        auto result = CloseHandle(*AlpcPortHandle);
        assert(result);

        *AlpcPortHandle = nullptr;
    }
}

VOID 
AlpcTransportBase::InitializeAlpcPortAttributes(
    _Inout_ ALPC_PORT_ATTRIBUTES& AlpcPortAttributes
)
{
    RtlZeroMemory(&AlpcPortAttributes, sizeof(AlpcPortAttributes));
    
    AlpcPortAttributes.Flags = ALPC_PORT_ATTRIBUTES_VALUES::kAlpcPortAttributesAllowImpersonation;
    AlpcPortAttributes.MaxMessageLength = m_MaxMessageLength;
    AlpcPortAttributes.MemoryBandwidth = m_MemoryBandwith;
    AlpcPortAttributes.MaxMessageLength = m_MaxPoolUsage;
    AlpcPortAttributes.MaxSectionSize = m_MaxSectionSize;
    AlpcPortAttributes.MaxViewSize = m_MaxViewSize;
    AlpcPortAttributes.DupObjectTypes = m_DupObjectTypes;

    AlpcPortAttributes.SecurityQos.Length = sizeof(AlpcPortAttributes.SecurityQos);
    AlpcPortAttributes.SecurityQos.ImpersonationLevel = SECURITY_IMPERSONATION_LEVEL::SecurityImpersonation;
    AlpcPortAttributes.SecurityQos.ContextTrackingMode = SECURITY_DYNAMIC_TRACKING;
    AlpcPortAttributes.SecurityQos.EffectiveOnly = false;
}

_Use_decl_annotations_
NTSTATUS 
AlpcTransportBase::InitializeMessageAndAttributes(
    _In_ ULONG MessageAttributesFlags,
    _In_opt_ const BYTE* Buffer,
    _In_ size_t BufferSize,
    _Out_ PALPC_CUSTOM_MESSAGE* AlpcMessage,
    _Out_ PALPC_MESSAGE_ATTRIBUTES* AlpcMessageAttributes
)
{
    *AlpcMessage = nullptr;
    *AlpcMessageAttributes = nullptr;

    if (BufferSize > USHRT_MAX)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    auto status = AlpcMessageInitialize(
        Buffer,                             // [in opt] Buffer : const BYTE*
        static_cast<USHORT>(BufferSize),    // [in] BufferSize : USHORT
        AlpcMessage                         // [out] AlpcMessage : PALPC_CUSTOM_MESSAGE*
    );
    if (!NT_SUCCESS(status))
    {
        goto CleanUp;
    }

    status = AlpcMessageAttributesInitialize(
        MessageAttributesFlags,     // [in] MessageAttributesFlags : ULONG
        AlpcMessageAttributes       // [out] MessageAttributes : PALPC_MESSAGE_ATTRIBUTES*
    );
    if (!NT_SUCCESS(status))
    {
        goto CleanUp;
    }

CleanUp:
    if (!NT_SUCCESS(status))
    {
        AlpcMessageUninitialize(AlpcMessage);
        AlpcMessageAttributesUninitialize(AlpcMessageAttributes);
    }
    return status;
}
