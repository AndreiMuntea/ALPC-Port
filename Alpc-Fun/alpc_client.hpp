#ifndef __ALPC_CLIENT_HPP__
#define __ALPC_CLIENT_HPP__ 

#include "alpc_transport_base.hpp"

#include <vector>

class AlpcClient : public AlpcTransportBase
{
public:
    AlpcClient(_In_ PUNICODE_STRING AlpcPortName);
    virtual ~AlpcClient();

    _Must_inspect_result_
    NTSTATUS
    SendAlpcMessage(
        _In_ const std::wstring& Message
    );

private:
    _Must_inspect_result_
    NTSTATUS
    ConnectToAlpcPort(
        _In_ PUNICODE_STRING AlpcPortName
    );

    HANDLE m_AlpcPortHandle = nullptr;
};

#endif //__ALPC_CLIENT_HPP__