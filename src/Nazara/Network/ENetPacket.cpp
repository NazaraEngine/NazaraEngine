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
				m_packet->owner->Delete(m_packet);
				return;
			}
		}

		m_packet = packet;
		if (m_packet)
			m_packet->referenceCount++;
	}
}
