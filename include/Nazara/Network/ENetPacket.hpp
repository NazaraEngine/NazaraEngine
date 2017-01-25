// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENETPACKET_HPP
#define NAZARA_ENETPACKET_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Network/NetPacket.hpp>

namespace Nz
{
	enum ENetPacketFlag
	{
		ENetPacketFlag_NoAllocate,
		ENetPacketFlag_Reliable,
		ENetPacketFlag_Unsequenced,
		ENetPacketFlag_UnreliableFragment,
		ENetPacketFlag_Sent
	};

	template<>
	struct EnumAsFlags<ENetPacketFlag>
	{
		static constexpr bool value = true;
		static constexpr int  max = ENetPacketFlag_Sent;
	};

	using ENetPacketFlags = Flags<ENetPacketFlag>;

	class MemoryPool;

	struct ENetPacket
	{
		MemoryPool* owner;
		ENetPacketFlags flags;
		NetPacket data;
		std::size_t referenceCount = 0;
	};

	struct ENetPacketRef
	{
		ENetPacketRef() = default;

		ENetPacketRef(ENetPacket* packet)
		{
			Reset(packet);
		}

		~ENetPacketRef()
		{
			Reset();
		}

		void Reset(ENetPacket* packet = nullptr);

		operator ENetPacket*() const
		{
			return m_packet;
		}

		ENetPacket* operator->() const
		{
			return m_packet;
		}

		ENetPacketRef& operator=(ENetPacket* packet)
		{
			Reset(packet);
		}

		ENetPacket* m_packet = nullptr;
	};
}

#endif // NAZARA_ENETPACKET_HPP
