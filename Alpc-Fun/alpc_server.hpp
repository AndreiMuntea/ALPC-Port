#ifndef __ALPC_SERVER_HPP__
#define __ALPC_SERVER_HPP__

#include "alpc_transport_base.hpp"

#include <vector>

class AlpcServer : public AlpcTransportBase
{
public:
    AlpcServer(_In_ PUNICODE_STRING AlpcPortName);
    virtual ~AlpcServer();

    _Must_inspect_result_
    NTSTATUS
    ReceiveAlpcMessage(
        _Out_ PALPC_CUSTOM_MESSAGE* Message
    );

    _Must_inspect_result_
    NTSTATUS 
    HandleConnectionRequest(
        _In_ PALPC_CUSTOM_MESSAGE Message
    );

    _Must_inspect_result_
    NTSTATUS
    HandleMessage(
        _In_ PALPC_CUSTOM_MESSAGE Message
    );

private:
    _Must_inspect_result_
    NTSTATUS
    CreateAlpcPort(
        _In_ PUNICODE_STRING AlpcPortName
    );

    HANDLE m_AlpcServerPortHandle = nullptr;
    std::vector<HANDLE> m_AlpcClientPorts;
};

#endif //__ALPC_SERVER_HPP__