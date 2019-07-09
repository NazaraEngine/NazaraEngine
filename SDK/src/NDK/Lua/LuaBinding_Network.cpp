// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/Lua/LuaBinding_Network.hpp>
#include <NDK/LuaAPI.hpp>

namespace Ndk
{
	std::unique_ptr<LuaBinding_Base> LuaBinding_Base::BindNetwork(LuaBinding& binding)
	{
		return std::make_unique<LuaBinding_Network>(binding);
	}

	LuaBinding_Network::LuaBinding_Network(LuaBinding& binding) :
	LuaBinding_Base(binding)
	{
		/*********************************** Nz::AbstractSocket **********************************/
		abstractSocket.Reset("AbstractSocket");
		{
			abstractSocket.BindMethod("Close", &Nz::AbstractSocket::Close);
			abstractSocket.BindMethod("EnableBlocking", &Nz::AbstractSocket::EnableBlocking);
			abstractSocket.BindMethod("GetLastError", &Nz::AbstractSocket::GetLastError);
			abstractSocket.BindMethod("GetState", &Nz::AbstractSocket::GetState);
			abstractSocket.BindMethod("GetType", &Nz::AbstractSocket::GetType);
			abstractSocket.BindMethod("IsBlockingEnabled", &Nz::AbstractSocket::IsBlockingEnabled);
			abstractSocket.BindMethod("QueryAvailableBytes", &Nz::AbstractSocket::QueryAvailableBytes);
		}

		/*********************************** Nz::IpAddress **********************************/
		ipAddress.Reset("IpAddress");
		{
			ipAddress.SetConstructor([] (Nz::LuaState& lua, Nz::IpAddress* instance, std::size_t argumentCount)
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 9U);

				int argIndex = 1;
				switch (argCount)
				{
					case 0:
						Nz::PlacementNew(instance);
						return true;

					case 1:
						Nz::PlacementNew(instance, lua.CheckString(argIndex));
						return true;

					case 4:
					case 5:
					{
						Nz::UInt8 a = lua.Check<Nz::UInt8>(&argIndex);
						Nz::UInt8 b = lua.Check<Nz::UInt8>(&argIndex);
						Nz::UInt8 c = lua.Check<Nz::UInt8>(&argIndex);
						Nz::UInt8 d = lua.Check<Nz::UInt8>(&argIndex);
						Nz::UInt16 port = lua.Check<Nz::UInt16>(&argIndex, 0);

						Nz::PlacementNew(instance, a, b, c, d, port);
						return true;
					}

					case 8:
					case 9:
					{
						Nz::UInt16 a = lua.Check<Nz::UInt16>(&argIndex);
						Nz::UInt16 b = lua.Check<Nz::UInt16>(&argIndex);
						Nz::UInt16 c = lua.Check<Nz::UInt16>(&argIndex);
						Nz::UInt16 d = lua.Check<Nz::UInt16>(&argIndex);
						Nz::UInt16 e = lua.Check<Nz::UInt16>(&argIndex);
						Nz::UInt16 f = lua.Check<Nz::UInt16>(&argIndex);
						Nz::UInt16 g = lua.Check<Nz::UInt16>(&argIndex);
						Nz::UInt16 h = lua.Check<Nz::UInt16>(&argIndex);
						Nz::UInt16 port = lua.Check<Nz::UInt16>(&argIndex, 0);

						Nz::PlacementNew(instance, a, b, c, d, e, f, g, h, port);
						return true;
					}
				}

				lua.Error("No matching overload for constructor");
				return false;
			});

			ipAddress.BindMethod("GetPort", &Nz::IpAddress::GetPort);
			ipAddress.BindMethod("GetProtocol", &Nz::IpAddress::GetProtocol);
			ipAddress.BindMethod("IsLoopback", &Nz::IpAddress::IsLoopback);
			ipAddress.BindMethod("IsValid", &Nz::IpAddress::IsValid);
			ipAddress.BindMethod("ToUInt32", &Nz::IpAddress::ToUInt32);
			ipAddress.BindMethod("__tostring", &Nz::IpAddress::ToString);

			ipAddress.BindStaticMethod("ResolveAddress", [] (Nz::LuaState& state) -> int
			{
				Nz::String service;
				Nz::ResolveError error = Nz::ResolveError_Unknown;

				int argIndex = 2;
				Nz::String hostName = Nz::IpAddress::ResolveAddress(state.Check<Nz::IpAddress>(&argIndex), &service, &error);

				if (error == Nz::ResolveError_NoError)
				{
					state.Push(hostName);
					state.Push(service);
					return 2;
				}
				else
				{
					state.PushBoolean(false);
					state.Push(error);
					return 2;
				}
			});

