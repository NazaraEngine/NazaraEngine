// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Network/Enums.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline NetPacket::NetPacket() :
	m_netCode(NetCode_Invalid)
	{
	}

	inline NetPacket::NetPacket(UInt16 netCode, std::size_t minCapacity)
	{
		Reset(netCode, minCapacity);
	}

	inline NetPacket::NetPacket(UInt16 netCode, const void* ptr, std::size_t size)
	{
		Reset(netCode, ptr, size);
	}

	inline NetPacket::~NetPacket()
	{
		FlushBits(); //< Needs to be done here as the stream will be freed before ByteStream calls it
		FreeStream();
	}

	inline const UInt8* NetPacket::GetConstData() const
	{
		NazaraAssert(m_buffer, "Invalid buffer");

		return m_buffer->GetConstBuffer();
	}

	inline UInt8* NetPacket::GetData() const
	{
		NazaraAssert(m_buffer, "Invalid buffer");

		return m_buffer->GetBuffer();
	}

	inline size_t NetPacket::GetDataSize() const
	{
		if (m_buffer)
			return m_buffer->GetSize() - HeaderSize;
		else
			return 0;
	}

	inline UInt16 NetPacket::GetNetCode() const
	{
		return m_netCode;
	}

	inline void NetPacket::Reset()
	{
		FreeStream();
	}

	inline void NetPacket::Reset(UInt16 netCode, std::size_t minCapacity)
	{
		InitStream(HeaderSize + minCapacity, HeaderSize, OpenMode_ReadWrite);
		m_netCode = netCode;
	}

	inline void NetPacket::Reset(UInt16 netCode, const void* ptr, std::size_t size)
	{
		InitStream(HeaderSize + size, HeaderSize, OpenMode_ReadOnly);
		m_buffer->Resize(HeaderSize + size);
		std::memcpy(m_buffer->GetBuffer() + HeaderSize, ptr, size);

		m_netCode = netCode;
	}

	inline void NetPacket::Resize(std::size_t newSize)
	{
		NazaraAssert(m_buffer, "Invalid buffer");

		m_buffer->Resize(newSize);
	}

	inline void NetPacket::SetNetCode(UInt16 netCode)
	{
		m_netCode = netCode;
	}
}

#include <Nazara/Core/DebugOff.hpp>
