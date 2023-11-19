#include "alpc_client.hpp"

#include <iostream>

AlpcClient::AlpcClient(
    _In_ PUNICODE_STRING AlpcPortName
) : AlpcTransportBase{}
{
    std::wcout << "Connecting to port: " << AlpcPortName->Buffer << std::endl;
    auto status = ConnectToAlpcPort(AlpcPortName);
    if (!NT_SUCCESS(status))
    {
        std::cout << "ConnectToAlpcPort failed with NTSTATUS = " << std::hex << status << std::dec << std::endl;
        throw std::exception("Unable to connect to ALPC port");
    }
}

AlpcClient::~AlpcClient()
{
    DisconnectAlpcPort(&this->m_AlpcPortHandle);
}

_Use_decl_annotations_
NTSTATUS 
AlpcClient::SendAlpcMessage(
    _In_ const std::wstring & Message
)
{
    return AlpcTransportBase::SendAlpcMessage(Message, this->m_AlpcPortHandle);
}

NTSTATUS 
AlpcClient::ConnectToAlpcPort(
    _In_ PUNICODE_STRING AlpcPortName
)
{
    PALPC_CUSTOM_MESSAGE connectMessage = nullptr;
    SIZE_T connectMessageLength = 0;
    ALPC_PORT_ATTRIBUTES alpcPortAttributes = { 0 };
    
    InitializeAlpcPortAttributes(alpcPortAttributes);

    auto status = AlpcMessageInitialize(nullptr, 0, &connectMessage);
    if (!NT_SUCCESS(status))
    {
        return status;
    }
    connectMessageLength = connectMessage->Header.u1.s1.TotalLength;

    status = NtAlpcConnectPort(
        &this->m_AlpcPortHandle,                            // [out] PortHandle : PHANDLE
        AlpcPortName,                                       // [in] PortName : PUNICODE_STRING
        nullptr,                                            // [in opt] ObjectAttributes : PALPC_PORT_ATTRIBUTES
        &alpcPortAttributes,                                // [in opt] PortAttributes : PALPC_PORT_ATTRIBUTES
        ALPC_MESSAGE_FLAGS::kAlpcMessageFlagSyncRequest,    // [in] Flags : ULONG
        nullptr,                                            // [in opt] RequiredServerSid : PSID
        &connectMessage->Header,                            // [inout opt] ConnectionMessage : PPORT_MESSAGE
        &connectMessageLength,                              // [inout opt] BufferLength : PSIZE_T
        nullptr,                                            // [inout opt] OutMessageAttributes : PALPC_MESSAGE_ATTRIBUTES
        nullptr,                                            // [inout opt] InMessageAttributes : PALPC_MESSAGE_ATTRIBUTES
        nullptr                                             // [in opt] Timeout : PLARGE_INTEGER
    );

    AlpcMessageUninitialize(&connectMessage);
    return status;
}
