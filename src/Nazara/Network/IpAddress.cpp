// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/IpAddress.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Network/Algorithm.hpp>
#include <algorithm>
#include <limits>

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Network/Win32/IpAddressImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <Nazara/Network/Posix/IpAddressImpl.hpp>
#else
#error Missing implementation: Network
#endif

#include <Nazara/Network/Debug.hpp>

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
	* \remark address C-string symbolizing the IP address or hostname
	*/

	bool IpAddress::BuildFromAddress(const char* address)
	{
		m_isValid = false;

		bool isIPv6;
		UInt8 result[16];
		if (!ParseIPAddress(address, result, &m_port, &isIPv6, nullptr))
			return false;

		m_isValid = true;
		if (isIPv6)
		{
			m_protocol = NetProtocol_IPv6;

			for (unsigned int i = 0; i < 8; ++i)
				m_ipv6[i] = UInt32(result[i*2]) << 8 | result[i*2 + 1];
		}
		else
		{
			m_protocol = NetProtocol_IPv4;

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

		NazaraAssert(m_protocol <= NetProtocol_Max, "Protocol has value out of enum");
		switch (m_protocol)
		{
			case NetProtocol_Any:
			case NetProtocol_Unknown:
				break;

			case NetProtocol_IPv4:
				return m_ipv4[0] == 127;

			case NetProtocol_IPv6:
				return m_ipv6 == LoopbackIpV6.m_ipv6; // Only compare the ip value
		}

		NazaraInternalError("Invalid protocol for IpAddress (0x" + String::Number(m_protocol) + ')');
		return false;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object
	*
	* \remark Produces a NazaraAssert if internal protocol is invalid (should never happen)
	*/

	String IpAddress::ToString() const
	{
		StringStream stream;

		if (m_isValid)
		{
			NazaraAssert(m_protocol <= NetProtocol_Max, "Protocol has value out of enum");
			switch (m_protocol)
			{
				case NetProtocol_Any:
				case NetProtocol_Unknown:
					break;

				case NetProtocol_IPv4:
					for (unsigned int i = 0; i < 4; ++i)
					{
						stream << int(m_ipv4[i]);
						if (i != 3)
							stream << '.';
					}
					break;

				case NetProtocol_IPv6:
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
					if (m_port != 0)
						stream << '[';

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

						stream << String::Number(m_ipv6[i], 16).ToLower();
					}

					if (m_port != 0)
						stream << ']';
					break;
			}

			if (m_port != 0)
				stream << ':' << m_port;
		}

		return stream;
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

	String IpAddress::ResolveAddress(const IpAddress& address, String* service, ResolveError* error)
	{
		NazaraAssert(address.IsValid(), "Invalid address");

		String hostname;
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

	std::vector<HostnameInfo> IpAddress::ResolveHostname(NetProtocol protocol, const String& hostname, const String& service, ResolveError* error)
	{
		NazaraAssert(protocol != NetProtocol_Unknown, "Invalid protocol");

		return IpAddressImpl::ResolveHostname(protocol, hostname, service, error);
	}

	IpAddress IpAddress::AnyIpV4(0, 0, 0, 0);
	IpAddress IpAddress::AnyIpV6(0, 0, 0, 0, 0, 0, 0, 0, 0);
	IpAddress IpAddress::BroadcastIpV4(255, 255, 255, 255);
	IpAddress IpAddress::Invalid;
	IpAddress IpAddress::LoopbackIpV4(127, 0, 0, 1);
	IpAddress IpAddress::LoopbackIpV6(0, 0, 0, 0, 0, 0, 0, 1);
}
