// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Network/IpAddress.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Network/Algorithm.hpp>
#include <algorithm>
#include <limits>
#include <sstream>

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Network/Win32/IpAddressImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <Nazara/Network/Posix/IpAddressImpl.hpp>
#else
#error Missing implementation: Network
#endif


namespace Nz
{
	/*!
	* \ingroup network
	* \class Nz::IpAddress
	* \brief Network class that represents an IP address
	*/

	/*!
	* \brief Builds the IP from a hostname
	* \return true If successful
	*
	* \remark address C-string symbolizing the IP address
	*/
	bool IpAddress::BuildFromAddress(const char* address)
	{
		m_isValid = false;

		bool isIPv6;
		UInt8 result[16];
		const char* endOfRead;
		if (!ParseIPAddress(address, result, &m_port, &isIPv6, &endOfRead))
			return false;

		// was everything parsed?
		if (*endOfRead != '\0')
			return false;

		m_isValid = true;
		if (isIPv6)
		{
			m_protocol = NetProtocol::IPv6;

			for (unsigned int i = 0; i < 8; ++i)
				m_ipv6[i] = UInt32(result[i*2]) << 8 | result[i*2 + 1];
		}
		else
		{
			m_protocol = NetProtocol::IPv4;

			for (unsigned int i = 0; i < 4; ++i)
				m_ipv4[i] = result[i];
		}

		return true;
	}

	/*!
	* \brief Checks whether the IP address is loopback
	* \return true If it is the case
	*
	* \remark Produces a NazaraAssert if internal protocol is invalid (should never happen)
	*/

	bool IpAddress::IsLoopback() const
	{
		if (!m_isValid)
			return false;

		NazaraAssert(m_protocol <= NetProtocol::Max, "Protocol has value out of enum");
		switch (m_protocol)
		{
			case NetProtocol::Any:
			case NetProtocol::Unknown:
				break;

			case NetProtocol::IPv4:
				return m_ipv4[0] == 127;

			case NetProtocol::IPv6:
				return m_ipv6 == LoopbackIpV6.m_ipv6; // Only compare the ip value
		}

		NazaraInternalErrorFmt("invalid protocol for IpAddress ({0:#x})", UnderlyingCast(m_protocol));
		return false;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object
	*
	* \remark Produces a NazaraAssert if internal protocol is invalid (should never happen)
	*/
	std::string IpAddress::ToString(bool includesPort) const
	{
		std::ostringstream stream;

		if (m_isValid)
		{
			NazaraAssert(m_protocol <= NetProtocol::Max, "Protocol has value out of enum");
			switch (m_protocol)
			{
				case NetProtocol::Any:
				case NetProtocol::Unknown:
					break;

				case NetProtocol::IPv4:
					for (unsigned int i = 0; i < 4; ++i)
					{
						stream << int(m_ipv4[i]);
						if (i != 3)
							stream << '.';
					}
					break;

				case NetProtocol::IPv6:
					// Canonical representation of an IPv6
					// https://tools.ietf.org/html/rfc5952

					// Find the longest zero sequence
					unsigned int f0 = std::numeric_limits<unsigned int>::max();
					unsigned int l0 = std::numeric_limits<unsigned int>::max();

					for (unsigned int i = 0; i < 8; ++i)
					{
						if (m_ipv6[i] == 0)
						{
							unsigned int j;
							for (j = i + 1; j < 8; ++j)
							{
								if (m_ipv6[j] != 0)
									break;
							}

							if (j - i > std::max<unsigned int>(l0 - f0, 1))
							{
								f0 = i;
								l0 = j;
							}
						}
					}

					// We need brackets around our IPv6 address if we have a port
					if (m_port != 0 && includesPort)
						stream << '[';

					// IPv4-mapped IPv6?
					if (f0 == 0 && l0 == 5 && m_ipv6[5] == 0xFFFF)
					{
						IPv4 ipv4 = {
							SafeCast<UInt8>(m_ipv6[6] >> 8),
							SafeCast<UInt8>(m_ipv6[6] & 0xFF),
							SafeCast<UInt8>(m_ipv6[7] >> 8),
							SafeCast<UInt8>(m_ipv6[7] & 0xFF),
						};

						stream << "::ffff:";
						for (unsigned int i = 0; i < 4; ++i)
						{
							stream << int(ipv4[i]);
							if (i != 3)
								stream << '.';
						}
					}
					else
					{
						for (unsigned int i = 0; i < 8; ++i)
						{
							if (i == f0)
							{
								stream << "::";
								i = l0;
								if (i >= 8)
									break;
							}
							else if (i != 0)
								stream << ':';

							stream << ToLower(NumberToString(m_ipv6[i], 16));
						}
					}


					if (m_port != 0 && includesPort)
						stream << ']';
					break;
			}

			if (m_port != 0 && includesPort)
				stream << ':' << m_port;
		}

		return stream.str();
	}

	/*!
	* \brief Resolves the address based on the IP
	* \return Hostname of the address
	*
	* \param address IP address to resolve
	* \param service Optional argument to specify the protocol used
	* \param error Optional argument to get the error
	*
	* \remark Produces a NazaraAssert if address is invalid
	*/
	std::string IpAddress::ResolveAddress(const IpAddress& address, std::string* service, ResolveError* error)
	{
		NazaraAssert(address.IsValid(), "Invalid address");

		std::string hostname;
		IpAddressImpl::ResolveAddress(address, &hostname, service, error);

		return hostname;
	}

	/*!
	* \brief Resolves the address based on the hostname
	* \return Informations about the host: IP(s) of the address, names, ...
	*
	* \param protocol Net protocol to use
	* \param hostname Hostname to resolve
	* \param service Specify the service used (http, ...)
	* \param error Optional argument to get the error
	*
	* \remark Produces a NazaraAssert if net protocol is set to unknown
	*/
	std::vector<HostnameInfo> IpAddress::ResolveHostname(NetProtocol protocol, const std::string& hostname, const std::string& service, ResolveError* error)
	{
		NazaraAssert(protocol != NetProtocol::Unknown, "Invalid protocol");

		return IpAddressImpl::ResolveHostname(protocol, hostname, service, error);
	}

	IpAddress IpAddress::AnyIpV4(0, 0, 0, 0);
	IpAddress IpAddress::AnyIpV6(0, 0, 0, 0, 0, 0, 0, 0, 0);
	IpAddress IpAddress::BroadcastIpV4(255, 255, 255, 255);
	IpAddress IpAddress::Invalid;
	IpAddress IpAddress::LoopbackIpV4(127, 0, 0, 1);
	IpAddress IpAddress::LoopbackIpV6(0, 0, 0, 0, 0, 0, 0, 1);
}
