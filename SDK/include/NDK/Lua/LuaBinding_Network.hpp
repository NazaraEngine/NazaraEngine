// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_LUABINDING_NETWORK_HPP
#define NDK_LUABINDING_NETWORK_HPP

#include <Nazara/Network/AbstractSocket.hpp>
#include <Nazara/Network/IpAddress.hpp>
#include <Nazara/Network/UdpSocket.hpp>
#include <NDK/Lua/LuaBinding_Base.hpp>

namespace Ndk
{
	class NDK_API LuaBinding_Network : public LuaBinding_Base
	{
		public:
			LuaBinding_Network(LuaBinding& binding);
			~LuaBinding_Network() = default;

			void Register(Nz::LuaInstance& instance) override;

			Nz::LuaClass<Nz::AbstractSocket> abstractSocket;
			Nz::LuaClass<Nz::IpAddress> ipAddress;
			Nz::LuaClass<Nz::UdpSocket> udpSocket;
	};
}

#endif // NDK_LUABINDING_NETWORK_HPP
