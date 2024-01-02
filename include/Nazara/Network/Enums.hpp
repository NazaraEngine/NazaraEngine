// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NETWORK_ENUMS_HPP
#define NAZARA_NETWORK_ENUMS_HPP

#include <NazaraUtils/Flags.hpp>

namespace Nz
{
	enum class NetProtocol
	{
		Any,
		IPv4,
		IPv6,
		Unknown,

		Max = Unknown
	};

	constexpr std::size_t NetProtocolCount = static_cast<std::size_t>(NetProtocol::Max) + 1;

	enum class PacketReliability
	{
		Reliable,        //< Packet will be resent if lost
		ReliableOrdered, //< Packet will be resent if lost and will only arrive in order
		Unreliable,      //< Packet won't be resent if lost

		Max = Unreliable
	};

	constexpr std::size_t PacketReliabilityCount = static_cast<std::size_t>(PacketReliability::Max) + 1;

	enum class ResolveError
	{
		NoError,

		Internal,             //< An internal error occurred
		ResourceError,        //< The operating system lacks the resources to proceed (insufficient memory)
		NonRecoverable,       //< An nonrecoverable error occurred
		NotFound,             //< No such host is known
		NotInitialized,       //< Nazara network has not been initialized
		ProtocolNotSupported, //< A specified protocol is not supported by the server
		TemporaryFailure,     //< A temporary failure occurred, try again
		Unknown,              //< The last operation failed with an unlisted error code

		Max = Unknown
	};

	constexpr std::size_t ResolveErrorCount = static_cast<std::size_t>(ResolveError::Max) + 1;

	enum class SocketError
	{
		NoError,

		AddressNotAvailable, //< The address is already in use (when binding/listening)
		ConnectionClosed,    //< The connection has been closed
		ConnectionRefused,   //< The connection attempt was refused
		DatagramSize,        //< The datagram size is over the system limit
		Internal,            //< The error is coming from the engine
		Interrupted,         //< The operation was interrupted by a signal
		Packet,              //< The packet encoding/decoding failed, probably because of corrupted data
		NetworkError,        //< The network system has failed (maybe network is down)
		NotInitialized,      //< Nazara network has not been initialized
		NotSupported,        //< The operation is not supported (e.g. creating a bluetooth socket on a system without any bluetooth adapter)
		ResolveError,        //< The hostname couldn't be resolved (more information in ResolveError code)
		ResourceError,       //< The operating system lacks the resources to proceed (e.g. memory/socket descriptor)
		TimedOut,            //< The operation timed out
		Unknown,             //< The last operation failed with an unlisted error code
		UnreachableHost,     //< The host is not reachable

		Max = UnreachableHost
	};

	constexpr std::size_t SocketErrorCount = static_cast<std::size_t>(SocketError::Max) + 1;

	enum class SocketPollEvent
	{
		Read,  //< One or more sockets is ready for a read operation
		Write, //< One or more sockets is ready for a write operation

		Max = Write
	};

	constexpr std::size_t SocketPollEventCount = static_cast<std::size_t>(SocketPollEvent::Max) + 1;

	template<>
	struct EnumAsFlags<SocketPollEvent>
	{
		static constexpr SocketPollEvent max = SocketPollEvent::Max;
	};

	using SocketPollEventFlags = Flags<SocketPollEvent>;

	enum class SocketState
	{
		Bound,        //< The socket is currently bound
		Connecting,   //< The socket is currently connecting
		Connected,    //< The socket is currently connected
		NotConnected, //< The socket is not connected (or has been disconnected)
		Resolving,    //< The socket is currently resolving a host name

		Max = Resolving
	};

	constexpr std::size_t SocketStateCount = static_cast<std::size_t>(SocketState::Max) + 1;

	enum class SocketType
	{
		Raw,
		TCP,
		UDP,
		Unknown,

		Max = Unknown
	};

	constexpr std::size_t SocketTypeCount = static_cast<std::size_t>(SocketType::Max) + 1;
}

#endif // NAZARA_NETWORK_ENUMS_HPP
