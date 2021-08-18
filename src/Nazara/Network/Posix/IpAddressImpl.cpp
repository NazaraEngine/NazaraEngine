// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Posix/IpAddressImpl.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Network/Posix/SocketImpl.hpp>
#include <cstring>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		using addrinfoImpl = addrinfo;

		int GetAddressInfo(const std::string& hostname, const std::string& service, const addrinfoImpl* hints, addrinfoImpl** results)
		{
			return getaddrinfo(hostname.c_str(), service.c_str(), hints, results);
		}

		int GetHostnameInfo(sockaddr* socketAddress, socklen_t socketLen, std::string* hostname, std::string* service, int flags)
		{
			std::array<char, NI_MAXHOST> hostnameBuffer;
			std::array<char, NI_MAXSERV> serviceBuffer;

			int result = getnameinfo(socketAddress, socketLen, hostnameBuffer.data(), hostnameBuffer.size(), serviceBuffer.data(), serviceBuffer.size(), flags);
			if (result == 0)
			{
				if (hostname)
					hostname->assign(hostnameBuffer.data());

				if (service)
					service->assign(serviceBuffer.data());
			}

			return result;
		}

		void FreeAddressInfo(addrinfoImpl* results)
		{
			freeaddrinfo(results);
		}

		IpAddress::IPv4 convertSockaddrToIPv4(const in_addr& addr)
		{
			union byteToInt
			{
				UInt8 b[sizeof(uint32_t)];
				uint32_t i;
			};

			byteToInt hostOrder;
			hostOrder.i = ntohl(addr.s_addr);

			return { {hostOrder.b[3], hostOrder.b[2], hostOrder.b[1], hostOrder.b[0]} };
		}

		IpAddress::IPv6 convertSockaddr6ToIPv6(const in6_addr& addr)
		{
			auto& rawIpV6 = addr.s6_addr;

			IpAddress::IPv6 ipv6;
			for (unsigned int i = 0; i < 8; ++i)
				ipv6[i] = Nz::UInt16(rawIpV6[i * 2]) << 8 | rawIpV6[i * 2 + 1];

			return ipv6;
		}
	}

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
		IpAddress::IPv4 ip4Address = Detail::convertSockaddrToIPv4(addressv4->sin_addr);
		return IpAddress(ip4Address, ntohs(addressv4->sin_port));
	}

	IpAddress IpAddressImpl::FromSockAddr(const sockaddr_in6* addressv6)
	{
		IpAddress::IPv6 ip6Address = Detail::convertSockaddr6ToIPv6(addressv6->sin6_addr);
		return IpAddress(ip6Address, ntohs(addressv6->sin6_port));
	}

	bool IpAddressImpl::ResolveAddress(const IpAddress& ipAddress, std::string* hostname, std::string* service, ResolveError* error)
	{
		SockAddrBuffer socketAddress;
		socklen_t socketAddressLen = ToSockAddr(ipAddress, socketAddress.data());

		if (Detail::GetHostnameInfo(reinterpret_cast<sockaddr*>(socketAddress.data()), socketAddressLen, hostname, service, NI_NUMERICSERV) != 0)
		{
			if (error)
				*error = TranslateEAIErrorToResolveError(errno);

			return false;
		}

		if (error)
			*error = ResolveError::NoError;

		return true;
	}

	std::vector<HostnameInfo> IpAddressImpl::ResolveHostname(NetProtocol procol, const std::string& hostname, const std::string& service, ResolveError* error)
	{
		std::vector<HostnameInfo> results;

		Detail::addrinfoImpl hints;
		std::memset(&hints, 0, sizeof(Detail::addrinfoImpl));
		hints.ai_family = SocketImpl::TranslateNetProtocolToAF(procol);
		hints.ai_flags = AI_CANONNAME;
		hints.ai_socktype = SOCK_STREAM;

		Detail::addrinfoImpl* servinfo;
		if (Detail::GetAddressInfo(hostname, service, &hints, &servinfo) != 0)
		{
			if (error)
				*error = TranslateEAIErrorToResolveError(errno);

			return results;
		}

		CallOnExit onExit([servinfo]()
		{
			Detail::FreeAddressInfo(servinfo);
		});

		// loop through all the results and connect to the first we can
		for (Detail::addrinfoImpl* p = servinfo; p != nullptr; p = p->ai_next)
		{
			HostnameInfo result;
			result.address = FromAddrinfo(p);
			result.canonicalName = p->ai_canonname;
			result.protocol = TranslatePFToNetProtocol(p->ai_family);
			result.socketType = TranslateSockToNetProtocol(p->ai_socktype);

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
						u_short addressPart = htons(address[i]);
						socketAddress->sin6_addr.s6_addr[i * 2 + 0] = addressPart >> 0;
						socketAddress->sin6_addr.s6_addr[i * 2 + 1] = addressPart >> 8;
					}

					return sizeof(sockaddr_in6);
				}

				default:
					NazaraInternalError("Unhandled ip protocol (0x" + NumberToString(UnderlyingCast(ipAddress.GetProtocol()), 16) + ')');
					break;
			}
		}

		NazaraError("Invalid ip address");
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
