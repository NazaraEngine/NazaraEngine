// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TCPCLIENT_HPP
#define NAZARA_TCPCLIENT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Network/AbstractSocket.hpp>
#include <Nazara/Network/IpAddress.hpp>

namespace Nz
{
	class NetPacket;

	class NAZARA_NETWORK_API TcpClient : public AbstractSocket, public Stream
	{
		friend class TcpServer;

		public:
			inline TcpClient();
			TcpClient(TcpClient&& tcpClient) = default;
			~TcpClient() = default;

			SocketState Connect(const IpAddress& remoteAddress);
			SocketState Connect(const String& hostName, NetProtocol protocol = NetProtocol_Any, const String& service = "http", ResolveError* error = nullptr);
			inline void Disconnect();

			void EnableLowDelay(bool lowDelay);
			void EnableKeepAlive(bool keepAlive, UInt64 msTime = 10000, UInt64 msInterval = 1000);

			bool EndOfStream() const override;

			UInt64 GetCursorPos() const override;
			inline UInt64 GetKeepAliveInterval() const;
			inline UInt64 GetKeepAliveTime() const;
			inline IpAddress GetRemoteAddress() const;
			UInt64 GetSize() const override;

			inline bool IsLowDelayEnabled() const;
			inline bool IsKeepAliveEnabled() const;

			bool Receive(void* buffer, std::size_t size, std::size_t* received);
			bool ReceivePacket(NetPacket* packet);

			bool Send(const void* buffer, std::size_t size, std::size_t* sent);
			bool SendPacket(const NetPacket& packet);

			bool SetCursorPos(UInt64 offset) override;

			bool WaitForConnected(UInt64 msTimeout = 3000);

			inline TcpClient& operator=(TcpClient&& tcpClient) = default;

		private:
			void FlushStream() override;

			void OnClose() override;
			void OnOpened() override;

			std::size_t ReadBlock(void* buffer, std::size_t size) override;
			void Reset(SocketHandle handle, const IpAddress& peerAddress);
			std::size_t WriteBlock(const void* buffer, std::size_t size) override;

			struct PendingPacket
			{
				std::size_t received = 0;
				ByteArray data;
				UInt16 netcode;
				bool headerReceived = false;
			};

			IpAddress m_peerAddress;
			PendingPacket m_pendingPacket;
			UInt64 m_keepAliveInterval;
			UInt64 m_keepAliveTime;
			bool m_isKeepAliveEnabled;
			bool m_isLowDelayEnabled;
	};
}

#include <Nazara/Network/TcpClient.inl>

#endif // NAZARA_TCPCLIENT_HPP
