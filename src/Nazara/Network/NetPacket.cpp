// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/NetPacket.hpp>
#include <Nazara/Core/LockGuard.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	void NetPacket::OnReceive(UInt16 netCode, const void* data, std::size_t size)
	{
		Reset(netCode, data, size);
	}

	const void* NetPacket::OnSend(std::size_t* newSize) const
	{
		NazaraAssert(newSize, "Invalid size pointer");
		NazaraAssert(m_netCode != NetCode_Invalid, "Invalid NetCode");

		std::size_t size = m_buffer->GetSize();
		if (!EncodeHeader(m_buffer->GetBuffer(), static_cast<UInt16>(size), m_netCode))
		{
			NazaraError("Failed to encode packet header");
			return nullptr;
		}

		*newSize = size;
		return m_buffer->GetBuffer();
	}

	bool NetPacket::DecodeHeader(const void* data, UInt16* packetSize, UInt16* netCode)
	{
		MemoryView stream(data, HeaderSize);

		SerializationContext context;
		context.stream = &stream;

		return Unserialize(context, packetSize) && Unserialize(context, netCode);
	}

	bool NetPacket::EncodeHeader(void* data, UInt16 packetSize, UInt16 netCode)
	{
		MemoryView stream(data, HeaderSize);

		SerializationContext context;
		context.stream = &stream;

		return Serialize(context, packetSize) && Serialize(context, netCode);
	}

	void NetPacket::OnEmptyStream()
	{
		Reset(0);
	}

	void NetPacket::FreeStream()
	{
		if (!m_buffer)
			return;

		std::size_t size = m_buffer->GetSize();

		Nz::LockGuard lock(*s_availableBuffersMutex);
		s_availableBuffers.emplace_back(std::make_pair(size, std::move(m_buffer)));
	}

	void NetPacket::InitStream(std::size_t minCapacity, UInt64 cursorPos, UInt32 openMode)
	{
		NazaraAssert(minCapacity >= cursorPos, "Cannot init stream with a smaller capacity than wanted cursor pos");

		{
			Nz::LockGuard lock(*s_availableBuffersMutex);

			FreeStream(); //< In case it wasn't released yet

			if (!s_availableBuffers.empty())
			{
				m_buffer = std::move(s_availableBuffers.back().second);
				s_availableBuffers.pop_back();
			}
		}

		if (!m_buffer)
			m_buffer = std::make_unique<ByteArray>();

		m_buffer->Resize(static_cast<std::size_t>(cursorPos));

		m_memoryStream.SetBuffer(m_buffer.get(), openMode);
		m_memoryStream.SetCursorPos(cursorPos);
		SetStream(&m_memoryStream);
	}

	bool NetPacket::Initialize()
	{
		s_availableBuffersMutex = std::make_unique<Mutex>();
		return true;
	}

	void NetPacket::Uninitialize()
	{
		s_availableBuffers.clear();
		s_availableBuffersMutex.reset();
	}

	std::unique_ptr<Mutex> NetPacket::s_availableBuffersMutex;
	std::vector<std::pair<std::size_t, std::unique_ptr<ByteArray>>> NetPacket::s_availableBuffers;
}
