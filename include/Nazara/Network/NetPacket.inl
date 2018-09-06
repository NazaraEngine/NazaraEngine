// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Network/Enums.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a NetPacket object by default
	*/

	inline NetPacket::NetPacket() :
	m_netCode(NetCode_Invalid)
	{
	}

	/*!
	* \brief Constructs a NetPacket object with a packet number and a minimal capacity
	*
	* \param netCode Packet number
	* \param minCapacity Minimal capacity of the packet
	*/

	inline NetPacket::NetPacket(UInt16 netCode, std::size_t minCapacity)
	{
		Reset(netCode, minCapacity);
	}

	/*!
	* \brief Constructs a NetPacket object with a packet number and raw memory
	*
	* \param netCode Packet number
	* \param ptr Raw memory
	* \param size Size of the memory
	*/

	inline NetPacket::NetPacket(UInt16 netCode, const void* ptr, std::size_t size)
	{
		Reset(netCode, ptr, size);
	}

	/*!
	* \brief Constructs a NetPacket object with another one by move semantic
	*
	* \param packet NetPacket to move into this
	*/

	inline NetPacket::NetPacket(NetPacket&& packet) :
	ByteStream(std::move(packet)),
	m_buffer(std::move(packet.m_buffer)),
	m_memoryStream(std::move(packet.m_memoryStream)),
	m_netCode(packet.m_netCode)
	{
		///< Redirect memory stream to the moved buffer
		m_memoryStream.SetBuffer(m_buffer.get(), m_memoryStream.GetOpenMode());
		SetStream(&m_memoryStream);
	}

	/*!
	* \brief Destructs the object
	*/

	inline NetPacket::~NetPacket()
	{
		FlushBits(); //< Needs to be done here as the stream will be freed before ByteStream calls it
		FreeStream();
	}

	/*!
	* \brief Gets the raw buffer
	* \return Constant raw buffer
	*
	* \remark Produces a NazaraAssert if internal buffer is invalid
	*/

	inline const UInt8* NetPacket::GetConstData() const
	{
		NazaraAssert(m_buffer, "Invalid buffer");

		return m_buffer->GetConstBuffer();
	}

	/*!
	* \brief Gets the raw buffer
	* \return Raw buffer
	*
	* \remark Produces a NazaraAssert if internal buffer is invalid
	*/

	inline UInt8* NetPacket::GetData() const
	{
		NazaraAssert(m_buffer, "Invalid buffer");

		return m_buffer->GetBuffer();
	}

	/*!
	* \brief Gets the size of the data
	* \return Size of the data
	*/

	inline size_t NetPacket::GetDataSize() const
	{
		if (m_buffer)
			return m_buffer->GetSize() - HeaderSize;
		else
			return 0;
	}

	/*!
	* \brief Gets the packet number
	* \return Packet number
	*/

	inline UInt16 NetPacket::GetNetCode() const
	{
		return m_netCode;
	}

	/*!
	* \brief Resets the packet
	*/

	inline void NetPacket::Reset()
	{
		FreeStream();
	}

	/*!
	* \brief Resets the packet with a packet number and a minimal capacity
	*
	* \param netCode Packet number
	* \param minCapacity Minimal capacity of the packet
	*/

	inline void NetPacket::Reset(UInt16 netCode, std::size_t minCapacity)
	{
		InitStream(HeaderSize + minCapacity, HeaderSize, OpenMode_ReadWrite);
		m_netCode = netCode;
	}

	/*!
	* \brief Resets the packet with a packet number and raw memory
	*
	* \param netCode Packet number
	* \param ptr Raw memory
	* \param size Size of the memory
	*/

	inline void NetPacket::Reset(UInt16 netCode, const void* ptr, std::size_t size)
	{
		InitStream(HeaderSize + size, HeaderSize, OpenMode_ReadOnly);
		m_buffer->Resize(HeaderSize + size);

		if (ptr)
			std::memcpy(m_buffer->GetBuffer() + HeaderSize, ptr, size);

		m_netCode = netCode;
	}

	/*!
	* \brief Resizes the packet
	*
	* \param newSize Size for the resizing operation
	*
	* \remark Produces a NazaraAssert if internal buffer is invalid
	*/

	inline void NetPacket::Resize(std::size_t newSize)
	{
		NazaraAssert(m_buffer, "Invalid buffer");

		m_buffer->Resize(newSize);
	}

	/*!
	* \brief Sets the packet number
	*
	* \param netCode Packet number
	*/

	inline void NetPacket::SetNetCode(UInt16 netCode)
	{
		m_netCode = netCode;
	}

	/*!
	* \brief Moves the NetPacket into this
	* \return A reference to this
	*
	* \param packet NetPacket to move in this
	*/

	inline NetPacket& NetPacket::operator=(NetPacket&& packet)
	{
		FreeStream();

		ByteStream::operator=(std::move(packet));

		m_buffer = std::move(packet.m_buffer);
		m_memoryStream = std::move(packet.m_memoryStream);
		m_netCode = packet.m_netCode;
		
		///< Redirect memory stream to the moved buffer
		m_memoryStream.SetBuffer(m_buffer.get(), m_memoryStream.GetOpenMode());
		SetStream(&m_memoryStream);

		return *this;
	}
}

#include <Nazara/Core/DebugOff.hpp>
