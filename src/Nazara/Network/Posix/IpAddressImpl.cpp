// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Posix/IpAddressImpl.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Network/Posix/SocketImpl.hpp>
#include <cstring>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		using addrinfoImpl = addrinfo;

		int GetAddressInfo(const String& hostname, const String& service, const addrinfoImpl* hints, addrinfoImpl** results)
		{
			return getaddrinfo(hostname.GetConstBuffer(), service.GetConstBuffer(), hints, results);
		}

		int GetHostnameInfo(sockaddr* socketAddress, socklen_t socketLen, String* hostname, String* service, int flags)
		{
			std::array<char, NI_MAXHOST> hostnameBuffer;
			std::array<char, NI_MAXSERV> serviceBuffer;

			int result = getnameinfo(socketAddress, socketLen, hostnameBuffer.data(), hostnameBuffer.size(), serviceBuffer.data(), serviceBuffer.size(), flags);
			if (result == 0)
			{
				if (hostname)
					hostname->Set(hostnameBuffer.data());

				if (service)
					service->Set(serviceBuffer.data());
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
			union byteToInt
			{
				UInt8 b[sizeof(uint32_t)];
				uint32_t i;
			};

			IpAddress::IPv6 ipv6Addr;

			for (auto i = 0; i < 4; ++i)
			{
				byteToInt hostOrder;
				hostOrder.i = 0;
				std::copy(addr.s6_addr + 4 * i, addr.s6_addr + 4 * (i + 1), hostOrder.b);
				ipv6Addr[2 * i] = (hostOrder.b[3] << 8) + hostOrder.b[2];
				ipv6Addr[2 * i + 1] = (hostOrder.b[1] << 8) + hostOrder.b[0];
			}

			return ipv6Addr;
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

	bool IpAddressImpl::ResolveAddress(const IpAddress& ipAddress, String* hostname, String* service, ResolveError* error)
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
			*error = ResolveError_NoError;

		return true;
	}

	std::vector<HostnameInfo> IpAddressImpl::ResolveHostname(NetProtocol procol, const String& hostname, const String& service, ResolveError* error)
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
			result.canonicalName = String::Unicode(p->ai_canonname);
			result.protocol = TranslatePFToNetProtocol(p->ai_family);
			result.socketType = TranslateSockToNetProtocol(p->ai_socktype);

			results.push_back(result);
		}

		if (error)
			*error = ResolveError_NoError;

		return results;
	}

	socklen_t IpAddressImpl::ToSockAddr(const IpAddress& ipAddress, void* buffer)
	{
		if (ipAddress.IsValid())
		{
			switch (ipAddress.GetProtocol())
			{
				case NetProtocol_IPv4:
				{
					sockaddr_in* socketAddress = reinterpret_cast<sockaddr_in*>(buffer);

					std::memset(socketAddress, 0, sizeof(sockaddr_in));
					socketAddress->sin_family = AF_INET;
					socketAddress->sin_port = htons(ipAddress.GetPort());
					socketAddress->sin_addr.s_addr = htonl(ipAddress.ToUInt32());

					return sizeof(sockaddr_in);
				}

				case NetProtocol_IPv6:
				{
					sockaddr_in6* socketAddress = reinterpret_cast<sockaddr_in6*>(buffer);

					std::memset(socketAddress, 0, sizeof(sockaddr_in6));
					socketAddress->sin6_family = AF_INET6;
					socketAddress->sin6_port = htons(ipAddress.GetPort());

					IpAddress::IPv6 address = ipAddress.ToIPv6();
					for (unsigned int i = 0; i < 8; ++i)
					{
						UInt16 networkOrder = htons(address[i]);
						socketAddress->sin6_addr.s6_addr[2 * i] = networkOrder / 256;
						socketAddress->sin6_addr.s6_addr[2 * i + 1] = networkOrder % 256;
					}

					return sizeof(sockaddr_in6);
				}

				default:
					NazaraInternalError("Unhandled ip protocol (0x" + String::Number(ipAddress.GetProtocol()) + ')');
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
				return NetProtocol_IPv4;

			case PF_INET6:
				return NetProtocol_IPv6;

			default:
				return NetProtocol_Unknown;
		}
	}

	SocketType IpAddressImpl::TranslateSockToNetProtocol(int socketType)
	{
		switch (socketType)
		{
			case SOCK_STREAM:
				return SocketType_TCP;

			case SOCK_DGRAM:
				return SocketType_UDP;

			case SOCK_RAW:
				return SocketType_Raw;

			default:
				return SocketType_Unknown;
		}
	}

	ResolveError IpAddressImpl::TranslateEAIErrorToResolveError(int error)
	{
		// http://man7.org/linux/man-pages/man3/gai_strerror.3.html
		switch (error)
		{
			case 0:
				return ResolveError_NoError;

			// Engine error
			case EAI_BADFLAGS:
			case EAI_SYSTEM:
				return ResolveError_Internal;

			case EAI_FAMILY:
			case EAI_SERVICE:
			case EAI_SOCKTYPE:
				return ResolveError_ProtocolNotSupported;

			case EAI_NONAME:
				return ResolveError_NotFound;

			case EAI_FAIL:
				return ResolveError_NonRecoverable;

			case EAI_NODATA:
				return ResolveError_NotInitialized;

			case EAI_MEMORY:
				return ResolveError_ResourceError;

			case EAI_AGAIN:
				return ResolveError_TemporaryFailure;
		}

		NazaraWarning("Unhandled EAI error: " + Error::GetLastSystemError(error) + " (" + String::Number(error) + ") as " + gai_strerror(error));
		return ResolveError_Unknown;
	}
}
