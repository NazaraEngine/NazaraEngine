// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Win32/IpAddressImpl.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Network/Win32/SocketImpl.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <cstring>
#include <Nazara/Network/Debug.hpp>

// some MinGW distributions seem to lack some defines
#ifndef ERROR_NOT_ENOUGH_MEMORY
#define ERROR_NOT_ENOUGH_MEMORY 8L
#endif

#ifndef WSA_NOT_ENOUGH_MEMORY
#define WSA_NOT_ENOUGH_MEMORY (ERROR_NOT_ENOUGH_MEMORY)
#endif

namespace Nz
{
	namespace Detail
	{
#if NAZARAUTILS_WINDOWS_NT6
		using addrinfoImpl = addrinfoW;

		int GetAddressInfo(const std::string& hostname, const std::string& service, const addrinfoImpl* hints, addrinfoImpl** results)
		{
			return GetAddrInfoW(ToWideString(hostname).c_str(), ToWideString(service).c_str(), hints, results);
		}

		int GetHostnameInfo(sockaddr* socketAddress, socklen_t socketLen, std::string* hostname, std::string* service, INT flags)
		{
			std::array<wchar_t, NI_MAXHOST> hostnameBuffer;
			std::array<wchar_t, NI_MAXSERV> serviceBuffer;

			int result = GetNameInfoW(socketAddress, socketLen, hostnameBuffer.data(), static_cast<DWORD>(hostnameBuffer.size()), serviceBuffer.data(), static_cast<DWORD>(serviceBuffer.size()), flags);
			if (result == 0)
			{
				if (hostname)
					*hostname = FromWideString(hostnameBuffer.data());

				if (service)
					*service = FromWideString(serviceBuffer.data());
			}

			return result;
		}

		void FreeAddressInfo(addrinfoImpl* results)
		{
			FreeAddrInfoW(results);
		}

		std::string TranslateCanonicalName(const wchar_t* str)
		{
			return FromWideString(str);
		}
#else
		using addrinfoImpl = addrinfo;

		int GetAddressInfo(const std::string& hostname, const std::string& service, const addrinfoImpl* hints, addrinfoImpl** results)
		{
			return getaddrinfo(hostname.c_str(), service.c_str(), hints, results);
		}

		int GetHostnameInfo(sockaddr* socketAddress, socklen_t socketLen, std::string* hostname, std::string* service, INT flags)
		{
			std::array<char, NI_MAXHOST> hostnameBuffer;
			std::array<char, NI_MAXSERV> serviceBuffer;

			int result = getnameinfo(socketAddress, socketLen, hostnameBuffer.data(), static_cast<DWORD>(hostnameBuffer.size()), serviceBuffer.data(), static_cast<DWORD>(serviceBuffer.size()), flags);
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

		std::string TranslateCanonicalName(const char* str)
		{
			return str;
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

#if NAZARAUTILS_WINDOWS_NT6
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

		if (Detail::GetHostnameInfo(reinterpret_cast<sockaddr*>(socketAddress.data()), socketAddressLen, hostname, service, NI_NUMERICSERV) != 0)
		{
			if (error)
				*error = TranslateWSAErrorToResolveError(WSAGetLastError());

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
				*error = TranslateWSAErrorToResolveError(WSAGetLastError());

			return results;
		}

		CallOnExit onExit([servinfo]()
		{
			Detail::FreeAddressInfo(servinfo);
		});

		for (Detail::addrinfoImpl* p = servinfo; p != nullptr; p = p->ai_next)
		{
			HostnameInfo& result = results.emplace_back();
			result.address = FromAddrinfo(p);
			result.protocol = TranslatePFToNetProtocol(p->ai_family);
			result.socketType = TranslateSockToNetProtocol(p->ai_socktype);

			if (p->ai_canonname)
				result.canonicalName = Detail::TranslateCanonicalName(p->ai_canonname);
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
						socketAddress->sin6_addr.s6_addr[i * 2 + 0] = static_cast<UCHAR>(addressPart >> 0);
						socketAddress->sin6_addr.s6_addr[i * 2 + 1] = static_cast<UCHAR>(addressPart >> 8);
					}

					return sizeof(sockaddr_in6);
				}

				default:
					NazaraInternalError("Unhandled ip protocol (0x" + NumberToString(UnderlyingCast(ipAddress.GetProtocol())) + ')');
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

	ResolveError IpAddressImpl::TranslateWSAErrorToResolveError(int error)
	{
		switch (error)
		{
			case 0:
				return ResolveError::NoError;

			// Engine error
			case WSAEFAULT:
			case WSAEINVAL:
				return ResolveError::Internal;

			case WSAEAFNOSUPPORT:
			case WSAESOCKTNOSUPPORT:
			case WSASERVICE_NOT_FOUND:
				return ResolveError::ProtocolNotSupported;

			case WSAHOST_NOT_FOUND:
				return ResolveError::NotFound;

			case WSANO_RECOVERY:
				return ResolveError::NonRecoverable;

			case WSANOTINITIALISED:
				return ResolveError::NotInitialized;

			case WSA_NOT_ENOUGH_MEMORY:
				return ResolveError::ResourceError;

			case WSATRY_AGAIN:
				return ResolveError::TemporaryFailure;
		}

		NazaraWarning("Unhandled WinSock error: " + Error::GetLastSystemError(error) + " (" + NumberToString(error) + ')');
		return ResolveError::Unknown;
	}
}
