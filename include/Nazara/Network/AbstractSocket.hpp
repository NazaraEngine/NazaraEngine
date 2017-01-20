// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTSOCKET_HPP
#define NAZARA_ABSTRACTSOCKET_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Network/Config.hpp>
#include <Nazara/Network/Enums.hpp>
#include <Nazara/Network/SocketHandle.hpp>

namespace Nz
{
	class NAZARA_NETWORK_API AbstractSocket
	{
		public:
			AbstractSocket(const AbstractSocket&) = delete;
			AbstractSocket(AbstractSocket&& abstractSocket);
			virtual ~AbstractSocket();

			void Close();

			void EnableBlocking(bool blocking);

			inline SocketError GetLastError() const;
			inline SocketHandle GetNativeHandle() const;
			inline SocketState GetState() const;
			inline SocketType GetType() const;

			inline bool IsBlockingEnabled() const;

			std::size_t QueryAvailableBytes() const;
			std::size_t QueryReceiveBufferSize() const;
			std::size_t QuerySendBufferSize() const;

			void SetReceiveBufferSize(std::size_t size);
			void SetSendBufferSize(std::size_t size);

			AbstractSocket& operator=(const AbstractSocket&) = delete;
			AbstractSocket& operator=(AbstractSocket&& abstractSocket);

			// Signals:
			NazaraSignal(OnStateChange, const AbstractSocket* /*socket*/, SocketState /*newState*/);

		protected:
			AbstractSocket(SocketType type);

			virtual void OnClose();
			virtual void OnOpened();

			bool Open(NetProtocol protocol);
			void Open(SocketHandle existingHandle);

			inline void UpdateState(SocketState newState);

			NetProtocol m_protocol;
			SocketError m_lastError;
			SocketHandle m_handle;
			SocketState m_state;
			SocketType m_type;
			bool m_isBlockingEnabled;
	};
}

#include <Nazara/Network/AbstractSocket.inl>

#endif // NAZARA_ABSTRACTSOCKET_HPP