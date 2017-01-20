// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TCPSERVER_HPP
#define NAZARA_TCPSERVER_HPP

#include <Nazara/Prerequesites.hpp>
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

#endif // NAZARA_TCPSERVER_HPP