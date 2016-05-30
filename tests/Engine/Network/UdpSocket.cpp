#include <Nazara/Network/UdpSocket.hpp>
#include <Catch/catch.hpp>

#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Network/NetPacket.hpp>

SCENARIO("UdpSocket", "[NETWORK][UDPSOCKET]")
{
	GIVEN("Two UdpSocket, one client, one server")
	{
		Nz::UInt16 port = 64256;
		Nz::UdpSocket server(Nz::NetProtocol_IPv4);
		REQUIRE(server.Bind(port) == Nz::SocketState_Bound);
		Nz::IpAddress serverIP = server.GetBoundAddress();
		REQUIRE(serverIP.IsValid());
		Nz::UdpSocket client(Nz::NetProtocol_IPv4);
		REQUIRE(client.Bind(port + 1) == Nz::SocketState_Bound);
		Nz::IpAddress clientIP = client.GetBoundAddress();
		REQUIRE(clientIP.IsValid());

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
