#include <Nazara/Network/Algorithm.hpp>
#include <Nazara/Network/IpAddress.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <frozen/string.h>
#include <frozen/unordered_set.h>

SCENARIO("IpAddress", "[NETWORK][IPADDRESS]")
{
	WHEN("Parsing IP")
	{
		// IPv4
		CHECK(Nz::IpAddress("0.0.0.0") == Nz::IpAddress::AnyIpV4);
		CHECK(Nz::IpAddress("0.0.0.0:0") == Nz::IpAddress::AnyIpV4);
		CHECK(Nz::IpAddress("0.0.0.0:12345") == Nz::IpAddress(0, 0, 0, 0, 12345));

		Nz::IpAddress loopbackIpV4("127.0.0.1");
		CHECK(loopbackIpV4 == Nz::IpAddress::LoopbackIpV4);
		CHECK(static_cast<bool>(loopbackIpV4));
		CHECK(loopbackIpV4.IsValid());
		CHECK(loopbackIpV4.IsLoopback());
		CHECK(loopbackIpV4.GetPort() == 0);
		CHECK(loopbackIpV4.GetProtocol() == Nz::NetProtocol::IPv4);
		CHECK(loopbackIpV4.ToString() == "127.0.0.1");
		CHECK(loopbackIpV4.ToUInt32() == 0x7F000001);

		Nz::IpAddress loopbackIpV4WithPort("127.0.0.1:80");
		CHECK(loopbackIpV4WithPort != Nz::IpAddress::LoopbackIpV4);
		CHECK(static_cast<bool>(loopbackIpV4WithPort));
		CHECK(loopbackIpV4WithPort.IsValid());
		CHECK(loopbackIpV4WithPort.IsLoopback());
		CHECK(loopbackIpV4WithPort.GetPort() == 80);
		CHECK(loopbackIpV4WithPort.GetProtocol() == Nz::NetProtocol::IPv4);
		CHECK(loopbackIpV4WithPort.ToString() == "127.0.0.1:80");
		CHECK(loopbackIpV4WithPort.ToUInt32() == 0x7F000001);

		Nz::IpAddress nonregularLoopbackIpV4("127.147.254.15:41235");
		CHECK(nonregularLoopbackIpV4 != Nz::IpAddress::LoopbackIpV4);
		CHECK(static_cast<bool>(nonregularLoopbackIpV4));
		CHECK(nonregularLoopbackIpV4.IsValid());
		CHECK(nonregularLoopbackIpV4.IsLoopback());
		CHECK(nonregularLoopbackIpV4.GetPort() == 41235);
		CHECK(nonregularLoopbackIpV4.GetProtocol() == Nz::NetProtocol::IPv4);
		CHECK(nonregularLoopbackIpV4.ToString() == "127.147.254.15:41235");
		CHECK(nonregularLoopbackIpV4.ToUInt32() == 0x7F93FE0F);

		nonregularLoopbackIpV4.SetPort(14738);
		CHECK(nonregularLoopbackIpV4.GetPort() == 14738);
		CHECK(nonregularLoopbackIpV4.ToString() == "127.147.254.15:14738");

		// IPv6
		CHECK(Nz::IpAddress("::") == Nz::IpAddress::AnyIpV6);
		CHECK(Nz::IpAddress("[::]:0") == Nz::IpAddress::AnyIpV6);
		CHECK(Nz::IpAddress("[::]:80") == Nz::IpAddress(0, 0, 0, 0, 0, 0, 0, 0, 80));

		CHECK(Nz::IpAddress("1:2:3:4:5:6:7:8") == Nz::IpAddress(1, 2, 3, 4, 5, 6, 7, 8));
		CHECK(Nz::IpAddress("1:2:3:4:5::7:8") == Nz::IpAddress(1, 2, 3, 4, 5, 0, 7, 8));
		CHECK(Nz::IpAddress("1:2:3:4::7:8") == Nz::IpAddress(1, 2, 3, 4, 0, 0, 7, 8));
		CHECK(Nz::IpAddress("1:2:3::7:8") == Nz::IpAddress(1, 2, 3, 0, 0, 0, 7, 8));
		CHECK(Nz::IpAddress("1:2::5:6:7:8") == Nz::IpAddress(1, 2, 0, 0, 5, 6, 7, 8));
		CHECK(Nz::IpAddress("1::7:8") == Nz::IpAddress(1, 0, 0, 0, 0, 0, 7, 8));
		CHECK(Nz::IpAddress("1::8") == Nz::IpAddress(1, 0, 0, 0, 0, 0, 0, 8));
		CHECK(Nz::IpAddress("::8") == Nz::IpAddress(0, 0, 0, 0, 0, 0, 0, 8));
		CHECK(Nz::IpAddress("1::") == Nz::IpAddress(1, 0, 0, 0, 0, 0, 0, 0));
		CHECK(Nz::IpAddress("1:2::8") == Nz::IpAddress(1, 2, 0, 0, 0, 0, 0, 8));

		CHECK(Nz::IpAddress("2001:41d0:2:d4cd::") == Nz::IpAddress(0x2001, 0x41D0, 0x2, 0xD4CD, 0, 0, 0, 0));
		CHECK(Nz::IpAddress("[2001:41D0:2:D4CD::]:80") == Nz::IpAddress(0x2001, 0x41D0, 0x2, 0xD4CD, 0, 0, 0, 0, 80));

		CHECK(Nz::IpAddress(0x2001, 0x41D0, 0x2, 0, 0, 0, 0, 0xD4CD).ToString() == "2001:41d0:2::d4cd");
		CHECK(Nz::IpAddress(0x2001, 0x41D0, 0x2, 0, 0, 0, 0, 0xD4CD, 443).ToString() == "[2001:41d0:2::d4cd]:443");
		CHECK(Nz::IpAddress(0x2001, 0x41D0, 0x2, 0xDEAD, 0xBEEF, 0x42, 0x2022, 0xD4CD, 443).ToString() == "[2001:41d0:2:dead:beef:42:2022:d4cd]:443");

		// All of theses are differents representations of the same IPv6 address
		std::array<std::string, 8> testAddresses {
			"2001:db8:0:0:1:0:0:1",
			"2001:0db8:0:0:1:0:0:1",
			"2001:db8::1:0:0:1",
			"2001:db8::0:1:0:0:1",
			"2001:0db8::1:0:0:1",
			"2001:db8:0:0:1::1",
			"2001:db8:0000:0:1::1",
			"2001:DB8:0:0:1::1"
		};

		Nz::IpAddress referenceAddress(0x2001, 0xDB8, 0, 0, 1, 0, 0, 1);
		for (const std::string& str : testAddresses)
		{
			INFO(str);
			CHECK(Nz::IpAddress(str) == referenceAddress);
		}

		Nz::IpAddress loopbackIpV6("::1");
		CHECK(loopbackIpV6 == Nz::IpAddress::LoopbackIpV6);
		CHECK(loopbackIpV6.IsValid());
		CHECK(loopbackIpV6.IsLoopback());
		CHECK(loopbackIpV6.GetPort() == 0);
		CHECK(loopbackIpV6.GetProtocol() == Nz::NetProtocol::IPv6);
		CHECK(loopbackIpV6.ToString() == "::1");

		Nz::IpAddress loopbackIpV6WithPort("[::1]:443");
		CHECK(loopbackIpV6WithPort != Nz::IpAddress::LoopbackIpV6);
		CHECK(loopbackIpV6WithPort.IsValid());
		CHECK(loopbackIpV6WithPort.IsLoopback());
		CHECK(loopbackIpV6WithPort.GetPort() == 443);
		CHECK(loopbackIpV6WithPort.GetProtocol() == Nz::NetProtocol::IPv6);
		CHECK(loopbackIpV6WithPort.ToString() == "[::1]:443");

		// IPv4-mapped IPv6
		Nz::IpAddress ipv4MappedIPv6("::ffff:192.168.173.22");
		CHECK(ipv4MappedIPv6.IsValid());
		CHECK_FALSE(ipv4MappedIPv6.IsLoopback());
		CHECK(ipv4MappedIPv6.GetProtocol() == Nz::NetProtocol::IPv6);
		CHECK(ipv4MappedIPv6.ToString() == "::ffff:192.168.173.22");
		CHECK(ipv4MappedIPv6 == Nz::IpAddress(0, 0, 0, 0, 0, 0xFFFF, 0xC0A8, 0xAD16));

		Nz::IpAddress ipv4MappedIPv6WithPort("::ffff:192.168.173.22");
		CHECK(ipv4MappedIPv6WithPort.IsValid());
		CHECK_FALSE(ipv4MappedIPv6WithPort.IsLoopback());
		CHECK(ipv4MappedIPv6WithPort.GetProtocol() == Nz::NetProtocol::IPv6);
		CHECK(ipv4MappedIPv6WithPort.ToString() == "::ffff:192.168.173.22");
		CHECK(ipv4MappedIPv6WithPort == Nz::IpAddress(0, 0, 0, 0, 0, 0xFFFF, 0xC0A8, 0xAD16));

		// Invalid addresses
		CHECK_FALSE(Nz::IpAddress("000.123.456.789").IsValid());
		CHECK_FALSE(Nz::IpAddress("0.0.0.0SomeGarbage").IsValid());
		CHECK_FALSE(Nz::IpAddress("0.0.0.0:Hell0").IsValid());
		CHECK_FALSE(Nz::IpAddress("Hey0.0.0.0:12345").IsValid());
		CHECK_FALSE(Nz::IpAddress("::1:ffff:192.168.173.22").IsValid());
		CHECK_FALSE(Nz::IpAddress("::ffff:123.412.210.230").IsValid());
		CHECK_FALSE(Nz::IpAddress("::ffff:127.0.0.0.1").IsValid());
		CHECK_FALSE(Nz::IpAddress("::ffff:0:255.255.255.255").IsValid());
	}

	WHEN("We resolve Cloudflare DNS")
	{
		std::vector<Nz::HostnameInfo> hostnameInfos = Nz::IpAddress::ResolveHostname(Nz::NetProtocol::Any, "one.one.one.one");
		CHECK(!hostnameInfos.empty());

		frozen::unordered_set expectedAddresses = frozen::make_unordered_set<frozen::string>({
			"1.1.1.1",
			"1.0.0.1",
			"2606:4700:4700::1111",
			"2606:4700:4700::1001"
		});

		for (const Nz::HostnameInfo& hostnameInfo : hostnameInfos)
		{
			const std::string& addressStr = hostnameInfo.address.ToString(false);
			INFO(addressStr);
			CHECK(expectedAddresses.count(frozen::string(addressStr)) > 0);
		}

		AND_WHEN("We resolve back the IP addresses to the hostname")
		{
			CHECK(Nz::IpAddress::ResolveAddress(Nz::IpAddress("1.1.1.1")) == "one.one.one.one");
			CHECK(Nz::IpAddress::ResolveAddress(Nz::IpAddress("2606:4700:4700::1111")) == "one.one.one.one");
		}
	}
}
