// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_NETWORK_HPP
#define NAZARA_ENUMS_NETWORK_HPP

namespace Nz
{
	enum NetCode : UInt16
	{
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
		SocketError_Packet,              //< The packet encoding/decoding failed, probably because of corrupted data
		SocketError_NetworkError,        //< The network system has failed (maybe network is down)
		SocketError_NotInitialized,      //< Nazara network has not been initialized
		SocketError_NotSupported,        //< The operation is not supported (e.g. creating a bluetooth socket on a system without any bluetooth adaptater)
		SocketError_ResolveError,        //< The hostname couldn't be resolved (more information in ResolveError code)
		SocketError_ResourceError,       //< The operating system lacks the resources to proceed (e.g. memory/socket descriptor)
		SocketError_TimedOut,            //< The operation timed out
		SocketError_Unknown,             //< The last operation failed with an unlisted error code
		SocketError_UnreachableHost,     //< The host is not reachable

		SocketError_Max = SocketError_UnreachableHost
	};

	enum SocketState
	{
		SocketState_Bound,        //< The socket is currently bound
		SocketState_Connecting,   //< The socket is currently connecting
		SocketState_Connected,    //< The socket is currently connected
		SocketState_NotConnected, //< The socket is not connected (or has been disconnected)
		SocketState_Resolving,    //< The socket is currently resolving a host name

		SocketState_Max = SocketState_NotConnected
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
