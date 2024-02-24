// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_NETWORK_UDPSOCKET_HPP
#define NAZARA_NETWORK_UDPSOCKET_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Network/AbstractSocket.hpp>
#include <Nazara/Network/IpAddress.hpp>

namespace Nz
{
	struct NetBuffer;

	class NAZARA_NETWORK_API UdpSocket : public AbstractSocket
	{
		public:
			inline UdpSocket();
			inline UdpSocket(NetProtocol protocol);
			inline UdpSocket(UdpSocket&& udpSocket) noexcept;
			~UdpSocket() = default;

			inline SocketState Bind(UInt16 port);
			SocketState Bind(const IpAddress& address);

			inline bool Create(NetProtocol protocol);

			void EnableBroadcasting(bool broadcasting);

			inline IpAddress GetBoundAddress() const;
			inline UInt16 GetBoundPort() const;

			inline bool IsBroadcastingEnabled() const;

			std::size_t QueryMaxDatagramSize();

			bool Receive(void* buffer, std::size_t size, IpAddress* from, std::size_t* received);
			bool ReceiveMultiple(NetBuffer* buffers, std::size_t bufferCount, IpAddress* from, std::size_t* received);

			bool Send(const IpAddress& to, const void* buffer, std::size_t size, std::size_t* sent);
			bool SendMultiple(const IpAddress& to, const NetBuffer* buffers, std::size_t bufferCount, std::size_t* sent);

			UdpSocket& operator=(const UdpSocket& udpSocket) = delete;
			UdpSocket& operator=(UdpSocket && udpSocket) noexcept = default;

		private:
			void OnClose() override;
			void OnOpened() override;

			IpAddress m_boundAddress;
			bool m_isBroadCastingEnabled;
	};
}

#include <Nazara/Network/UdpSocket.inl>

#endif // NAZARA_NETWORK_UDPSOCKET_HPP
