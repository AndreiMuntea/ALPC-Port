#ifndef __ALPC_MESSAGE_ATTRIBUTES_HPP__
#define __ALPC_MESSAGE_ATTRIBUTES_HPP__

#include "ntinclude.h"

typedef enum _ALPC_MESSAGE_ATTRIBUTES_VALUES
{
    kAlpcMessageAttributesNone = 0,
    kAlpcMessageAttributesWorkOnBehalfOf = 0x2000000,
    kAlpcMessageAttributesDirect = 0x4000000,
    kAlpcMessageAttributesToken = 0x8000000,
    kAlpcMessageAttributesHandle = 0x10000000,
    kAlpcMessageAttributesContext = 0x20000000,
    kAlpcMessageAttributesView = 0x40000000,
    kAlpcMessageAttributesSecurity = 0x80000000,
}ALPC_MESSAGE_ATTRIBUTES_VALUES, *PALPC_MESSAGE_ATTRIBUTES_VALUES;

_Must_inspect_result_
NTSTATUS
AlpcMessageAttributesInitialize(
    _In_ ULONG MessageAttributesFlags,
    _Out_ PALPC_MESSAGE_ATTRIBUTES* MessageAttributes
);

VOID
AlpcMessageAttributesUninitialize(
    _Inout_ PALPC_MESSAGE_ATTRIBUTES* MessageAttributes
);

#endif //__ALPC_MESSAGE_ATTRIBUTES_HPP__