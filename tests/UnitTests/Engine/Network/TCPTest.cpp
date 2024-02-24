#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/ByteStream.hpp>
#include <Nazara/Math/Vector3.hpp>
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

			Nz::ByteArray byteArray;
			Nz::ByteStream packet(&byteArray);
			Nz::Vector3f vector123(1.f, 2.f, 3.f);
			packet << vector123;

			std::size_t sent;
			REQUIRE(client.Send(byteArray.GetConstBuffer(), byteArray.GetSize(), &sent));
			CHECK(sent == byteArray.GetSize());

			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			THEN("We should get it on the server")
			{
				CHECK(!serverToClient.EndOfStream());

				Nz::ByteArray resultPacket(byteArray.GetSize());

				std::size_t received;
				REQUIRE(serverToClient.Receive(resultPacket.GetBuffer(), resultPacket.GetSize(), &received));
				CHECK(received == resultPacket.GetSize());

				Nz::ByteStream packetOut(&resultPacket);
				Nz::Vector3f result;
				packetOut >> result;

				CHECK(result == vector123);
			}
		}
	}
}
