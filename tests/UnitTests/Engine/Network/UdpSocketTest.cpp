#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Network/UdpSocket.hpp>
#include <Nazara/Network/NetPacket.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

SCENARIO("UdpSocket", "[NETWORK][UDPSOCKET]")
{
	GIVEN("Two UdpSocket, one client, one server")
	{
		Nz::UdpSocket server(Nz::NetProtocol::IPv4);
		REQUIRE(server.Bind(0) == Nz::SocketState::Bound);

		Nz::UInt16 port = server.GetBoundPort();

		Nz::IpAddress serverIP(Nz::IpAddress::LoopbackIpV4.ToIPv4(), port);
		REQUIRE(serverIP.IsValid());

		Nz::UdpSocket client(Nz::NetProtocol::IPv4);
		CHECK_FALSE(client.IsBroadcastingEnabled());
		CHECK(client.QueryMaxDatagramSize() > 1500);

		WHEN("We send data from client")
		{
			Nz::NetPacket packet(1);
			Nz::Vector3f vector123(1.f, 2.f, 3.f);
			packet << vector123;
			REQUIRE(client.SendPacket(serverIP, packet));

			THEN("We should get it on the server")
			{
				Nz::NetPacket resultPacket;
				Nz::IpAddress fromIp;
				REQUIRE(server.ReceivePacket(&resultPacket, &fromIp));

				Nz::Vector3f result;
				resultPacket >> result;
				REQUIRE(result == vector123);
			}
		}
	}
}
