#include "alpc_message.hpp"

_Use_decl_annotations_
NTSTATUS
AlpcMessageInitialize(
    _In_opt_ const BYTE* Buffer,
    _In_ USHORT BufferSize,
    _Out_ PALPC_CUSTOM_MESSAGE* AlpcMessage
)
{
    *AlpcMessage = nullptr;

    USHORT totalSize = BufferSize + sizeof(PORT_MESSAGE);
    if (totalSize < sizeof(PORT_MESSAGE))
    {
        return STATUS_INTEGER_OVERFLOW;
    }

    auto alpcMessage = (PALPC_CUSTOM_MESSAGE)malloc(totalSize);
    if (!alpcMessage)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    
    RtlZeroMemory(alpcMessage, totalSize);
    alpcMessage->Header.u1.s1.TotalLength = totalSize;

    if (Buffer)
    {
        RtlCopyMemory(&alpcMessage->Buffer, Buffer, BufferSize);
        alpcMessage->Header.u1.s1.DataLength = BufferSize;
    }

    *AlpcMessage = alpcMessage;
    return STATUS_SUCCESS;
}

VOID
AlpcMessageUninitialize(
    _Inout_ PALPC_CUSTOM_MESSAGE* AlpcMessage
)
{
    if (*AlpcMessage)
    {
        free(*AlpcMessage);
        *AlpcMessage = nullptr;
    }
}