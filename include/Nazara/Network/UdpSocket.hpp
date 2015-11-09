// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UDPSOCKET_HPP
#define NAZARA_UDPSOCKET_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Network/AbstractSocket.hpp>
#include <Nazara/Network/IpAddress.hpp>

namespace Nz
{
	class NAZARA_NETWORK_API UdpSocket : public AbstractSocket
	{
		public:
			inline UdpSocket();
			inline UdpSocket(NetProtocol protocol);
			inline UdpSocket(UdpSocket&& udpSocket);
			~UdpSocket() = default;

			inline SocketState Bind(UInt16 port);
			SocketState Bind(const IpAddress& address);

			inline bool Create(NetProtocol protocol);

			inline IpAddress GetBoundAddress() const;
			inline UInt16 GetBoundPort() const;
			inline SocketState GetState() const;

			unsigned int QueryMaxDatagramSize();

			bool Receive(void* buffer, std::size_t size, IpAddress* from, std::size_t* received);

			bool Send(const IpAddress& to, const void* buffer, std::size_t size, std::size_t* sent);

		private:
			void OnClose() override;
			void OnOpened() override;

			IpAddress m_boundAddress;
			SocketState m_state;
	};
}

#include <Nazara/Network/UdpSocket.inl>

#endif // NAZARA_UDPSOCKET_HPP