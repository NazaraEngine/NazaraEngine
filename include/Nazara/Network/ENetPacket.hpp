// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_NETWORK_ENETPACKET_HPP
#define NAZARA_NETWORK_ENETPACKET_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Network/NetPacket.hpp>
#include <NazaraUtils/MemoryPool.hpp>
#include <NazaraUtils/MovablePtr.hpp>

namespace Nz
{
	enum class ENetPacketFlag
	{
		Reliable,
		Unsequenced,
		UnreliableFragment
	};

	template<>
	struct EnumAsFlags<ENetPacketFlag>
	{
		static constexpr ENetPacketFlag max = ENetPacketFlag::UnreliableFragment;
	};

	using ENetPacketFlags = Flags<ENetPacketFlag>;

	constexpr ENetPacketFlags ENetPacketFlag_Unreliable = 0;

	struct ENetPacket
	{
		ENetPacketFlags flags;
		NetPacket data;
		std::size_t poolIndex;
		std::size_t referenceCount = 0;
	};

	struct NAZARA_NETWORK_API ENetPacketRef
	{
		ENetPacketRef() = default;

		ENetPacketRef(MemoryPool<ENetPacket>* pool, ENetPacket* packet) :
		m_pool(pool)
		{
			Reset(packet);
		}

		ENetPacketRef(const ENetPacketRef& packet) :
		ENetPacketRef()
		{
			Reset(packet);
			m_pool = packet.m_pool;
		}

		ENetPacketRef(ENetPacketRef&&) noexcept = default;

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

		ENetPacketRef& operator=(const ENetPacketRef& packet)
		{
			Reset(packet);
			m_pool = packet.m_pool;
			return *this;
		}

		ENetPacketRef& operator=(ENetPacketRef&&) noexcept = default;

		MovablePtr<MemoryPool<ENetPacket>> m_pool;
		MovablePtr<ENetPacket> m_packet;
	};
}

#endif // NAZARA_NETWORK_ENETPACKET_HPP
