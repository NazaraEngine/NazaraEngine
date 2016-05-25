// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <limits>
#include <ostream>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline IpAddress::IpAddress() :
	m_isValid(false)
	{
	}

	inline IpAddress::IpAddress(const IPv4& ip, UInt16 port) :
	m_ipv4(ip),
	m_protocol(NetProtocol_IPv4),
	m_port(port),
	m_isValid(true)
	{
	}

	inline IpAddress::IpAddress(const IPv6& ip, UInt16 port) :
	m_ipv6(ip),
	m_protocol(NetProtocol_IPv6),
	m_port(port),
	m_isValid(true)
	{
	}

	inline IpAddress::IpAddress(const UInt8& a, const UInt8& b, const UInt8& c, const UInt8& d, UInt16 port) :
	IpAddress(IPv4{a, b, c, d}, port)
	{
	}

	inline IpAddress::IpAddress(const UInt16& a, const UInt16& b, const UInt16& c, const UInt16& d, const UInt16& e, const UInt16& f, const UInt16& g, const UInt16& h, UInt16 port) :
	IpAddress(IPv6{a, b, c, d, e, f, g, h}, port)
	{
	}

	inline IpAddress::IpAddress(const char* address)
	{
		BuildFromAddress(address);
	}

	inline IpAddress::IpAddress(const String& address)
	{
		BuildFromAddress(address.GetConstBuffer());
	}

	inline UInt16 IpAddress::GetPort() const
	{
		return m_port;
	}

	inline NetProtocol IpAddress::GetProtocol() const
	{
		return m_protocol;
	}

	inline bool IpAddress::IsValid() const
	{
		return m_isValid;
	}

	inline void IpAddress::SetPort(UInt16 port)
	{
		m_port = port;
	}

	inline IpAddress::IPv4 IpAddress::ToIPv4() const
	{
		NazaraAssert(m_isValid && m_protocol == NetProtocol_IPv4, "Address is not a valid IPv4");

		return m_ipv4;
	}

	inline IpAddress::IPv6 IpAddress::ToIPv6() const
	{
		NazaraAssert(m_isValid && m_protocol == NetProtocol_IPv6, "IP is not a valid IPv6");

		return m_ipv6;
	}

	inline UInt32 IpAddress::ToUInt32() const
	{
		NazaraAssert(m_isValid && m_protocol == NetProtocol_IPv4, "Address is not a valid IPv4");

		return UInt32(m_ipv4[0]) << 24 |
		       UInt32(m_ipv4[1]) << 16 |
		       UInt32(m_ipv4[2]) << 8  |
		       UInt32(m_ipv4[3]) << 0;
	}

	inline IpAddress::operator bool() const
	{
		return IsValid();
	}

	inline std::ostream& operator<<(std::ostream& out, const IpAddress& address)
	{
		out << "IpAddress(" << address.ToString() << ')';
		return out;
	}

	inline bool operator==(const IpAddress& first, const IpAddress& second)
	{
		// We need to check the validity of each address before comparing them
		if (!first.m_isValid || !second.m_isValid)
			return first.m_isValid == second.m_isValid;

		// Then the protocol
		if (first.m_protocol != second.m_protocol)
			return false;

		// Each protocol has its variables to compare
		switch (first.m_protocol)
		{
			case NetProtocol_Any:
			case NetProtocol_Unknown:
				break;

			case NetProtocol_IPv4:
			{
				if (first.m_ipv4 != second.m_ipv4)
					return false;

				break;
			}

			case NetProtocol_IPv6:
			{
				if (first.m_ipv6 != second.m_ipv6)
					return false;

				break;
			}
		}

		// Check the port, in case there is one
		if (first.m_port != second.m_port)
			return false;

		return true;
	}

	inline bool operator!=(const IpAddress& first, const IpAddress& second)
	{
		return !operator==(first, second);
	}

	inline bool operator<(const IpAddress& first, const IpAddress& second)
	{
		// If the second address is invalid, there's no way we're lower than it
		if (!second.m_isValid)
			return false;

		// By this point, the second address is valid, thus check our validity
		if (!first.m_isValid)
			return true; // Invalid address are lower than valid one

		// Compare protocols
		if (first.m_protocol != second.m_protocol)
			return first.m_protocol < second.m_protocol;

		// Compare IP (thanks to std::array comparison operator)
		switch (first.m_protocol)
		{
			case NetProtocol_Any:
			case NetProtocol_Unknown:
				break;

			case NetProtocol_IPv4:
			{
				if (first.m_ipv4 != second.m_ipv4)
					return first.m_ipv4 < second.m_ipv4;

				break;
			}

			case NetProtocol_IPv6:
			{
				if (first.m_ipv6 != second.m_ipv6)
					return first.m_ipv6 < second.m_ipv6;

				break;
			}
		}

		// Compare port
		if (first.m_port != second.m_port)
			return first.m_port < second.m_port;

		return false; //< Same address
	}

	inline bool operator<=(const IpAddress& first, const IpAddress& second)
	{
		return !operator<(second, first);
	}

	inline bool operator>(const IpAddress& first, const IpAddress& second)
	{
		return second < first;
	}

	inline bool operator>=(const IpAddress& first, const IpAddress& second)
	{
		return !operator<(first, second);
	}
}

namespace std
{
	template<>
	struct hash<Nz::IpAddress>
	{
		size_t operator()(const Nz::IpAddress& ip) const
		{
			if (!ip)
				return std::numeric_limits<size_t>::max(); //< Returns a fixed value for invalid addresses

			// This is SDBM adapted for IP addresses, tested to generate the least collisions possible
			// (It doesn't mean it cannot be improved though)
			std::size_t h = 0;
			switch (ip.GetProtocol())
			{
				case Nz::NetProtocol_Any:
				case Nz::NetProtocol_Unknown:
					return std::numeric_limits<size_t>::max();

				case Nz::NetProtocol_IPv4:
				{
					h = ip.ToUInt32() + (h << 6) + (h << 16) - h;
					break;
				}
				case Nz::NetProtocol_IPv6:
				{
					Nz::IpAddress::IPv6 v6 = ip.ToIPv6();
					for (std::size_t i = 0; i < v6.size(); i++)
						h = v6[i] + (h << 6) + (h << 16) - h;

					break;
				}
			}

			return ip.GetPort() + (h << 6) + (h << 16) - h;
		}
	};
}
#include <Nazara/Network/DebugOff.hpp>
