// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <limits>
#include <ostream>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a IpAddress object by default
	*/

	inline IpAddress::IpAddress() :
	m_isValid(false)
	{
	}

	/*!
	* \brief Constructs a IpAddress object with an IP and a port
	*
	* \param ip IPv4 address
	* \param port Port of the IP
	*/

	inline IpAddress::IpAddress(const IPv4& ip, UInt16 port) :
	m_ipv4(ip),
	m_protocol(NetProtocol_IPv4),
	m_port(port),
	m_isValid(true)
	{
	}

	/*!
	* \brief Constructs a IpAddress object with an IP and a port
	*
	* \param ip IPv6 address
	* \param port Port of the IP
	*/

	inline IpAddress::IpAddress(const IPv6& ip, UInt16 port) :
	m_ipv6(ip),
	m_protocol(NetProtocol_IPv6),
	m_port(port),
	m_isValid(true)
	{
	}

	/*!
	* \brief Constructs a IpAddress object with an IP and a port
	*
	* \param ip IPv4 address (a.b.c.d)
	* \param port Port of the IP
	*/

	inline IpAddress::IpAddress(const UInt8& a, const UInt8& b, const UInt8& c, const UInt8& d, UInt16 port) :
	IpAddress(IPv4{a, b, c, d}, port)
	{
	}

	/*!
	* \brief Constructs a IpAddress object with an IP and a port
	*
	* \param ip IPv6 address (a.b.c.d.e.f.g.h)
	* \param port Port of the IP
	*/

	inline IpAddress::IpAddress(const UInt16& a, const UInt16& b, const UInt16& c, const UInt16& d, const UInt16& e, const UInt16& f, const UInt16& g, const UInt16& h, UInt16 port) :
	IpAddress(IPv6{a, b, c, d, e, f, g, h}, port)
	{
	}

	/*!
	* Constructs a IpAddress object with a C-string
	*
	* \param address Hostname or textual IP address
	*/

	inline IpAddress::IpAddress(const char* address)
	{
		BuildFromAddress(address);
	}

	/*!
	* Constructs a IpAddress object with a string
	*
	* \param address Hostname or textual IP address
	*/

	inline IpAddress::IpAddress(const String& address)
	{
		BuildFromAddress(address.GetConstBuffer());
	}

	/*!
	* \brief Gets the port
	* \return Port attached to the IP address
	*/

	inline UInt16 IpAddress::GetPort() const
	{
		return m_port;
	}

	/*!
	* \brief Gets the net protocol
	* \return Protocol attached to the IP address
	*/

	inline NetProtocol IpAddress::GetProtocol() const
	{
		return m_protocol;
	}

	/*!
	* \brief Checks whether the IP address is valid
	* \return true If successful
	*/

	inline bool IpAddress::IsValid() const
	{
		return m_isValid;
	}

	/*!
	* \brief Sets the port
	*
	* \param port Port attached to the IP address
	*/

	inline void IpAddress::SetPort(UInt16 port)
	{
		m_port = port;
	}

	/*!
	* \brief Converts IpAddress to IPv4
	* \return Corresponding IPv4
	*
	* \remark Produces a NazaraAssert if net protocol is not IPv4
	*/

	inline IpAddress::IPv4 IpAddress::ToIPv4() const
	{
		NazaraAssert(m_isValid && m_protocol == NetProtocol_IPv4, "Address is not a valid IPv4");

		return m_ipv4;
	}

	/*!
	* \brief Converts IpAddress to IPv6
	* \return Corresponding IPv6
	*
	* \remark Produces a NazaraAssert if net protocol is not IPv6
	*/

	inline IpAddress::IPv6 IpAddress::ToIPv6() const
	{
		NazaraAssert(m_isValid && m_protocol == NetProtocol_IPv6, "IP is not a valid IPv6");

		return m_ipv6;
	}

	/*!
	* \brief Converts IpAddress to UInt32
	* \return Corresponding UInt32
	*
	* \remark Produces a NazaraAssert if net protocol is not IPv4
	*/

	inline UInt32 IpAddress::ToUInt32() const
	{
		NazaraAssert(m_isValid && m_protocol == NetProtocol_IPv4, "Address is not a valid IPv4");

		return UInt32(m_ipv4[0]) << 24 |
		       UInt32(m_ipv4[1]) << 16 |
		       UInt32(m_ipv4[2]) << 8  |
		       UInt32(m_ipv4[3]) << 0;
	}

	/*!
	* \brief Converts IpAddress to boolean
	* \return true If IpAddress is valid
	*
	* \see IsValid
	*/

	inline IpAddress::operator bool() const
	{
		return IsValid();
	}

	/*!
	* \brief Output operator
	* \return The stream
	*
	* \param out The stream
	* \param address The address to output
	*/

	inline std::ostream& operator<<(std::ostream& out, const IpAddress& address)
	{
		out << "IpAddress(" << address.ToString() << ')';
		return out;
	}

	/*!
	* \brief Compares the IpAddress to other one
	* \return true if the ip addresses are the same
	*
	* \param first First ip address to compare
	* \param second Second ip address to compare with
	*/

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

	/*!
	* \brief Compares the IpAddress to other one
	* \return false if the ip addresses are the same
	*
	* \param first First ip address to compare
	* \param second Second ip address to compare with
	*/

	inline bool operator!=(const IpAddress& first, const IpAddress& second)
	{
		return !operator==(first, second);
	}

	/*!
	* \brief Compares the IpAddress to other one
	* \return true if this ip address is inferior to the other one
	*
	* \param first First ip address to compare
	* \param second Second ip address to compare with
	*/

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

	/*!
	* \brief Compares the IpAddress to other one
	* \return true if this ip address is inferior or equal to the other one
	*
	* \param first First ip address to compare
	* \param second Second ip address to compare with
	*/

	inline bool operator<=(const IpAddress& first, const IpAddress& second)
	{
		return !operator<(second, first);
	}

	/*!
	* \brief Compares the IpAddress to other one
	* \return true if this ip address is greather to the other one
	*
	* \param first First ip address to compare
	* \param second Second ip address to compare with
	*/

	inline bool operator>(const IpAddress& first, const IpAddress& second)
	{
		return second < first;
	}

	/*!
	* \brief Compares the IpAddress to other one
	* \return true if this ip address is greather or equal to the other one
	*
	* \param first First ip address to compare
	* \param second Second ip address to compare with
	*/

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
		/*!
		* \brief Converts IpAddress to hash
		* \return Hash of the IpAddress
		*
		* \param ip IpAddress to hash
		*/

		size_t operator()(const Nz::IpAddress& ip) const
		{
			if (!ip)
				return std::numeric_limits<size_t>::max(); //< Returns a fixed value for invalid addresses

			// This is SDBM adapted for IP addresses, tested to generate the least collisions possible
			// (It doesn't mean it cannot be improved though)
			std::size_t hash = 0;
			switch (ip.GetProtocol())
			{
				case Nz::NetProtocol_Any:
				case Nz::NetProtocol_Unknown:
					return std::numeric_limits<size_t>::max();

				case Nz::NetProtocol_IPv4:
				{
					hash = ip.ToUInt32() + (hash << 6) + (hash << 16) - hash;
					break;
				}
				case Nz::NetProtocol_IPv6:
				{
					Nz::IpAddress::IPv6 v6 = ip.ToIPv6();
					for (std::size_t i = 0; i < v6.size(); i++)
						hash = v6[i] + (hash << 6) + (hash << 16) - hash;

					break;
				}
			}

			return ip.GetPort() + (hash << 6) + (hash << 16) - hash;
		}
	};
}
#include <Nazara/Network/DebugOff.hpp>
