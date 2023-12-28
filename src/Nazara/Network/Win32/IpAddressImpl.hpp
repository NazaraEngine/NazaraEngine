// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NETWORK_WIN32_IPADDRESSIMPL_HPP
#define NAZARA_NETWORK_WIN32_IPADDRESSIMPL_HPP

#include <Nazara/Network/IpAddress.hpp>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <string>

namespace Nz
{
	class IpAddressImpl
	{
		public:
			using SockAddrBuffer = std::array<UInt8, sizeof(sockaddr_in6)>;

			IpAddressImpl() = delete;
			~IpAddressImpl() = delete;

			static IpAddress FromAddrinfo(const addrinfo* info);
#if NAZARA_UTILS_WINDOWS_NT6
			static IpAddress FromAddrinfo(const addrinfoW* info);
	#endif
			static IpAddress FromSockAddr(const sockaddr* address);
			static IpAddress FromSockAddr(const sockaddr_in* addressv4);
			static IpAddress FromSockAddr(const sockaddr_in6* addressv6);

			static bool ResolveAddress(const IpAddress& ipAddress, std::string* hostname, std::string* service, ResolveError* error);
			static std::vector<HostnameInfo> ResolveHostname(NetProtocol procol, const std::string& hostname, const std::string& service, ResolveError* error);

			static socklen_t ToSockAddr(const IpAddress& ipAddress, void* buffer);
			static NetProtocol TranslatePFToNetProtocol(int family);
			static SocketType TranslateSockToNetProtocol(int socketType);
			static ResolveError TranslateWSAErrorToResolveError(int error);
	};
}

#endif // NAZARA_NETWORK_WIN32_IPADDRESSIMPL_HPP
