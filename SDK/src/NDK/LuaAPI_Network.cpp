// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/LuaAPI.hpp>
#include <Nazara/Network.hpp>
#include <Nazara/Lua/LuaClass.hpp>

namespace Ndk
{
	void LuaAPI::Register_Network(Nz::LuaInstance& instance)
	{
		/*********************************** Nz::AbstractSocket **********************************/
		Nz::LuaClass<Nz::AbstractSocket> abstractSocketClass("AbstractSocket");

		abstractSocketClass.SetMethod("Close",               &Nz::AbstractSocket::Close);
		abstractSocketClass.SetMethod("EnableBlocking",      &Nz::AbstractSocket::EnableBlocking);
		abstractSocketClass.SetMethod("GetLastError",        &Nz::AbstractSocket::GetLastError);
		abstractSocketClass.SetMethod("GetState",            &Nz::AbstractSocket::GetState);
		abstractSocketClass.SetMethod("GetType",             &Nz::AbstractSocket::GetType);
		abstractSocketClass.SetMethod("IsBlockingEnabled",   &Nz::AbstractSocket::IsBlockingEnabled);
		abstractSocketClass.SetMethod("QueryAvailableBytes", &Nz::AbstractSocket::QueryAvailableBytes);

		abstractSocketClass.Register(instance);

		Nz::LuaClass<Nz::IpAddress> ipAddressClass("IpAddress");

		ipAddressClass.SetConstructor([] (Nz::LuaInstance& lua) -> Nz::IpAddress*
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 9U);

			int argIndex = 1;
			switch (argCount)
			{
				case 0:
					return new Nz::IpAddress;

				case 1:
					return new Nz::IpAddress(lua.CheckString(argIndex));

				case 4:
				case 5:
					return new Nz::IpAddress(lua.Check<Nz::UInt8>(&argIndex), lua.Check<Nz::UInt8>(&argIndex), lua.Check<Nz::UInt8>(&argIndex), lua.Check<Nz::UInt8>(&argIndex), lua.Check<Nz::UInt16>(&argIndex, 0));

				case 8:
				case 9:
					return new Nz::IpAddress(lua.Check<Nz::UInt16>(&argIndex), lua.Check<Nz::UInt16>(&argIndex), lua.Check<Nz::UInt16>(&argIndex), lua.Check<Nz::UInt16>(&argIndex),
					                         lua.Check<Nz::UInt16>(&argIndex), lua.Check<Nz::UInt16>(&argIndex), lua.Check<Nz::UInt16>(&argIndex), lua.Check<Nz::UInt16>(&argIndex), lua.Check<Nz::UInt16>(&argIndex, 0));
			}

			return nullptr;
		});

		ipAddressClass.SetMethod("GetPort",     &Nz::IpAddress::GetPort);
		ipAddressClass.SetMethod("GetProtocol", &Nz::IpAddress::GetProtocol);
		ipAddressClass.SetMethod("IsLoopback",  &Nz::IpAddress::IsLoopback);
		ipAddressClass.SetMethod("IsValid",     &Nz::IpAddress::IsValid);
		ipAddressClass.SetMethod("ToUInt32",    &Nz::IpAddress::ToUInt32);
		ipAddressClass.SetMethod("__tostring",  &Nz::IpAddress::ToString);

		ipAddressClass.SetStaticMethod("ResolveAddress", [] (Nz::LuaInstance& instance) -> int
		{
			Nz::String service;
			Nz::ResolveError error = Nz::ResolveError_Unknown;

			int argIndex = 1;
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

		ipAddressClass.SetStaticMethod("ResolveHostname", [] (Nz::LuaInstance& instance) -> int
		{
			Nz::ResolveError error = Nz::ResolveError_Unknown;

			int argIndex = 1;
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
					instance.SetField("Address",       std::move(info.address));
					instance.SetField("CanonicalName", std::move(info.canonicalName));
					instance.SetField("Protocol",      std::move(info.protocol));
					instance.SetField("SocketType",    std::move(info.socketType));
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

		ipAddressClass.Register(instance);

		// Enums

		// Nz::NetProtocol
		static_assert(Nz::NetProtocol_Max + 1 == 4, "Nz::NetProtocol has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 4);
		{
			instance.SetField("Any",     Nz::NetProtocol_Any);
			instance.SetField("IPv4",    Nz::NetProtocol_IPv4);
			instance.SetField("IPv6",    Nz::NetProtocol_IPv6);
			instance.SetField("Unknown", Nz::NetProtocol_Unknown);
		}
		instance.SetGlobal("NetProtocol");

		// Nz::PacketPriority
		static_assert(Nz::PacketPriority_Max + 1 == 4, "Nz::PacketPriority has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 6);
		{
			instance.SetField("High",      Nz::PacketPriority_High);
			instance.SetField("Highest",   Nz::PacketPriority_Highest);
			instance.SetField("Immediate", Nz::PacketPriority_Immediate);
			instance.SetField("Medium",    Nz::PacketPriority_Medium);
			instance.SetField("Low",       Nz::PacketPriority_Low);
			instance.SetField("Lowest",    Nz::PacketPriority_Lowest);
		}
		instance.SetGlobal("PacketPriority");

		// Nz::PacketReliability
		static_assert(Nz::PacketReliability_Max + 1 == 3, "Nz::PacketReliability has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 3);
		{
			instance.SetField("Reliable",        Nz::PacketReliability_Reliable);
			instance.SetField("ReliableOrdered", Nz::PacketReliability_ReliableOrdered);
			instance.SetField("Unreliable",      Nz::PacketReliability_Unreliable);
		}
		instance.SetGlobal("PacketReliability");

		// Nz::ResolveError
		static_assert(Nz::ResolveError_Max + 1 == 9, "Nz::ResolveError has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 9);
		{
			instance.SetField("Internal",             Nz::ResolveError_Internal);
			instance.SetField("ResourceError",        Nz::ResolveError_ResourceError);
			instance.SetField("NoError",              Nz::ResolveError_NoError);
			instance.SetField("NonRecoverable",       Nz::ResolveError_NonRecoverable);
			instance.SetField("NotFound",             Nz::ResolveError_NotFound);
			instance.SetField("NotInitialized",       Nz::ResolveError_NotInitialized);
			instance.SetField("ProtocolNotSupported", Nz::ResolveError_ProtocolNotSupported);
			instance.SetField("TemporaryFailure",     Nz::ResolveError_TemporaryFailure);
			instance.SetField("Unknown",              Nz::ResolveError_Unknown);
		}
		instance.SetGlobal("ResolveError");
		
		// Nz::SocketError
		static_assert(Nz::SocketError_Max + 1 == 15, "Nz::ResolveError has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 15);
		{
			instance.SetField("AddressNotAvailable", Nz::SocketError_AddressNotAvailable);
			instance.SetField("ConnectionClosed",    Nz::SocketError_ConnectionClosed);
			instance.SetField("ConnectionRefused",   Nz::SocketError_ConnectionRefused);
			instance.SetField("DatagramSize",        Nz::SocketError_DatagramSize);
			instance.SetField("Internal",            Nz::SocketError_Internal);
			instance.SetField("Packet",              Nz::SocketError_Packet);
			instance.SetField("NetworkError",        Nz::SocketError_NetworkError);
			instance.SetField("NoError",             Nz::SocketError_NoError);
			instance.SetField("NotInitialized",      Nz::SocketError_NotInitialized);
			instance.SetField("NotSupported",        Nz::SocketError_NotSupported);
			instance.SetField("ResolveError",        Nz::SocketError_ResolveError);
			instance.SetField("ResourceError",       Nz::SocketError_ResourceError);
			instance.SetField("TimedOut",            Nz::SocketError_TimedOut);
			instance.SetField("Unknown",             Nz::SocketError_Unknown);
			instance.SetField("UnreachableHost",     Nz::SocketError_UnreachableHost);
		}
		instance.SetGlobal("SocketError");

		// Nz::SocketState
		static_assert(Nz::SocketState_Max + 1 == 5, "Nz::SocketState has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 5);
		{
			instance.SetField("Bound",        Nz::SocketState_Bound);
			instance.SetField("Connecting",   Nz::SocketState_Connecting);
			instance.SetField("Connected",    Nz::SocketState_Connected);
			instance.SetField("NotConnected", Nz::SocketState_NotConnected);
			instance.SetField("Resolving",    Nz::SocketState_Resolving);
		}
		instance.SetGlobal("SocketState");

		// Nz::SocketType
		static_assert(Nz::SocketType_Max + 1 == 4, "Nz::SocketState has been updated but change was not reflected to Lua binding");
		instance.PushTable(0, 4);
		{
			instance.SetField("Raw",     Nz::SocketType_Raw);
			instance.SetField("TCP",     Nz::SocketType_TCP);
			instance.SetField("UDP",     Nz::SocketType_UDP);
			instance.SetField("Unknown", Nz::SocketType_Unknown);
		}
		instance.SetGlobal("SocketType");
	}
}
