// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_NETWORK_TCPSERVER_HPP
#define NAZARA_NETWORK_TCPSERVER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Network/AbstractSocket.hpp>
#include <Nazara/Network/IpAddress.hpp>

namespace Nz
{
	class TcpClient;

	class NAZARA_NETWORK_API TcpServer : public AbstractSocket
	{
		public:
			inline TcpServer();
			inline TcpServer(TcpServer&& tcpServer);
			~TcpServer() = default;

			bool AcceptClient(TcpClient* newClient);

			inline IpAddress GetBoundAddress() const;
			inline UInt16 GetBoundPort() const;

			inline SocketState Listen(NetProtocol protocol, UInt16 port, unsigned int queueSize = 10);
			SocketState Listen(const IpAddress& address, unsigned int queueSize = 10);

		private:
			void OnClose() override;
			void OnOpened() override;

			IpAddress m_boundAddress;
	};
}

#include <Nazara/Network/TcpServer.inl>

#endif // NAZARA_NETWORK_TCPSERVER_HPP
