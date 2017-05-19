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
			ipAddress.SetConstructor([] (Nz::LuaInstance& lua, Nz::IpAddress* instance, std::size_t argumentCount)
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

			ipAddress.BindStaticMethod("ResolveAddress", [] (Nz::LuaInstance& instance) -> int
			{
				Nz::String service;
				Nz::ResolveError error = Nz::ResolveError_Unknown;

				int argIndex = 2;
				Nz::String hostName = Nz::IpAddress::ResolveAddress(instance.Check<Nz::IpAddress>(&argIndex), &service, &error);

				if (error == Nz::ResolveError_NoError)
				{
					instance.Push(hostName);
					instance.Push(service);
					return 2;
				}
				else
				{
					instance.PushBoolean(false);
					instance.Push(error);
					return 2;
				}
			});

			ipAddress.BindStaticMethod("ResolveHostname", [] (Nz::LuaInstance& instance) -> int
			{
				Nz::ResolveError error = Nz::ResolveError_Unknown;

				int argIndex = 2;
				Nz::NetProtocol protocol = instance.Check<Nz::NetProtocol>(&argIndex);
				Nz::String      hostname = instance.Check<Nz::String>(&argIndex);
				Nz::String      service = instance.Check<Nz::String>(&argIndex, "http");

				std::vector<Nz::HostnameInfo> addresses = Nz::IpAddress::ResolveHostname(protocol, hostname, service, &error);
				if (error == Nz::ResolveError_NoError)
				{
					int index = 1;
					instance.PushTable(addresses.size());
					for (Nz::HostnameInfo& info : addresses)
					{
						instance.PushInteger(index++);
						instance.PushTable(0, 4);
						instance.PushField("Address", std::move(info.address));
						instance.PushField("CanonicalName", std::move(info.canonicalName));
						instance.PushField("Protocol", std::move(info.protocol));
						instance.PushField("SocketType", std::move(info.socketType));
						instance.SetTable();
					}

					return 1;
				}
				else
				{
					instance.PushBoolean(false);
					instance.Push(error);
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

			udpSocket.BindMethod("Bind", [](Nz::LuaInstance& lua, Nz::UdpSocket& instance, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;
				if (lua.IsOfType(argIndex, "IpAddress"))
					return lua.Push(instance.Bind(*static_cast<Nz::IpAddress*>(lua.ToUserdata(argIndex))));
				else
					return lua.Push(instance.Bind(lua.Check<Nz::UInt16>(&argIndex)));
			});

			udpSocket.BindMethod("Receive", [](Nz::LuaInstance& lua, Nz::UdpSocket& instance, std::size_t /*argumentCount*/) -> int
			{
				Nz::IpAddress from;

				std::array<char, 0xFFFF> buffer;
				std::size_t received;
				if (instance.Receive(buffer.data(), buffer.size(), &from, &received))
				{
					lua.PushBoolean(true);
					lua.PushString(from.ToString());
					lua.PushString(buffer.data(), received);
					return 3;
				}

				lua.PushBoolean(false);
				return 1;
			});

			udpSocket.BindMethod("Send", [](Nz::LuaInstance& lua, Nz::UdpSocket& instance, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;
				Nz::String to = lua.Check<Nz::String>(&argIndex);

				std::size_t bufferLength;
				const char* buffer = lua.CheckString(argIndex, &bufferLength);

				std::size_t sent;
				bool ret;
				if ((ret = instance.Send(Nz::IpAddress(to), buffer, bufferLength, &sent)) != true)
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
	void LuaBinding_Network::Register(Nz::LuaInstance& instance)
	{
		// Classes
		abstractSocket.Register(instance);
		ipAddress.Register(instance);
		udpSocket.Register(instance);

		// Enums

		// Nz::NetProtocol
		static_assert(Nz::NetProtocol_Max + 1 == 4, "Nz::NetProtocol has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 4);
		{
			instance.PushField("Any",     Nz::NetProtocol_Any);
			instance.PushField("IPv4",    Nz::NetProtocol_IPv4);
			instance.PushField("IPv6",    Nz::NetProtocol_IPv6);
			instance.PushField("Unknown", Nz::NetProtocol_Unknown);
		}
		instance.SetGlobal("NetProtocol");

		// Nz::PacketPriority
		static_assert(Nz::PacketPriority_Max + 1 == 4, "Nz::PacketPriority has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 6);
		{
			instance.PushField("High",      Nz::PacketPriority_High);
			instance.PushField("Highest",   Nz::PacketPriority_Highest);
			instance.PushField("Immediate", Nz::PacketPriority_Immediate);
			instance.PushField("Medium",    Nz::PacketPriority_Medium);
			instance.PushField("Low",       Nz::PacketPriority_Low);
			instance.PushField("Lowest",    Nz::PacketPriority_Lowest);
		}
		instance.SetGlobal("PacketPriority");

		// Nz::PacketReliability
		static_assert(Nz::PacketReliability_Max + 1 == 3, "Nz::PacketReliability has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 3);
		{
			instance.PushField("Reliable",        Nz::PacketReliability_Reliable);
			instance.PushField("ReliableOrdered", Nz::PacketReliability_ReliableOrdered);
			instance.PushField("Unreliable",      Nz::PacketReliability_Unreliable);
		}
		instance.SetGlobal("PacketReliability");

		// Nz::ResolveError
		static_assert(Nz::ResolveError_Max + 1 == 9, "Nz::ResolveError has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 9);
		{
			instance.PushField("Internal",             Nz::ResolveError_Internal);
			instance.PushField("ResourceError",        Nz::ResolveError_ResourceError);
			instance.PushField("NoError",              Nz::ResolveError_NoError);
			instance.PushField("NonRecoverable",       Nz::ResolveError_NonRecoverable);
			instance.PushField("NotFound",             Nz::ResolveError_NotFound);
			instance.PushField("NotInitialized",       Nz::ResolveError_NotInitialized);
			instance.PushField("ProtocolNotSupported", Nz::ResolveError_ProtocolNotSupported);
			instance.PushField("TemporaryFailure",     Nz::ResolveError_TemporaryFailure);
			instance.PushField("Unknown",              Nz::ResolveError_Unknown);
		}
		instance.SetGlobal("ResolveError");

		// Nz::SocketError
		static_assert(Nz::SocketError_Max + 1 == 15, "Nz::ResolveError has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 15);
		{
			instance.PushField("AddressNotAvailable", Nz::SocketError_AddressNotAvailable);
			instance.PushField("ConnectionClosed",    Nz::SocketError_ConnectionClosed);
			instance.PushField("ConnectionRefused",   Nz::SocketError_ConnectionRefused);
			instance.PushField("DatagramSize",        Nz::SocketError_DatagramSize);
			instance.PushField("Internal",            Nz::SocketError_Internal);
			instance.PushField("Packet",              Nz::SocketError_Packet);
			instance.PushField("NetworkError",        Nz::SocketError_NetworkError);
			instance.PushField("NoError",             Nz::SocketError_NoError);
			instance.PushField("NotInitialized",      Nz::SocketError_NotInitialized);
			instance.PushField("NotSupported",        Nz::SocketError_NotSupported);
			instance.PushField("ResolveError",        Nz::SocketError_ResolveError);
			instance.PushField("ResourceError",       Nz::SocketError_ResourceError);
			instance.PushField("TimedOut",            Nz::SocketError_TimedOut);
			instance.PushField("Unknown",             Nz::SocketError_Unknown);
			instance.PushField("UnreachableHost",     Nz::SocketError_UnreachableHost);
		}
		instance.SetGlobal("SocketError");

		// Nz::SocketState
		static_assert(Nz::SocketState_Max + 1 == 5, "Nz::SocketState has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 5);
		{
			instance.PushField("Bound",        Nz::SocketState_Bound);
			instance.PushField("Connecting",   Nz::SocketState_Connecting);
			instance.PushField("Connected",    Nz::SocketState_Connected);
			instance.PushField("NotConnected", Nz::SocketState_NotConnected);
			instance.PushField("Resolving",    Nz::SocketState_Resolving);
		}
		instance.SetGlobal("SocketState");

		// Nz::SocketType
		static_assert(Nz::SocketType_Max + 1 == 4, "Nz::SocketState has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 4);
		{
			instance.PushField("Raw",     Nz::SocketType_Raw);
			instance.PushField("TCP",     Nz::SocketType_TCP);
			instance.PushField("UDP",     Nz::SocketType_UDP);
			instance.PushField("Unknown", Nz::SocketType_Unknown);
		}
		instance.SetGlobal("SocketType");
	}
}
