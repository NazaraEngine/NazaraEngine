// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_NETWORK_HPP
#define NAZARA_ENUMS_NETWORK_HPP

namespace Nz
{
	enum ResolveError
	{
		ResolveError_NoError,

		ResolveError_Internal,             //< An internal error occured
		ResolveError_ResourceError,        //< The operating system lacks the resources to proceed (insufficient memory)
		ResolveError_NonRecoverable,       //< An nonrecoverable error occured
		ResolveError_NotFound,             //< No such host is known
		ResolveError_NotInitialized,       //< Nazara network has not been initialized
		ResolveError_ProtocolNotSupported, //< A specified protocol is not supported by the server
		ResolveError_TemporaryFailure,     //< A temporary failure occured, try again
		ResolveError_Unknown,              //< The last operation failed with an unlisted error code

		ResolveError_Max = ResolveError_TemporaryFailure
	};

	enum NetProtocol
	{
		NetProtocol_Any,
		NetProtocol_IPv4,
		NetProtocol_IPv6,

		NetProtocol_Max = NetProtocol_IPv6
	};

	enum SocketError
	{
		SocketError_NoError,

		SocketError_AddressNotAvailable,      //< The address is already in use (when binding/listening)
		SocketError_ConnectionClosed,         //< The connection has been closed
		SocketError_ConnectionRefused,        //< The connection attempt was refused
		SocketError_DatagramSize,             //< The datagram size is over the system limit
		SocketError_Internal,                 //< The error is coming from the engine
		SocketError_NetworkError,             //< The network system has failed (maybe network is down)
		SocketError_NotInitialized,           //< Nazara network has not been initialized
		SocketError_NotSupported,             //< The operation is not supported (e.g. creating a bluetooth socket on a system without any bluetooth adaptater)
		SocketError_ResourceError,            //< The operating system lacks the resources to proceed (e.g. memory/socket descriptor)
		SocketError_UnreachableHost,          //< The host is not reachable
		SocketError_TimedOut,                 //< The operation timed out
		SocketError_Unknown,                  //< The last operation failed with an unlisted error code
		
		SocketError_Max = SocketError_Unknown
	};

	enum SocketState
	{
		SocketState_Bound,        //< The socket is currently bound
		SocketState_Connecting,   //< The socket is currently connecting
		SocketState_Connected,    //< The socket is currently connected
		SocketState_NotConnected, //< The socket is not connected (or has been disconnected)

		SocketState_Max = SocketState_NotConnected
	};

	enum SocketType
	{
		SocketType_Raw,
		SocketType_TCP,
		SocketType_UDP,

		SocketType_Max = SocketType_UDP
	};
}

#endif // NAZARA_ENUMS_NETWORK_HPP
