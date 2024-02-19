// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_NETWORK_POSIX_IPADDRESSIMPL_HPP
#define NAZARA_NETWORK_POSIX_IPADDRESSIMPL_HPP

#include <Nazara/Network/IpAddress.hpp>
#include <string>
#include <netdb.h>
#include <netinet/in.h>

namespace Nz
{
	class IpAddressImpl
	{
		public:
			using SockAddrBuffer = std::array<UInt8, sizeof(sockaddr_in6)>;

			IpAddressImpl() = delete;
			~IpAddressImpl() = delete;

			static IpAddress FromAddrinfo(const addrinfo* info);
			static IpAddress FromSockAddr(const sockaddr* address);
			static IpAddress FromSockAddr(const sockaddr_in* addressv4);
			static IpAddress FromSockAddr(const sockaddr_in6* addressv6);

			static bool ResolveAddress(const IpAddress& ipAddress, std::string* hostname, std::string* service, ResolveError* error);
			static std::vector<HostnameInfo> ResolveHostname(NetProtocol procol, const std::string& hostname, const std::string& service, ResolveError* error);

			static socklen_t ToSockAddr(const IpAddress& ipAddress, void* buffer);
			static NetProtocol TranslatePFToNetProtocol(int family);
			static SocketType TranslateSockToNetProtocol(int socketType);
			static ResolveError TranslateEAIErrorToResolveError(int error);
	};
}

#endif // NAZARA_NETWORK_POSIX_IPADDRESSIMPL_HPP
