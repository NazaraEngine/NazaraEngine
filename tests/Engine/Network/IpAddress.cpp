#include <Nazara/Network/IpAddress.hpp>
#include <Catch/catch.hpp>

SCENARIO("IpAddress", "[NETWORK][IPADDRESS]")
{
	GIVEN("Two default IpAddress")
	{
		Nz::IpAddress ipAddressV4;
		Nz::IpAddress ipAddressV6;

		WHEN("We parse localhost")
		{
			Nz::String localhostIPv4 = "127.0.0.1";
			Nz::String localhostIPv6 = "::1";
			REQUIRE(ipAddressV4.BuildFromAddress(localhostIPv4.GetConstBuffer()));
			REQUIRE(ipAddressV6.BuildFromAddress(localhostIPv6.GetConstBuffer()));

			THEN("It's the loop back")
			{
				REQUIRE(ipAddressV4.IsLoopback());
				REQUIRE(ipAddressV6.IsLoopback());
			}
		}
	}

	GIVEN("No IpAddress")
	{
		WHEN("We get the IP of Nazara")
		{
			std::vector<Nz::HostnameInfo> hostnameInfos = Nz::IpAddress::ResolveHostname(Nz::NetProtocol_IPv4, "nazara.digitalpulsesoftware.net");

			THEN("Result is not null")
			{
				REQUIRE(!hostnameInfos.empty());
			}
		}

		WHEN("We convert IP to hostname")
		{
			Nz::IpAddress google(8, 8, 8, 8);
			THEN("Google (DNS) is 8.8.8.8")
			{
					REQUIRE(Nz::IpAddress::ResolveAddress(google) == "google-public-dns-a.google.com");
			}
		}
	}
}
