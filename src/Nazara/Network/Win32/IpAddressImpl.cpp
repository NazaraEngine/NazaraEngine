// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Win32/IpAddressImpl.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Network/Win32/SocketImpl.hpp>
#include <cstring>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		#if NAZARA_CORE_WINDOWS_VISTA
		using addrinfoImpl = addrinfoW;

		int GetAddressInfo(const String& hostname, const String& service, const addrinfoImpl* hints, addrinfoImpl** results)
		{
			return GetAddrInfoW(hostname.GetWideString().c_str(), service.GetWideString().c_str(), &hints, &servinfo);
		}

		int GetHostnameInfo(sockaddr* socketAddress, socklen_t socketLen, String* hostname, String* service, INT flags)
		{
			std::array<wchar_t, NI_MAXHOST> hostnameBuffer;
			std::array<wchar_t, NI_MAXSERV> serviceBuffer;

			int result = GetNameInfoW(socketAddress, socketLen, hostnameBuffer.data(), hostnameBuffer.size(), serviceBuffer.data(), serviceBuffer.size(), flags);
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
			FreeAddrInfoW(results);
		}
		#else
		using addrinfoImpl = addrinfo;

		int GetAddressInfo(const String& hostname, const String& service, const addrinfoImpl* hints, addrinfoImpl** results)
		{
			return getaddrinfo(hostname.GetConstBuffer(), service.GetConstBuffer(), hints, results);
		}

		int GetHostnameInfo(sockaddr* socketAddress, socklen_t socketLen, String* hostname, String* service, INT flags)
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
		#endif
	}

	IpAddress IpAddressImpl::FromAddrinfo(const addrinfo* info)
	{
		switch (info->ai_family)
		{
			case AF_INET:
			{
				sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(info->ai_addr);

				auto& rawIpV4 = ipv4->sin_addr.S_un.S_un_b;
				return IpAddress(rawIpV4.s_b1, rawIpV4.s_b2, rawIpV4.s_b3, rawIpV4.s_b4, ntohs(ipv4->sin_port));
			}

			case AF_INET6:
			{
				sockaddr_in6* ipv6 = reinterpret_cast<sockaddr_in6*>(info->ai_addr);

				auto& rawIpV6 = ipv6->sin6_addr.u.Word;
				return IpAddress(rawIpV6[0], rawIpV6[1], rawIpV6[2], rawIpV6[3], rawIpV6[4], rawIpV6[5], rawIpV6[6], rawIpV6[7], ntohs(ipv6->sin6_port));
			}
		}

		return IpAddress::Invalid;
	}

	#if NAZARA_CORE_WINDOWS_VISTA
	IpAddress IpAddressImpl::FromAddrinfo(const addrinfoW* info)
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
	#endif

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
		auto& rawIpV4 = addressv4->sin_addr.S_un.S_un_b;
		return IpAddress(rawIpV4.s_b1, rawIpV4.s_b2, rawIpV4.s_b3, rawIpV4.s_b4, ntohs(addressv4->sin_port));
	}

	IpAddress IpAddressImpl::FromSockAddr(const sockaddr_in6* addressv6)
	{
		auto& rawIpV6 = addressv6->sin6_addr.u.Word;
		return IpAddress(rawIpV6[0], rawIpV6[1], rawIpV6[2], rawIpV6[3], rawIpV6[4], rawIpV6[5], rawIpV6[6], rawIpV6[7], ntohs(addressv6->sin6_port));
	}

	bool IpAddressImpl::ResolveAddress(const IpAddress& ipAddress, String* hostname, String* service, ResolveError* error)
	{
		SockAddrBuffer socketAddress;
		socklen_t socketAddressLen = ToSockAddr(ipAddress, socketAddress.data());

		if (Detail::GetHostnameInfo(reinterpret_cast<sockaddr*>(socketAddress.data()), socketAddressLen, hostname, service, NI_NUMERICSERV) != 0)
		{
			if (error)
				*error = TranslateWSAErrorToResolveError(WSAGetLastError());

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
				*error = TranslateWSAErrorToResolveError(WSAGetLastError());

			return results;
		}

		CallOnExit onExit([servinfo]()
		{
			Detail::FreeAddressInfo(servinfo);
		});

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
					socketAddress->sin_addr.S_un.S_addr = htonl(ipAddress.ToUInt32());

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
						socketAddress->sin6_addr.u.Word[i] = htons(address[i]);

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

	ResolveError IpAddressImpl::TranslateWSAErrorToResolveError(int error)
	{
		switch (error)
		{
			case 0:
				return ResolveError_NoError;

			// Engine error
			case WSAEFAULT:
			case WSAEINVAL:
				return ResolveError_Internal;

			case WSAEAFNOSUPPORT:
			case WSAESOCKTNOSUPPORT:
			case WSASERVICE_NOT_FOUND:
				return ResolveError_ProtocolNotSupported;

			case WSAHOST_NOT_FOUND:
				return ResolveError_NotFound;

			case WSANO_RECOVERY:
				return ResolveError_NonRecoverable;

			case WSANOTINITIALISED:
				return ResolveError_NotInitialized;

			case WSA_NOT_ENOUGH_MEMORY:
				return ResolveError_ResourceError;

			case WSATRY_AGAIN:
				return ResolveError_TemporaryFailure;
		}

		NazaraWarning("Unhandled WinSock error: " + Error::GetLastSystemError(error) + " (" + String::Number(error) + ')');
		return ResolveError_Unknown;
	}
}
