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
		ENetPacketFlag_Reliable,
		ENetPacketFlag_Unsequenced,
		ENetPacketFlag_UnreliableFragment
	};

	template<>
	struct EnumAsFlags<ENetPacketFlag>
	{
		static constexpr ENetPacketFlag max = ENetPacketFlag_UnreliableFragment;
	};

	using ENetPacketFlags = Flags<ENetPacketFlag>;

	constexpr ENetPacketFlags ENetPacketFlag_Unreliable = 0;

	class MemoryPool;

	struct ENetPacket
	{
		MemoryPool* owner;
		ENetPacketFlags flags;
		NetPacket data;
		std::size_t referenceCount = 0;
	};

	struct NAZARA_NETWORK_API ENetPacketRef
	{
		ENetPacketRef() = default;

		ENetPacketRef(ENetPacket* packet)
		{
			Reset(packet);
		}

		ENetPacketRef(const ENetPacketRef& packet) :
		ENetPacketRef()
		{
			Reset(packet);
		}

		ENetPacketRef(ENetPacketRef&& packet) :
		m_packet(packet.m_packet)
		{
			packet.m_packet = nullptr;
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

			return *this;
		}

		ENetPacketRef& operator=(const ENetPacketRef& packet)
		{
			Reset(packet);

			return *this;
		}

		ENetPacketRef& operator=(ENetPacketRef&& packet)
		{
			m_packet = packet.m_packet;
			packet.m_packet = nullptr;

			return *this;
		}

		ENetPacket* m_packet = nullptr;
	};
}

#endif // NAZARA_ENETPACKET_HPP
