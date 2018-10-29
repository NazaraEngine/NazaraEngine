// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_NETWORK_HPP
#define NAZARA_ENUMS_NETWORK_HPP

#include <Nazara/Core/Flags.hpp>

namespace Nz
{
	enum NetCode : UInt16
	{
		NetCode_Acknowledge           = 0x9A4E,
		NetCode_AcknowledgeConnection = 0xC108,
		NetCode_Ping                  = 0x96AC,
		NetCode_Pong                  = 0x974C,
		NetCode_RequestConnection     = 0xF27D,

		NetCode_Invalid = 0x0000
	};

	enum NetProtocol
	{
		NetProtocol_Any,
		NetProtocol_IPv4,
		NetProtocol_IPv6,
		NetProtocol_Unknown,

		NetProtocol_Max = NetProtocol_Unknown
	};

	enum PacketPriority
	{
		PacketPriority_High      = 1, //< High-priority packet, will be sent quickly
		PacketPriority_Immediate = 0, //< Immediate priority, will be sent immediately
		PacketPriority_Medium    = 2, //< Medium-priority packet, will be sent as regular
		PacketPriority_Low       = 3, //< Low-priority packet, may take some time to be sent

		PacketPriority_Lowest  = PacketPriority_Low,
		PacketPriority_Highest = PacketPriority_Immediate,
		PacketPriority_Max     = PacketPriority_Low
	};

	enum PacketReliability
	{
		PacketReliability_Reliable,        //< Packet will be resent if lost
		PacketReliability_ReliableOrdered, //< Packet will be resent if lost and will only arrive in order
		PacketReliability_Unreliable,      //< Packet won't be resent if lost

		PacketReliability_Max = PacketReliability_Unreliable
	};

	enum ResolveError
	{
		ResolveError_NoError,

		ResolveError_Internal,             //< An internal error occurred
		ResolveError_ResourceError,        //< The operating system lacks the resources to proceed (insufficient memory)
		ResolveError_NonRecoverable,       //< An nonrecoverable error occurred
		ResolveError_NotFound,             //< No such host is known
		ResolveError_NotInitialized,       //< Nazara network has not been initialized
		ResolveError_ProtocolNotSupported, //< A specified protocol is not supported by the server
		ResolveError_TemporaryFailure,     //< A temporary failure occurred, try again
		ResolveError_Unknown,              //< The last operation failed with an unlisted error code

		ResolveError_Max = ResolveError_Unknown
	};

	enum SocketError
	{
		SocketError_NoError,

		SocketError_AddressNotAvailable, //< The address is already in use (when binding/listening)
		SocketError_ConnectionClosed,    //< The connection has been closed
		SocketError_ConnectionRefused,   //< The connection attempt was refused
		SocketError_DatagramSize,        //< The datagram size is over the system limit
		SocketError_Internal,            //< The error is coming from the engine
		SocketError_Interrupted,         //< The operation was interrupted by a signal
		SocketError_Packet,              //< The packet encoding/decoding failed, probably because of corrupted data
		SocketError_NetworkError,        //< The network system has failed (maybe network is down)
		SocketError_NotInitialized,      //< Nazara network has not been initialized
		SocketError_NotSupported,        //< The operation is not supported (e.g. creating a bluetooth socket on a system without any bluetooth adapter)
		SocketError_ResolveError,        //< The hostname couldn't be resolved (more information in ResolveError code)
		SocketError_ResourceError,       //< The operating system lacks the resources to proceed (e.g. memory/socket descriptor)
		SocketError_TimedOut,            //< The operation timed out
		SocketError_Unknown,             //< The last operation failed with an unlisted error code
		SocketError_UnreachableHost,     //< The host is not reachable

		SocketError_Max = SocketError_UnreachableHost
	};

	enum SocketPollEvent
	{
		SocketPollEvent_Read,  //< One or more sockets is ready for a read operation
		SocketPollEvent_Write, //< One or more sockets is ready for a write operation

		SocketPollEvent_Max = SocketPollEvent_Write
	};

	template<>
	struct EnumAsFlags<SocketPollEvent>
	{
		static constexpr SocketPollEvent max = SocketPollEvent_Max;
	};

	using SocketPollEventFlags = Flags<SocketPollEvent>;

	enum SocketState
	{
		SocketState_Bound,        //< The socket is currently bound
		SocketState_Connecting,   //< The socket is currently connecting
		SocketState_Connected,    //< The socket is currently connected
		SocketState_NotConnected, //< The socket is not connected (or has been disconnected)
		SocketState_Resolving,    //< The socket is currently resolving a host name

		SocketState_Max = SocketState_Resolving
	};

	enum SocketType
	{
		SocketType_Raw,
		SocketType_TCP,
		SocketType_UDP,
		SocketType_Unknown,

		SocketType_Max = SocketType_Unknown
	};
}

#endif // NAZARA_ENUMS_NETWORK_HPP
