#include "alpc_message_attributes.hpp"

_Use_decl_annotations_
NTSTATUS
AlpcMessageAttributesInitialize(
    _In_ ULONG MessageAttributesFlags,
    _Out_ PALPC_MESSAGE_ATTRIBUTES* MessageAttributes
)
{
    *MessageAttributes = nullptr;

    // First we compute the needed size
    SIZE_T requiredSize = 0;
    auto status = AlpcInitializeMessageAttribute(
        MessageAttributesFlags,     // [in] AttributeFlags : ULONG
        nullptr,                    // [out opt] Buffer : PALPC_MESSAGE_ATTRIBUTES
        0,                          // [in] BufferSize : ULONG
        &requiredSize               // [out] RequiredBufferSize : PULONG
    );

    // Something unexpected went wrong. We return a proper error status
    if (requiredSize == 0)
    {
        return (NT_SUCCESS(status)) ? STATUS_INFO_LENGTH_MISMATCH
                                    : status;
    }

    auto messageAttributes = (PALPC_MESSAGE_ATTRIBUTES)malloc(requiredSize);
    if (!messageAttributes)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    RtlZeroMemory(messageAttributes, requiredSize);

    // Now we perform the actual initialization
    status = AlpcInitializeMessageAttribute(
        MessageAttributesFlags,     // [in] AttributeFlags : ULONG
        messageAttributes,          // [out opt] Buffer : PALPC_MESSAGE_ATTRIBUTES
        requiredSize,               // [in] BufferSize : SIZE_T
        &requiredSize               // [out] RequiredBufferSize : PSIZE_T
    );
    if (!NT_SUCCESS(status))
    {
        free(messageAttributes);
        return status;
    }

    *MessageAttributes = messageAttributes;
    return STATUS_SUCCESS;
}

VOID
AlpcMessageAttributesUninitialize(
    _Inout_ PALPC_MESSAGE_ATTRIBUTES* MessageAttributes
)
{
    if (*MessageAttributes)
    {
        free(*MessageAttributes);
        *MessageAttributes = nullptr;
    }
}