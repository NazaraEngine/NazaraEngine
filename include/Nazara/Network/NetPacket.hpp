// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NETPACKET_HPP
#define NAZARA_NETPACKET_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ByteStream.hpp>
#include <Nazara/Core/MemoryStream.hpp>
#include <Nazara/Network/Config.hpp>
#include <mutex>

namespace Nz
{
	class NAZARA_NETWORK_API NetPacket : public ByteStream
	{
		friend class Network;

		public:
			inline NetPacket();
			inline NetPacket(UInt16 netCode, std::size_t minCapacity = 0);
			inline NetPacket(UInt16 netCode, const void* ptr, std::size_t size);
			NetPacket(const NetPacket&) = delete;
			NetPacket(NetPacket&& packet);
			inline ~NetPacket();

			inline const UInt8* GetConstData() const;
			inline UInt8* GetData() const;
			inline size_t GetDataSize() const;
			inline UInt16 GetNetCode() const;

			virtual void OnReceive(UInt16 netCode, const void* data, std::size_t size);
			virtual const void* OnSend(std::size_t* newSize) const;

			inline void Reset();
			inline void Reset(UInt16 netCode, std::size_t minCapacity = 0);
			inline void Reset(UInt16 netCode, const void* ptr, std::size_t size);

			inline void Resize(std::size_t newSize);

			inline void SetNetCode(UInt16 netCode);

			NetPacket& operator=(const NetPacket&) = delete;
			NetPacket& operator=(NetPacket&& packet);

			static bool DecodeHeader(const void* data, UInt32* packetSize, UInt16* netCode);
			static bool EncodeHeader(void* data, UInt32 packetSize, UInt16 netCode);

			static constexpr std::size_t HeaderSize = sizeof(UInt32) + sizeof(UInt16); //< PacketSize + NetCode

		private:
			void OnEmptyStream() override;

			void FreeStream();
			void InitStream(std::size_t minCapacity, UInt64 cursorPos, OpenModeFlags openMode);

			static bool Initialize();
			static void Uninitialize();

			std::unique_ptr<ByteArray> m_buffer;
			MemoryStream m_memoryStream;
			UInt16 m_netCode;

			static std::mutex s_availableBuffersMutex;
			static std::vector<std::pair<std::size_t, std::unique_ptr<ByteArray>>> s_availableBuffers;
	};
}

#include <Nazara/Network/NetPacket.inl>

#endif // NAZARA_NETPACKET_HPP
