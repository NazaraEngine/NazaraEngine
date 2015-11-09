// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TCPBASE_HPP
#define NAZARA_TCPBASE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Network/AbstractSocket.hpp>

namespace Nz
{
	class NAZARA_NETWORK_API TcpBase : public AbstractSocket
	{
		public:
			~TcpBase() = default;

			inline bool IsLowDelayEnabled() const;
			inline bool IsKeepAliveEnabled() const;

			// Slots
			NazaraSignal(OnStateChange, const TcpBase* /*socket*/, SocketState /*newState*/);

		protected:
			TcpBase();
			TcpBase(TcpBase&& tcpBase);
		
			virtual void OnOpened() override;

			SocketState m_state;
			UInt64 m_keepAliveInterval;
			UInt64 m_keepAliveTime;
			bool m_isLowDelayEnabled;
			bool m_isKeepAliveEnabled;
	};
}

#include <Nazara/Network/TcpBase.inl>

#endif // NAZARA_TCPBASE_HPP