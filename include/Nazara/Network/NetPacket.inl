// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>
#include <Nazara/Network/Enums.hpp>
#include <cstring>

namespace Nz
{
	inline NetPacket::NetPacket() :
	m_netCode(NetCode_Invalid)
	{
	}

	inline NetPacket::NetPacket(UInt16 netCode, std::size_t sizeHint)
	{
		Reset(netCode, sizeHint);
	}

	inline NetPacket::NetPacket(UInt16 netCode, const void* ptr, std::size_t size)
	{
		Reset(netCode, ptr, size);
	}

	inline NetPacket::~NetPacket()
	{
		FreeStream();
	}

	inline UInt16 NetPacket::GetNetCode() const
	{
		return m_netCode;
	}

	inline void NetPacket::Reset()
	{
		FreeStream();
	}

	inline void NetPacket::Reset(UInt16 netCode, std::size_t sizeHint)
	{
		InitStream(HeaderSize + sizeHint, HeaderSize, OpenMode_WriteOnly);
		m_netCode = netCode;
	}

	inline void NetPacket::Reset(UInt16 netCode, const void* ptr, std::size_t size)
	{
		InitStream(HeaderSize + size, HeaderSize, OpenMode_ReadOnly);
		m_buffer->Resize(HeaderSize + size);
		std::memcpy(m_buffer->GetBuffer() + HeaderSize, ptr, size);

		m_netCode = netCode;
	}

	inline void NetPacket::SetNetCode(UInt16 netCode)
	{
		m_netCode = netCode;
	}
}

#include <Nazara/Core/DebugOff.hpp>
