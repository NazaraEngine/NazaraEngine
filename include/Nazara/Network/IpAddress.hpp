// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_IPADDRESS_HPP
#define NAZARA_IPADDRESS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Network/Config.hpp>
#include <Nazara/Network/Enums.hpp>
#include <array>
#include <iosfwd>

namespace Nz
{
	struct HostnameInfo;

	class NAZARA_NETWORK_API IpAddress
	{
		public:
			using IPv4 = std::array<UInt8, 4>;  //< four 8bits blocks
			using IPv6 = std::array<UInt16, 8>; //< eight 16bits blocks

			inline IpAddress();
			inline IpAddress(const IPv4& ip, UInt16 port = 0);
			inline IpAddress(const IPv6& ip, UInt16 port = 0);
			inline IpAddress(const UInt8& a, const UInt8& b, const UInt8& c, const UInt8& d, UInt16 port = 0);
			inline IpAddress(const UInt16& a, const UInt16& b, const UInt16& c, const UInt16& d, const UInt16& e, const UInt16& f, const UInt16& g, const UInt16& h, UInt16 port = 0);
			inline explicit IpAddress(const char* address);
			inline explicit IpAddress(const String& address);
			IpAddress(const IpAddress&) = default;
			IpAddress(IpAddress&&) noexcept = default;
			~IpAddress() = default;

			bool BuildFromAddress(const char* address);

			inline UInt16 GetPort() const;
			inline NetProtocol GetProtocol() const;

			bool IsLoopback() const;
			inline bool IsValid() const;

			inline void SetPort(UInt16 port);

			inline IPv4 ToIPv4() const;
			inline IPv6 ToIPv6() const;
			String ToString() const;
			inline UInt32 ToUInt32() const;

			inline explicit operator bool() const;

			IpAddress& operator=(const IpAddress&) = default;
			IpAddress& operator=(IpAddress&&) noexcept = default;

			static String ResolveAddress(const IpAddress& address, String* service = nullptr, ResolveError* error = nullptr);
			static std::vector<HostnameInfo> ResolveHostname(NetProtocol procol, const String& hostname, const String& protocol = "http", ResolveError* error = nullptr);

			inline friend std::ostream& operator<<(std::ostream& out, const IpAddress& address);

			inline friend bool operator==(const IpAddress& first, const IpAddress& second);
			inline friend bool operator!=(const IpAddress& first, const IpAddress& second);
			inline friend bool operator<(const IpAddress& first, const IpAddress& second);
			inline friend bool operator<=(const IpAddress& first, const IpAddress& second);
			inline friend bool operator>(const IpAddress& first, const IpAddress& second);
			inline friend bool operator>=(const IpAddress& first, const IpAddress& second);

			static IpAddress AnyIpV4;
			static IpAddress AnyIpV6;
			static IpAddress BroadcastIpV4;
			static IpAddress Invalid;
			static IpAddress LoopbackIpV4;
			static IpAddress LoopbackIpV6;

		private:
			union
			{
				IPv4 m_ipv4;
				IPv6 m_ipv6;
			};

			NetProtocol m_protocol;
			UInt16 m_port;
			bool m_isValid;
	};

	struct HostnameInfo
	{
		IpAddress address;
		NetProtocol protocol;
		SocketType socketType;
		String canonicalName;
	};

}

#include <Nazara/Network/IpAddress.inl>

#endif // NAZARA_IPADDRESS_HPP
