// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <cstring>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		/*!
		* \brief Parses a decimal number
		* \return true If successful
		*
		* \param str C-string symbolizing the string to parse
		* \param number Optional argument to return the number parsed
		* \param endOfRead Optional argument to determine where parsing stopped
		*/
		bool ParseDecimal(const char* str, unsigned int* number, const char** endOfRead)
		{
			const char* ptr = str;
			unsigned int val = 0;
			while (*ptr >= '0' && *ptr <= '9')
			{
				val *= 10;
				val += *ptr - '0';

				++ptr;
			}

			if (str == ptr)
				return false;

			if (number)
				*number = val;

			if (endOfRead)
				*endOfRead = ptr;

			return true;
		}

		/*!
		* \brief Parses a hexadecimal number
		* \return true If successful
		*
		* \param str C-string symbolizing the string to parse
		* \param number Optional argument to return the number parsed
		* \param endOfRead Optional argument to determine where parsing stopped
		*/
		bool ParseHexadecimal(const char* str, unsigned int* number, const char** endOfRead)
		{
			const char* ptr = str;
			unsigned int val = 0;
			while ((*ptr >= '0' && *ptr <= '9') || ((*ptr & 0x5F) >= 'A' && (*ptr & 0x5F) <= 'F'))
			{
				val *= 16;
				val += (*ptr > '9') ? ((*ptr & 0x5F) - 'A' + 10) : *ptr - '0';

				++ptr;
			}

			if (str == ptr)
				return false;

			if (number)
				*number = val;

			if (endOfRead)
				*endOfRead = ptr;

			return true;
		}
	}


	/*!
	* \ingroup network
	* \brief Returns the text representation of an error
	* \return Text representation of an error
	*
	* \param resolveError Error enumeration
	*/
	const char* ErrorToString(ResolveError resolveError)
	{
		switch (resolveError)
		{
			case ResolveError_NoError:
				return "No error";

			case ResolveError_Internal:
				return "An internal error occurred";

			case ResolveError_ResourceError:
				return "The operating system lacks the resources to proceed";

			case ResolveError_NonRecoverable:
				return "A nonrecoverable error occurred";

			case ResolveError_NotFound:
				return "No such host is known";

			case ResolveError_NotInitialized:
				return "Nazara Network has not been initialized";

			case ResolveError_ProtocolNotSupported:
				return "A specified protocol is not supported by the server";

			case ResolveError_TemporaryFailure:
				return "A temporary failure occurred, try again";

			case ResolveError_Unknown:
				return "An unknown error occurred";

			default:
				return "Invalid error value";
		}
	}

	/*!
	* \ingroup network
	* \brief Returns the text representation of an error
	* \return Text representation of an error
	*
	* \param socketError Error enumeration
	*/
	const char* ErrorToString(SocketError socketError)
	{
		switch (socketError)
		{
			case SocketError_NoError:
				return "No error";

			case SocketError_AddressNotAvailable:
				return "The address is already in use";

			case SocketError_ConnectionClosed:
				return "The connection has been closed";

			case SocketError_ConnectionRefused:
				return "The connection attempt was refused";

			case SocketError_DatagramSize:
				return "The datagram size is over the system limit";

			case SocketError_Internal:
				return "An internal error occurred";

			case SocketError_Interrupted:
				return "The operation was interrupted by a signal";

			case SocketError_Packet:
				return "Packet encoding or decoding failed";

			case SocketError_NetworkError:
				return "Networking subsystem failed";

			case SocketError_NotInitialized:
				return "Network module has not been initialized";

			case SocketError_NotSupported:
				return "This operation is not supported";

			case SocketError_ResolveError:
				return "The hostname couldn't be resolved";

			case SocketError_ResourceError:
				return "The operating system lacks the resources to proceed";

			case SocketError_TimedOut:
				return "The operation timed out";

			case SocketError_Unknown:
				return "An unknown error occurred";

			case SocketError_UnreachableHost:
				return "The host is not reachable";

			default:
				return "Invalid error value";
		}
	}

	/*!
	* \ingroup network
	* \brief Parse a textual IPv4 or IPv6 address
	* \return true If successful
	*
	* From http://rosettacode.org/wiki/Parse_an_IP_Address
	* Parse a textual IPv4 or IPv6 address, optionally with port, into a binary
	* array (for the address, in host order), and an optionally provided port.
	* Also, indicate which of those forms (4 or 6) was parsed.
	*
	* \param addressPtr C-string which symbolizes the ip adress
	* \param result Byte array to return the result in
	* \param port Optional argument to resolve according to a specific port
	* \param isIPv6 Optional argument to determine if the address is IPv6
	* \param endOfRead Optional argument to determine where parsing stopped
	*
	* \remark Produces a NazaraAssert if addressPtr is invalid
	* \remark Produces a NazaraAssert if result is invalid
	*/
	bool ParseIPAddress(const char* addressPtr, UInt8 result[16], UInt16* port, bool* isIPv6, const char** endOfRead)
	{
		NazaraAssert(addressPtr, "Invalid address string");
		NazaraAssert(result, "Invalid result pointer");

		//find first colon, dot, and open bracket
		const char* colonPtr = std::strchr(addressPtr, ':');
		const char* dotPtr = std::strchr(addressPtr, '.');
		const char* openBracketPtr = std::strchr(addressPtr, '[');

		// we'll consider this to (probably) be IPv6 if we find an open
		// bracket, or an absence of dots, or if there is a colon, and it
		// precedes any dots that may or may not be there
		bool detectedIPv6 = openBracketPtr || !dotPtr || (colonPtr && (!dotPtr || colonPtr < dotPtr));

		// OK, now do a little further sanity check our initial guess...
		if (detectedIPv6)
		{
			// if open bracket, then must have close bracket that follows somewhere
			const char* closeBracketPtr = std::strchr(addressPtr, ']');
			if (openBracketPtr && (!closeBracketPtr || closeBracketPtr < openBracketPtr))
				return false;
		}
		else // probably ipv4
		{
			// dots must exist, and precede any colons
			if (!dotPtr || (colonPtr && colonPtr < dotPtr))
				return false;
		}

		// OK, there should be no correctly formed strings which are miscategorized,
		// and now any format errors will be found out as we continue parsing
		// according to plan.
		if (!detectedIPv6)	//try to parse as IPv4
		{
			// 4 dotted quad decimal; optional port if there is a colon
			// since there are just 4, and because the last one can be terminated
			// differently, I'm just going to unroll any potential loop.
			UInt8* resultPtr = result;

			for (unsigned int i = 0; i < 4; ++i)
			{
				unsigned int value;
				if (!Detail::ParseDecimal(addressPtr, &value, &addressPtr) || value > 255) //must be in range and followed by dot and nonempty
					return false;

				if (i != 3)
				{
					if (*addressPtr != '.')
						return false;

					addressPtr++;
				}

				*resultPtr++ = static_cast<UInt8>(value);
			}
		}
		else // try to parse as IPv6
		{
			UInt8* resultPtr;
			UInt8* zeroLoc;

			// up to 8 16-bit hex quantities, separated by colons, with at most one
			// empty quantity, acting as a stretchy run of zeros.  optional port
			// if there are brackets followed by colon and decimal port number.
			// A further form allows an ipv4 dotted quad instead of the last two
			// 16-bit quantities, but only if in the ipv4 space ::ffff:x:x .

			if (openBracketPtr)	// start past the open bracket, if it exists
				addressPtr = openBracketPtr + 1;

			resultPtr = result;
			zeroLoc = nullptr; // if we find a 'zero compression' location

			bool mappedIPv4 = false;
			unsigned int i;
			for (i = 0; i < 8; ++i) // we've got up to 8 of these, so we will use a loop
			{
				const char* savedPtr = addressPtr;
				unsigned int value;		// get value; these are hex
				if (!Detail::ParseHexadecimal(addressPtr, &value, &addressPtr)) // if empty, we are zero compressing; note the loc
				{
					if (zeroLoc) //there can be only one!
					{
						// unless it's a terminal empty field, then this is OK, it just means we're done with the host part
						if (resultPtr == zeroLoc)
						{
							--i;
							break;
						}

						return false; // otherwise, it's a format error
					}

					if (*addressPtr != ':') // empty field can only be via :
						return false;

					if (i == 0 && *++addressPtr != ':') // leading zero compression requires an extra peek, and adjustment
						return false;

					zeroLoc = resultPtr;
					++addressPtr;
				}
				else
				{
					if ('.' == *addressPtr) // special case of ipv4 convenience notation
					{
						addressPtr = savedPtr;

						// who knows how to parse ipv4?  we do!
						UInt8 ipv4[16];
						bool ipv6;
						if (!ParseIPAddress(addressPtr, ipv4, nullptr, &ipv6, &addressPtr) || ipv6) // must parse and must be ipv4
							return false;

						// transfer addrlocal into the present location
						for (unsigned int j = 0; j < 4; ++j)
							*(resultPtr++) = ipv4[j];

						++i; // pretend like we took another short, since the ipv4 effectively is two shorts
						mappedIPv4 = true; // remember how we got here for further validation later
						break; // totally done with address
					}

					if (value > 65535) // must be 16 bit quantity
						return false;

					*(resultPtr++) = value >> 8;
					*(resultPtr++) = value & 0xFF;

					if (*addressPtr == ':') // typical case inside; carry on
						++addressPtr;
					else // some other terminating character; done with this parsing parts
						break;
				}
			}

			// handle any zero compression we found
			if (zeroLoc)
			{
				std::ptrdiff_t nHead = (int) (zeroLoc - result);   // how much before zero compression
				std::ptrdiff_t nTail = i * 2 - nHead;              // how much after zero compression
				std::ptrdiff_t nZeros = 16 - nTail - nHead;        // how much zeros
				std::memmove(&result[16 - nTail], zeroLoc, nTail); // scootch stuff down
				std::memset(zeroLoc, 0, nZeros);                   // clear the compressed zeros
			}

			// validation of ipv4 subspace ::ffff:x.x
			if (mappedIPv4)
			{
				static const UInt8 abyPfx[] = {0,0, 0,0, 0,0, 0,0, 0,0, 0xFF,0xFF};
				if (std::memcmp(result, abyPfx, sizeof(abyPfx)) != 0)
					return false;
			}

			// close bracket
			if (openBracketPtr)
			{
				if (*addressPtr != ']')
					return false;

				++addressPtr;
			}
		}

		// if asked to read the port
		if (port)
		{
			if (*addressPtr == ':') // have port part
			{
				++addressPtr; // past the colon

				unsigned int portValue;
				if (!Detail::ParseDecimal(addressPtr, &portValue, nullptr) || portValue > 65535)
					return false;

				if (port)
					*port = static_cast<UInt16>(portValue);
			}
			else // finished just with IP address
				*port = 0; // indicate we have no port part
		}

		if (isIPv6)
			*isIPv6 = detectedIPv6;

		if (endOfRead)
			*endOfRead = addressPtr;

		return true;
	}
}
