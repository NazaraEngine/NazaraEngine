// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/ENetPacket.hpp>
#include <Nazara/Core/MemoryPool.hpp>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	/// Temporary
	void ENetPacketRef::Reset(ENetPacket* packet)
	{
		if (m_packet)
		{
			if (--m_packet->referenceCount == 0)
			{
				assert(m_pool);
				m_pool->Free(m_packet->poolIndex);
			}
		}

		m_packet = packet;
		if (m_packet)
			m_packet->referenceCount++;
	}
}
