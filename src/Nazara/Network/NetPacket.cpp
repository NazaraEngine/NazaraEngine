// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/NetPacket.hpp>
#include <Nazara/Core/LockGuard.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup network
	* \class Nz::NetPacket
	* \brief Network class that represents a packet
	*/

	/*!
	* \brief Operation to do when receiving data
	*
	* \param netCode Packet number
	* \param data Raw memory
	* \param size Size of the memory
	*/

	void NetPacket::OnReceive(UInt16 netCode, const void* data, std::size_t size)
	{
		Reset(netCode, data, size);
	}

	/*!
	* \brief Operation to do when sending data
	* \return Beggining of the raw memory
	*
	* \param newSize Size of the memory to send
	*
	* \remark Produces a NazaraAssert if newSize is invalid
	* \remark Produces a NazaraAssert if net code is invalid
	* \remark Produces a NazaraError if header could not be encoded
	*/

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

	/*!
	* \brief Decodes the header of the packet
	* \return true If successful
	*
	* \param data Raw memory
	* \param packetSize Size of the packet
	* \param netCode Packet number
	*/

	bool NetPacket::DecodeHeader(const void* data, UInt16* packetSize, UInt16* netCode)
	{
		MemoryView stream(data, HeaderSize);

		SerializationContext context;
		context.stream = &stream;

		return Unserialize(context, packetSize) && Unserialize(context, netCode);
	}

	/*!
	* \brief Encodes the header of the packet
	* \return true If successful
	*
	* \param data Raw memory
	* \param packetSize Size of the packet
	* \param netCode Packet number
	*/

	bool NetPacket::EncodeHeader(void* data, UInt16 packetSize, UInt16 netCode)
	{
		MemoryView stream(data, HeaderSize);

		SerializationContext context;
		context.stream = &stream;

		return Serialize(context, packetSize) && Serialize(context, netCode);
	}

	/*!
	* \brief Operation to do when stream is empty
	*/

	void NetPacket::OnEmptyStream()
	{
		Reset(0);
	}

	/*!
	* \brief Frees the stream
	*/

	void NetPacket::FreeStream()
	{
		if (!m_buffer)
			return;

		std::size_t size = m_buffer->GetSize();

		Nz::LockGuard lock(*s_availableBuffersMutex);
		s_availableBuffers.emplace_back(std::make_pair(size, std::move(m_buffer)));
	}

	/*!
	* \brief Inits the internal stream
	*
	* \param minCapacity Minimal capacity of the stream
	* \param cursorPos Position of the cursor in the stream
	* \param openMode Flag of the stream
	*
	* \remark Produces a NazaraAssert if cursor position is greather than the capacity
	*/

	void NetPacket::InitStream(std::size_t minCapacity, UInt64 cursorPos, OpenModeFlags openMode)
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

		m_buffer->Resize(minCapacity);

		m_memoryStream.SetBuffer(m_buffer.get(), openMode);
		m_memoryStream.SetCursorPos(cursorPos);
		SetStream(&m_memoryStream);
	}

	/*!
	* \brief Initializes the NetPacket class
	* \return true If initialization is successful
	*/

	bool NetPacket::Initialize()
	{
		s_availableBuffersMutex = std::make_unique<Mutex>();
		return true;
	}

	/*!
	* \brief Uninitializes the NetPacket class
	*/

	void NetPacket::Uninitialize()
	{
		s_availableBuffers.clear();
		s_availableBuffersMutex.reset();
	}

	std::unique_ptr<Mutex> NetPacket::s_availableBuffersMutex;
	std::vector<std::pair<std::size_t, std::unique_ptr<ByteArray>>> NetPacket::s_availableBuffers;
}
