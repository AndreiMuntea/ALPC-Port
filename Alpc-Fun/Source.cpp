#include "alpc_client.hpp"
#include "alpc_server.hpp"

#include <iostream>
#include <thread>

DWORD WINAPI StartServerRoutine(PVOID Context);
DWORD WINAPI StartClientRoutine(PVOID Context);

static UNICODE_STRING gAlpcPortName = RTL_CONSTANT_STRING(L"\\RPC Control\\MyCoolDummyAlpcPort");

int main()
{
    std::thread t1(StartServerRoutine, nullptr);
    std::thread t2(StartClientRoutine, nullptr);

    t1.join();
    t2.join();
    
    return 0;
}

DWORD WINAPI StartServerRoutine(PVOID Context)
{
    UNREFERENCED_PARAMETER(Context);

    AlpcServer server{ &gAlpcPortName };
    PALPC_CUSTOM_MESSAGE message = nullptr;

    // wait for a client
    auto status = server.ReceiveAlpcMessage(&message);
    if (!NT_SUCCESS(status))
    {
        std::cout << "ReceiveAlpcMessage failed with NTSTATUS = " << std::hex << status << std::dec << std::endl;
        goto CleanUp;
    }
    status = server.HandleConnectionRequest(message);
    if (!NT_SUCCESS(status))
    {
        std::cout << "HandleConnectionRequest failed with NTSTATUS = " << std::hex << status << std::dec << std::endl;
        goto CleanUp;
    }
    AlpcMessageUninitialize(&message);

    // now wait for an actual message;
    status = server.ReceiveAlpcMessage(&message);
    if (!NT_SUCCESS(status))
    {
        std::cout << "ReceiveAlpcMessage failed with NTSTATUS = " << std::hex << status << std::dec << std::endl;
        goto CleanUp;
    }
    status = server.HandleMessage(message);
    if (!NT_SUCCESS(status))
    {
        std::cout << "HandleMessage failed with NTSTATUS = " << std::hex << status << std::dec << std::endl;
        goto CleanUp;
    }
    AlpcMessageUninitialize(&message);

CleanUp:
    AlpcMessageUninitialize(&message);
    return 0;
}
DWORD WINAPI StartClientRoutine(PVOID Context)
{
    UNREFERENCED_PARAMETER(Context);

    Sleep(1000); // Give some time to create the ALPC port
    AlpcClient client{ &gAlpcPortName };
    
    auto status = client.SendAlpcMessage(L"Ana are mere");
    if (!NT_SUCCESS(status))
    {
        std::cout << "SendAlpcMessage failed with NTSTATUS = " << std::hex << status << std::dec << std::endl;
        goto CleanUp;
    }

    Sleep(1000); // Give some time so the message will be sent
CleanUp:
    return 0;
}