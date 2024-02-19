#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Network/NetPacket.hpp>
#include <Nazara/Network/TcpClient.hpp>
#include <Nazara/Network/TcpServer.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <thread>

SCENARIO("TCP", "[NETWORK][TCP]")
{
	GIVEN("Two TCP, one client, one server")
	{
		Nz::TcpServer server;
		server.EnableBlocking(false);

		REQUIRE(server.Listen(Nz::NetProtocol::IPv4, 0) == Nz::SocketState::Bound);

		Nz::IpAddress serverIP(Nz::IpAddress::LoopbackIpV4.ToIPv4(), server.GetBoundPort());
		REQUIRE(serverIP.IsValid());

		Nz::TcpClient client;
		CHECK(client.WaitForConnected(100) == Nz::SocketState::NotConnected);
		REQUIRE(client.Connect(serverIP) == Nz::SocketState::Connecting);

		Nz::IpAddress clientIP = client.GetRemoteAddress();
		CHECK(clientIP.IsValid());

		REQUIRE(client.WaitForConnected(100) == Nz::SocketState::Connected);

		CHECK(client.IsBlockingEnabled());
		CHECK_FALSE(client.IsKeepAliveEnabled());
		CHECK_FALSE(client.IsLowDelayEnabled());
		CHECK(client.QueryReceiveBufferSize() > 0);
		CHECK(client.QuerySendBufferSize() > 0);

		Nz::TcpClient serverToClient;
		REQUIRE(server.AcceptClient(&serverToClient));

		WHEN("We send data from client")
		{
			CHECK(serverToClient.EndOfStream());

			Nz::NetPacket packet(1);
			Nz::Vector3f vector123(1.f, 2.f, 3.f);
			packet << vector123;
			REQUIRE(client.SendPacket(packet));

			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			THEN("We should get it on the server")
			{
				CHECK(!serverToClient.EndOfStream());

				Nz::NetPacket resultPacket;
				REQUIRE(serverToClient.ReceivePacket(&resultPacket));

				Nz::Vector3f result;
				resultPacket >> result;

				CHECK(result == vector123);
			}
		}
	}
}
