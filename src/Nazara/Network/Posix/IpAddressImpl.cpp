// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Posix/IpAddressImpl.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Network/Posix/SocketImpl.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <cstring>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	IpAddress IpAddressImpl::FromAddrinfo(const addrinfo* info)
	{
		switch (info->ai_family)
		{
			case AF_INET:
			{
				sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(info->ai_addr);

				return FromSockAddr(ipv4);
			}

			case AF_INET6:
			{
				sockaddr_in6* ipv6 = reinterpret_cast<sockaddr_in6*>(info->ai_addr);

				return FromSockAddr(ipv6);
			}
		}

		return IpAddress::Invalid;
	}

	IpAddress IpAddressImpl::FromSockAddr(const sockaddr* address)
	{
		switch (address->sa_family)
		{
			case AF_INET:
				return FromSockAddr(reinterpret_cast<const sockaddr_in*>(address));

			case AF_INET6:
				return FromSockAddr(reinterpret_cast<const sockaddr_in6*>(address));
		}

		return IpAddress::Invalid;
	}

	IpAddress IpAddressImpl::FromSockAddr(const sockaddr_in* addressv4)
	{
		IpAddress::IPv4 ipv4;
		std::memcpy(&ipv4[0], &addressv4->sin_addr.s_addr, sizeof(uint32_t)); //< addr.s_addr is in big-endian so its already correctly ordered

		return IpAddress(ipv4, ntohs(addressv4->sin_port));
	}

	IpAddress IpAddressImpl::FromSockAddr(const sockaddr_in6* addressv6)
	{
		auto& rawIpV6 = addressv6->sin6_addr.s6_addr;

		IpAddress::IPv6 ipv6;
		for (unsigned int i = 0; i < 8; ++i)
			ipv6[i] = UInt16(rawIpV6[i * 2]) << 8 | rawIpV6[i * 2 + 1];

		return IpAddress(ipv6, ntohs(addressv6->sin6_port));
	}

	bool IpAddressImpl::ResolveAddress(const IpAddress& ipAddress, std::string* hostname, std::string* service, ResolveError* error)
	{
		SockAddrBuffer socketAddress;
		socklen_t socketAddressLen = ToSockAddr(ipAddress, socketAddress.data());

		std::array<char, NI_MAXHOST> hostnameBuffer;
		std::array<char, NI_MAXSERV> serviceBuffer;

		if (getnameinfo(reinterpret_cast<sockaddr*>(socketAddress.data()), socketAddressLen, hostnameBuffer.data(), hostnameBuffer.size(), serviceBuffer.data(), serviceBuffer.size(), NI_NUMERICSERV) != 0)
		{
			if (error)
				*error = TranslateEAIErrorToResolveError(errno);

			return false;
		}

		if (hostname)
			hostname->assign(hostnameBuffer.data());

		if (service)
			service->assign(serviceBuffer.data());

		if (error)
			*error = ResolveError::NoError;

		return true;
	}

	std::vector<HostnameInfo> IpAddressImpl::ResolveHostname(NetProtocol procol, const std::string& hostname, const std::string& service, ResolveError* error)
	{
		std::vector<HostnameInfo> results;

		addrinfo hints;
		std::memset(&hints, 0, sizeof(addrinfo));
		hints.ai_family = SocketImpl::TranslateNetProtocolToAF(procol);
		hints.ai_flags = AI_CANONNAME;
		hints.ai_socktype = SOCK_STREAM;

		addrinfo* servinfo;
		if (getaddrinfo(hostname.c_str(), service.c_str(), &hints, &servinfo) != 0)
		{
			if (error)
				*error = TranslateEAIErrorToResolveError(errno);

			return results;
		}

		CallOnExit onExit([servinfo]()
		{
			freeaddrinfo(servinfo);
		});

		// loop through all the results and connect to the first we can
		for (addrinfo* p = servinfo; p != nullptr; p = p->ai_next)
		{
			HostnameInfo result;
			result.address = FromAddrinfo(p);
			result.protocol = TranslatePFToNetProtocol(p->ai_family);
			result.socketType = TranslateSockToNetProtocol(p->ai_socktype);

			if (p->ai_canonname)
				result.canonicalName = p->ai_canonname;

			results.push_back(result);
		}

		if (error)
			*error = ResolveError::NoError;

		return results;
	}

	socklen_t IpAddressImpl::ToSockAddr(const IpAddress& ipAddress, void* buffer)
	{
		if (ipAddress.IsValid())
		{
			switch (ipAddress.GetProtocol())
			{
				case NetProtocol::IPv4:
				{
					sockaddr_in* socketAddress = reinterpret_cast<sockaddr_in*>(buffer);

					std::memset(socketAddress, 0, sizeof(sockaddr_in));
					socketAddress->sin_family = AF_INET;
					socketAddress->sin_port = htons(ipAddress.GetPort());
					socketAddress->sin_addr.s_addr = htonl(ipAddress.ToUInt32());

					return sizeof(sockaddr_in);
				}

				case NetProtocol::IPv6:
				{
					sockaddr_in6* socketAddress = reinterpret_cast<sockaddr_in6*>(buffer);

					std::memset(socketAddress, 0, sizeof(sockaddr_in6));
					socketAddress->sin6_family = AF_INET6;
					socketAddress->sin6_port = htons(ipAddress.GetPort());

					IpAddress::IPv6 address = ipAddress.ToIPv6();
					for (unsigned int i = 0; i < 8; ++i)
					{
						UInt16 addressPart = htons(address[i]);
						socketAddress->sin6_addr.s6_addr[i * 2 + 0] = addressPart >> 0;
						socketAddress->sin6_addr.s6_addr[i * 2 + 1] = addressPart >> 8;
					}

					return sizeof(sockaddr_in6);
				}

				default:
					NazaraInternalErrorFmt("unhandled ip protocol ({0:#x})", UnderlyingCast(ipAddress.GetProtocol()));
					break;
			}
		}

		NazaraError("invalid ip address");
		return 0;
	}

	NetProtocol IpAddressImpl::TranslatePFToNetProtocol(int family)
	{
		switch (family)
		{
			case PF_INET:
				return NetProtocol::IPv4;

			case PF_INET6:
				return NetProtocol::IPv6;

			default:
				return NetProtocol::Unknown;
		}
	}

	SocketType IpAddressImpl::TranslateSockToNetProtocol(int socketType)
	{
		switch (socketType)
		{
			case SOCK_STREAM:
				return SocketType::TCP;

			case SOCK_DGRAM:
				return SocketType::UDP;

			case SOCK_RAW:
				return SocketType::Raw;

			default:
				return SocketType::Unknown;
		}
	}

	ResolveError IpAddressImpl::TranslateEAIErrorToResolveError(int error)
	{
		// http://man7.org/linux/man-pages/man3/gai_strerror.3.html
		switch (error)
		{
			case 0:
				return ResolveError::NoError;

			// Engine error
			case EAI_BADFLAGS:
			case EAI_SYSTEM:
				return ResolveError::Internal;

			case EAI_FAMILY:
			case EAI_SERVICE:
			case EAI_SOCKTYPE:
				return ResolveError::ProtocolNotSupported;

			case EAI_NONAME:
				return ResolveError::NotFound;

			case EAI_FAIL:
				return ResolveError::NonRecoverable;

			case EAI_NODATA:
				return ResolveError::NotInitialized;

			case EAI_MEMORY:
				return ResolveError::ResourceError;

			case EAI_AGAIN:
				return ResolveError::TemporaryFailure;
		}

		NazaraWarning("Unhandled EAI error: " + Error::GetLastSystemError(error) + " (" + NumberToString(error) + ") as " + gai_strerror(error));
		return ResolveError::Unknown;
	}
}
