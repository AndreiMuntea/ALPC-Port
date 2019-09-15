#include "alpc_server.hpp"

#include <iostream>
#include <string>

_Use_decl_annotations_
AlpcServer::AlpcServer(PUNICODE_STRING AlpcPortName) : AlpcTransportBase{}
{
    auto status = CreateAlpcPort(AlpcPortName);
    if (!NT_SUCCESS(status))
    {
        std::cout << "CreateAlpcPort failed with NTSTATUS = " << std::hex << status << std::dec << std::endl;
        throw std::exception("Unable to create ALPC port");
    }

    std::wcout << "Created ALPC port : " << AlpcPortName->Buffer << std::endl;
}

AlpcServer::~AlpcServer()
{
    DisconnectAlpcPort(&this->m_AlpcServerPortHandle);
    for (auto& clientPort : m_AlpcClientPorts)
    {
        DisconnectAlpcPort(&clientPort);
    }
}

_Use_decl_annotations_
NTSTATUS 
AlpcServer::ReceiveAlpcMessage(
    _Out_ PALPC_CUSTOM_MESSAGE * Message
)
{
    return AlpcTransportBase::ReceiveAlpcMessage(Message, this->m_AlpcServerPortHandle);
}

_Use_decl_annotations_
NTSTATUS
AlpcServer::CreateAlpcPort(
    _In_ PUNICODE_STRING AlpcPortName
)
{
    OBJECT_ATTRIBUTES alpcPortObjectAttributes = { 0 };
    ALPC_PORT_ATTRIBUTES alpcPortAttributes = { 0 };

    InitializeAlpcPortAttributes(alpcPortAttributes);
    InitializeObjectAttributes(&alpcPortObjectAttributes, AlpcPortName, 0, nullptr, nullptr);

    return NtAlpcCreatePort(
        &this->m_AlpcServerPortHandle,  // [out] PortHandle : PHANDLE
        &alpcPortObjectAttributes,      // [in opt] ObjectAttributes : POBJECT_ATTRIBUTES
        &alpcPortAttributes             // [in opt] PortAttributes : PALPC_PORT_ATTRIBUTES
    );
}

_Use_decl_annotations_
NTSTATUS
AlpcServer::HandleConnectionRequest(
    _In_ PALPC_CUSTOM_MESSAGE Message
)
{
    HANDLE clientPortHandle = nullptr;
    ALPC_PORT_ATTRIBUTES alpcPortAttributes = { 0 };

    InitializeAlpcPortAttributes(alpcPortAttributes);

    auto status = NtAlpcAcceptConnectPort(
        &clientPortHandle,                          // [out] PortHandle : PHANDLE
        this->m_AlpcServerPortHandle,               // [in] ConnectionPortHandle : HANDLE
        ALPC_MESSAGE_FLAGS::kAlpcMessageFlagNone,   // [in] Flags : ULONG
        nullptr,                                    // [in opt] ObjectAttributes : POBJECT_ATTRIBUTES
        &alpcPortAttributes,                        // [in opt] PortAttributes : PALPC_PORT_ATTRIBUTES
        nullptr,                                    // [in opt] PortContext : PVOID
        &Message->Header,                           // [in] ConnectionRequest : PPORT_MESSAGE
        nullptr,                                    // [inout opt] ConnectionMessageAttributes : PALPC_MESSAGE_ATTRIBUTES
        true                                        // [in] AcceptConnection : BOOLEAN
    );
    if (NT_SUCCESS(status))
    {
        this->m_AlpcClientPorts.push_back(clientPortHandle);
    }
    return status;
}

_Use_decl_annotations_
NTSTATUS
AlpcServer::HandleMessage(
    _In_ PALPC_CUSTOM_MESSAGE Message
)
{
    auto rawBuffer = (wchar_t*)Message->Buffer;
    auto rawBufferSize = Message->Header.u1.s1.DataLength / sizeof(wchar_t);
    std::wcout << "New message! Size: " << rawBufferSize << std::endl << "Content:" << std::wstring(rawBuffer) << std::endl;

    // to force a termination for now
    return STATUS_SUCCESS;
}
