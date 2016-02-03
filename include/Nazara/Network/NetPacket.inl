// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline UInt16 NetPacket::GetNetCode() const
	{
		return m_netCode;
	}

	inline void NetPacket::SetNetCode(UInt16 netCode)
	{
		m_netCode = netCode;
	}
}

#include <Nazara/Core/DebugOff.hpp>
