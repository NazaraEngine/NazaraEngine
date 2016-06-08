#include <Nazara/Network/RUdpConnection.hpp>
#include <Catch/catch.hpp>

#include <Nazara/Math/Vector3.hpp>

SCENARIO("RUdpConnection", "[NETWORK][RUDPCONNECTION]")
{
	GIVEN("Two RUdpConnection, one client, one server")
	{
		Nz::UInt16 port = 64266;
		Nz::RUdpConnection server;
		REQUIRE(server.Listen(Nz::NetProtocol_IPv4, port));
		Nz::IpAddress serverIP = server.GetBoundAddress();
		REQUIRE(serverIP.IsValid());
		Nz::RUdpConnection client;
		REQUIRE(client.Listen(Nz::NetProtocol_IPv4, port + 1));
		Nz::IpAddress clientIP = client.GetBoundAddress();
		REQUIRE(client.Connect(serverIP));
		REQUIRE(clientIP.IsValid());

		WHEN("We send data from client")
		{
			Nz::NetPacket packet(1);
			Nz::Vector3f vector123(1.f, 2.f, 3.f);
			packet << vector123;
			REQUIRE(client.Send(serverIP, Nz::PacketPriority_Immediate, Nz::PacketReliability_Reliable, packet));
			client.Update();

			THEN("We should get it on the server")
			{
				Nz::RUdpMessage rudpMessage;
				server.Update();
				REQUIRE(server.PollMessage(&rudpMessage));
				Nz::Vector3f result;
				rudpMessage.data >> result;
				REQUIRE(result == vector123);
			}
		}
	}
}
