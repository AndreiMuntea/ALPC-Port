#ifndef __NTALPCAPI_H__
#define __NTALPCAPI_H__

#include "ntdefs.h"
#include "ntalpc.h"


EXTERN_C_START
//
// Used to create the main server connection port.
// This port is used by the server to listen for incoming connections.
//
NTSYSCALLAPI
NTSTATUS NTAPI
NtAlpcCreatePort(
    _Out_ PHANDLE PortHandle,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PALPC_PORT_ATTRIBUTES PortAttributes
);

//
// Used by client to attempt connection to a server connection port.
// If connection is accepted by the server, it returns a port handle.
// This handle should be used to further communicate with the server.
// 
NTSYSCALLAPI
NTSTATUS NTAPI
NtAlpcConnectPort(
    _Out_ PHANDLE PortHandle,
    _In_ PUNICODE_STRING PortName,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PALPC_PORT_ATTRIBUTES PortAttributes,
    _In_ ULONG Flags,
    _In_opt_ PSID RequiredServerSid,
    _Inout_updates_bytes_to_opt_(*BufferLength, *BufferLength) PPORT_MESSAGE ConnectionMessage,
    _Inout_opt_ PSIZE_T BufferLength,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES OutMessageAttributes,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES InMessageAttributes,
    _In_opt_ PLARGE_INTEGER Timeout
);

//
// Used by server to accept or decline incoming connections,
// It returns to the server a new port handle used to communicate
// with the client whose connection has been accepted.
//
NTSYSCALLAPI
NTSTATUS NTAPI
NtAlpcAcceptConnectPort(
    _Out_ PHANDLE PortHandle,
    _In_ HANDLE ConnectionPortHandle,
    _In_ ULONG Flags,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PALPC_PORT_ATTRIBUTES PortAttributes,
    _In_opt_ PVOID PortContext,
    _In_reads_bytes_(ConnectionRequest->u1.s1.TotalLength) PPORT_MESSAGE ConnectionRequest,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES ConnectionMessageAttributes,
    _In_ BOOLEAN AcceptConnection
);

//
// Used to send messages across a received port handle.
//
NTSYSCALLAPI
NTSTATUS NTAPI
NtAlpcSendWaitReceivePort(
    _In_ HANDLE PortHandle,
    _In_ ULONG Flags,
    _In_reads_bytes_opt_(SendMessage->u1.s1.TotalLength) PPORT_MESSAGE SendMessage,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES SendMessageAttributes,
    _Out_writes_bytes_to_opt_(*BufferLength, *BufferLength) PPORT_MESSAGE ReceiveMessage,
    _Inout_opt_ PSIZE_T BufferLength,
    _Inout_opt_ PALPC_MESSAGE_ATTRIBUTES ReceiveMessageAttributes,
    _In_opt_ PLARGE_INTEGER Timeout
);

//
// Used by client to disconnect communication port from server port
// Flags value can be one from ALPC_DISCONNECT_PORT_FLAGS values
//
NTSYSCALLAPI
NTSTATUS NTAPI
NtAlpcDisconnectPort(
    _In_ HANDLE PortHandle,
    _In_ ULONG Flags
);

//
// Server process should create new thread starting from execution of NtAlpcAcceptConnectPort. 
// Main thread should call NtListenPort again to make possible for other processes to connect to port.
// PortHandle is a HANDLE to named Port Object, created with NtCreatePort.
// ConnectionRequest represent the result of call - pointer to structure PPORT_MESSAGE filled with incoming connection data.
// This call is blocking, and cannot be interrupted, even if the handle is closed. 
// The only way how to stop the block is to send an LPC request which will be recognized by server thread as "Stop" command
//
NTSYSCALLAPI
NTSTATUS NTAPI
NtListenPort(
    _In_ HANDLE PortHandle,
    _Out_ PPORT_MESSAGE ConnectionRequest
);

//
// Is called by server process after all initializations for new connection.
// PortHandle is a HANDLE to Port Object received with NtAlpcAcceptConnectPort call.
//
NTSYSCALLAPI
NTSTATUS NTAPI
NtCompleteConnectPort(
    _In_ HANDLE PortHandle
);

NTSYSAPI
NTSTATUS NTAPI
AlpcInitializeMessageAttribute(
    _In_ ULONG AttributeFlags,
    _Out_opt_ PALPC_MESSAGE_ATTRIBUTES Buffer,
    _In_ SIZE_T BufferSize,
    _Out_ PSIZE_T RequiredBufferSize
);

EXTERN_C_END

#endif //__NTALPCAPI_H__