			ipAddress.BindStaticMethod("ResolveHostname", [] (Nz::LuaState& state) -> int
			{
				Nz::ResolveError error = Nz::ResolveError_Unknown;

				int argIndex = 2;
				Nz::NetProtocol protocol = state.Check<Nz::NetProtocol>(&argIndex);
				Nz::String      hostname = state.Check<Nz::String>(&argIndex);
				Nz::String      service = state.Check<Nz::String>(&argIndex, "http");

				std::vector<Nz::HostnameInfo> addresses = Nz::IpAddress::ResolveHostname(protocol, hostname, service, &error);
				if (error == Nz::ResolveError_NoError)
				{
					int index = 1;
					state.PushTable(addresses.size());
					for (Nz::HostnameInfo& info : addresses)
					{
						state.PushInteger(index++);
						state.PushTable(0, 4);
						state.PushField("Address", std::move(info.address));
						state.PushField("CanonicalName", std::move(info.canonicalName));
						state.PushField("Protocol", std::move(info.protocol));
						state.PushField("SocketType", std::move(info.socketType));
						state.SetTable();
					}

					return 1;
				}
				else
				{
					state.PushBoolean(false);
					state.Push(error);
					return 2;
				}
			});
		}

		udpSocket.Reset("UdpSocket");
		{
			udpSocket.Inherit<Nz::AbstractSocket>(abstractSocket);

			udpSocket.BindDefaultConstructor();

			udpSocket.BindMethod("Create", &Nz::UdpSocket::Create);
			udpSocket.BindMethod("EnableBroadcasting", &Nz::UdpSocket::EnableBroadcasting);
			udpSocket.BindMethod("GetBoundAddress", &Nz::UdpSocket::GetBoundAddress);
			udpSocket.BindMethod("GetBoundPort", &Nz::UdpSocket::GetBoundPort);
			udpSocket.BindMethod("IsBroadcastingEnabled", &Nz::UdpSocket::IsBroadcastingEnabled);
			udpSocket.BindMethod("QueryMaxDatagramSize", &Nz::UdpSocket::QueryMaxDatagramSize);

			udpSocket.BindMethod("Bind", [](Nz::LuaState& lua, Nz::UdpSocket& socket, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;
				if (lua.IsOfType(argIndex, "IpAddress"))
					return lua.Push(socket.Bind(*static_cast<Nz::IpAddress*>(lua.ToUserdata(argIndex))));
				else
					return lua.Push(socket.Bind(lua.Check<Nz::UInt16>(&argIndex)));
			});

			udpSocket.BindMethod("Receive", [](Nz::LuaState& lua, Nz::UdpSocket& socket, std::size_t /*argumentCount*/) -> int
			{
				Nz::IpAddress from;

				std::array<char, 0xFFFF> buffer;
				std::size_t received;
				if (socket.Receive(buffer.data(), buffer.size(), &from, &received))
				{
					lua.PushBoolean(true);
					lua.PushString(from.ToString());
					lua.PushString(buffer.data(), received);
					return 3;
				}

				lua.PushBoolean(false);
				return 1;
			});

			udpSocket.BindMethod("Send", [](Nz::LuaState& lua, Nz::UdpSocket& socket, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;
				Nz::String to = lua.Check<Nz::String>(&argIndex);

				std::size_t bufferLength;
				const char* buffer = lua.CheckString(argIndex, &bufferLength);

				std::size_t sent;
				bool ret;
				if ((ret = socket.Send(Nz::IpAddress(to), buffer, bufferLength, &sent)) != true)
					sent = 0;

				return lua.Push(std::make_pair(ret, sent));
			});
		}
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the Network classes
	*/
	void LuaBinding_Network::Register(Nz::LuaState& state)
	{
		// Classes
		abstractSocket.Register(state);
		ipAddress.Register(state);
		udpSocket.Register(state);

		// Enums

		// Nz::NetProtocol
		static_assert(Nz::NetProtocol_Max + 1 == 4, "Nz::NetProtocol has been updated but change was not reflected to Lua binding");
		state.PushTable(0, 4);
		{
			state.PushField("Any",     Nz::NetProtocol_Any);
			state.PushField("IPv4",    Nz::NetProtocol_IPv4);
			state.PushField("IPv6",    Nz::NetProtocol_IPv6);
			state.PushField("Unknown", Nz::NetProtocol_Unknown);
		}
		state.SetGlobal("NetProtocol");

		// Nz::PacketPriority
		static_assert(Nz::PacketPriority_Max + 1 == 4, "Nz::PacketPriority has been updated but change was not reflected to Lua binding");
		state.PushTable(0, 6);
		{
			state.PushField("High",      Nz::PacketPriority_High);
			state.PushField("Highest",   Nz::PacketPriority_Highest);
			state.PushField("Immediate", Nz::PacketPriority_Immediate);
			state.PushField("Medium",    Nz::PacketPriority_Medium);
			state.PushField("Low",       Nz::PacketPriority_Low);
			state.PushField("Lowest",    Nz::PacketPriority_Lowest);
		}
		state.SetGlobal("PacketPriority");

		// Nz::PacketReliability
		static_assert(Nz::PacketReliability_Max + 1 == 3, "Nz::PacketReliability has been updated but change was not reflected to Lua binding");
		state.PushTable(0, 3);
		{
			state.PushField("Reliable",        Nz::PacketReliability_Reliable);
			state.PushField("ReliableOrdered", Nz::PacketReliability_ReliableOrdered);
			state.PushField("Unreliable",      Nz::PacketReliability_Unreliable);
		}
		state.SetGlobal("PacketReliability");

		// Nz::ResolveError
		static_assert(Nz::ResolveError_Max + 1 == 9, "Nz::ResolveError has been updated but change was not reflected to Lua binding");
		state.PushTable(0, 9);
		{
			state.PushField("Internal",             Nz::ResolveError_Internal);
			state.PushField("ResourceError",        Nz::ResolveError_ResourceError);
			state.PushField("NoError",              Nz::ResolveError_NoError);
			state.PushField("NonRecoverable",       Nz::ResolveError_NonRecoverable);
			state.PushField("NotFound",             Nz::ResolveError_NotFound);
			state.PushField("NotInitialized",       Nz::ResolveError_NotInitialized);
			state.PushField("ProtocolNotSupported", Nz::ResolveError_ProtocolNotSupported);
			state.PushField("TemporaryFailure",     Nz::ResolveError_TemporaryFailure);
			state.PushField("Unknown",              Nz::ResolveError_Unknown);
		}
		state.SetGlobal("ResolveError");

		// Nz::SocketError
		static_assert(Nz::SocketError_Max + 1 == 16, "Nz::SocketError has been updated but change was not reflected to Lua binding");
		state.PushTable(0, 16);
		{
			state.PushField("AddressNotAvailable", Nz::SocketError_AddressNotAvailable);
			state.PushField("ConnectionClosed",    Nz::SocketError_ConnectionClosed);
			state.PushField("ConnectionRefused",   Nz::SocketError_ConnectionRefused);
			state.PushField("DatagramSize",        Nz::SocketError_DatagramSize);
			state.PushField("Internal",            Nz::SocketError_Internal);
			state.PushField("Interrupted",         Nz::SocketError_Interrupted);
			state.PushField("Packet",              Nz::SocketError_Packet);
			state.PushField("NetworkError",        Nz::SocketError_NetworkError);
			state.PushField("NoError",             Nz::SocketError_NoError);
			state.PushField("NotInitialized",      Nz::SocketError_NotInitialized);
			state.PushField("NotSupported",        Nz::SocketError_NotSupported);
			state.PushField("ResolveError",        Nz::SocketError_ResolveError);
			state.PushField("ResourceError",       Nz::SocketError_ResourceError);
			state.PushField("TimedOut",            Nz::SocketError_TimedOut);
			state.PushField("Unknown",             Nz::SocketError_Unknown);
			state.PushField("UnreachableHost",     Nz::SocketError_UnreachableHost);
		}
		state.SetGlobal("SocketError");

		// Nz::SocketState
		static_assert(Nz::SocketState_Max + 1 == 5, "Nz::SocketState has been updated but change was not reflected to Lua binding");
		state.PushTable(0, 5);
		{
			state.PushField("Bound",        Nz::SocketState_Bound);
			state.PushField("Connecting",   Nz::SocketState_Connecting);
			state.PushField("Connected",    Nz::SocketState_Connected);
			state.PushField("NotConnected", Nz::SocketState_NotConnected);
			state.PushField("Resolving",    Nz::SocketState_Resolving);
		}
		state.SetGlobal("SocketState");

		// Nz::SocketType
		static_assert(Nz::SocketType_Max + 1 == 4, "Nz::SocketState has been updated but change was not reflected to Lua binding");
		state.PushTable(0, 4);
		{
			state.PushField("Raw",     Nz::SocketType_Raw);
			state.PushField("TCP",     Nz::SocketType_TCP);
			state.PushField("UDP",     Nz::SocketType_UDP);
			state.PushField("Unknown", Nz::SocketType_Unknown);
		}
		state.SetGlobal("SocketType");
	}
}
