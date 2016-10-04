#include <Nazara/Network/TcpClient.hpp>
#include <Nazara/Network/TcpServer.hpp>
#include <Catch/catch.hpp>

#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Network/NetPacket.hpp>

#include <random>

SCENARIO("TCP", "[NETWORK][TCP]")
{
	GIVEN("Two TCP, one client, one server")
	{
		Nz::UInt16 port = 26456;
		Nz::TcpServer server;
		server.EnableBlocking(false);

		REQUIRE(server.Listen(Nz::NetProtocol_IPv4, port) == Nz::SocketState_Bound);

		Nz::IpAddress serverIP(Nz::IpAddress::LoopbackIpV4.ToIPv4(), port);
		REQUIRE(serverIP.IsValid());

		Nz::TcpClient client;
		REQUIRE(client.Connect(serverIP) == Nz::SocketState_Connecting);

		Nz::IpAddress clientIP = client.GetRemoteAddress();
		REQUIRE(clientIP.IsValid());

		Nz::TcpClient serverToClient;
		REQUIRE(server.AcceptClient(&serverToClient));

		WHEN("We send data from client")
		{
			Nz::NetPacket packet(1);
			Nz::Vector3f vector123(1.f, 2.f, 3.f);
			packet << vector123;
			REQUIRE(client.SendPacket(packet));

			THEN("We should get it on the server")
			{
				Nz::NetPacket resultPacket;
				REQUIRE(serverToClient.ReceivePacket(&resultPacket));
				Nz::Vector3f result;
				resultPacket >> result;
				REQUIRE(result == vector123);
			}
		}
	}
}
