// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TCPCLIENT_HPP
#define NAZARA_TCPCLIENT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Network/TcpBase.hpp>
#include <Nazara/Network/IpAddress.hpp>

namespace Nz
{
	class NAZARA_NETWORK_API TcpClient : public TcpBase
	{
		friend class TcpServer;

		public:
			TcpClient() = default;
			inline TcpClient(TcpClient&& tcpClient);
			~TcpClient() = default;

			SocketState Connect(const IpAddress& remoteAddress, UInt64 msTimeout = 3000);
			inline void Disconnect();

			void EnableLowDelay(bool lowDelay);
			void EnableKeepAlive(bool keepAlive, UInt64 msTime = 10000, UInt64 msInterval = 1000);

			inline UInt64 GetKeepAliveInterval() const;
			inline UInt64 GetKeepAliveTime() const;
			inline IpAddress GetRemoteAddress() const;

			SocketState QueryState();

			bool Receive(void* buffer, std::size_t size, std::size_t* received);

			bool Send(const void* buffer, std::size_t size, std::size_t* sent);

		private:
			void OnClose() override;
			void OnOpened() override;

			void Reset(SocketHandle handle, const IpAddress& peerAddress);

			IpAddress m_peerAddress;
	};
}

#include <Nazara/Network/TcpClient.inl>

#endif // NAZARA_TCPCLIENT_HPP