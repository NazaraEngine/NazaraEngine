// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/RUdpConnection.hpp>
#include <utility>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline void RUdpConnection::Close()
	{
		m_socket.Close();
	}

	inline void RUdpConnection::Disconnect()
	{
		Close();
	}

	inline IpAddress RUdpConnection::GetBoundAddress() const
	{
		return m_socket.GetBoundAddress();
	}

	inline UInt16 RUdpConnection::GetBoundPort() const
	{
		return m_socket.GetBoundPort();
	}

	inline SocketError RUdpConnection::GetLastError() const
	{
		return m_lastError;
	}

	inline bool RUdpConnection::Listen(NetProtocol protocol, UInt16 port)
	{
		NazaraAssert(protocol != NetProtocol_Any, "Any protocol not supported for Listen"); //< TODO
		NazaraAssert(protocol != NetProtocol_Unknown, "Invalid protocol"); 

		IpAddress any;
		switch (protocol)
		{
			case NetProtocol_Any:
			case NetProtocol_Unknown:
				NazaraInternalError("Invalid protocol Any at this point");
				return false;

			case NetProtocol_IPv4:
				any = IpAddress::AnyIpV4;
				break;

			case NetProtocol_IPv6:
				any = IpAddress::AnyIpV6;
				break;
		}

		any.SetPort(port);
		return Listen(any);
	}

	inline void RUdpConnection::SetProtocolId(UInt32 protocolId)
	{
		m_protocol = protocolId;
	}

	inline void RUdpConnection::SetTimeBeforeAck(UInt32 ms)
	{
		m_forceAckSendTime = ms * 1000; //< Store in microseconds for easier handling
	}
	
	inline unsigned int RUdpConnection::ComputeSequenceDifference(SequenceIndex sequence, SequenceIndex sequence2)
	{
		unsigned int difference;
		if (sequence2 > sequence)
			difference = std::numeric_limits<SequenceIndex>::max() - sequence2 + sequence;
		else
			difference = sequence - sequence2;

		return 0;
	}

	inline bool RUdpConnection::HasPendingPackets(PeerData& peer)
	{
		for (unsigned int priority = PacketPriority_Highest; priority <= PacketPriority_Lowest; ++priority)
		{
			std::vector<PendingPacket>& pendingPackets = peer.pendingPackets[priority];
			if (!pendingPackets.empty())
				return true;

			pendingPackets.clear();
		}

		return false;
	}

	inline bool RUdpConnection::IsAckMoreRecent(SequenceIndex ack, SequenceIndex ack2)
	{
		constexpr SequenceIndex maxDifference = std::numeric_limits<SequenceIndex>::max() / 2;

		if (ack > ack2)
			return ack - ack2 <= maxDifference;
		else if (ack2 > ack)
			return ack2 - ack > maxDifference;
		else
			return false; ///< Same ack
	}

	inline bool RUdpConnection::IsReliable(PacketReliability reliability)
	{
		switch (reliability)
		{
			case PacketReliability_Reliable:
			case PacketReliability_ReliableOrdered:
				return true;

			case PacketReliability_Unreliable:
				return false;
		}

		NazaraError("PacketReliability not handled (0x" + String::Number(reliability, 16) + ')');
		return false;
	}

	inline void RUdpConnection::SimulateNetwork(double packetLoss)
	{
		NazaraAssert(packetLoss >= 0.0 && packetLoss <= 1.0, "Packet loss must be in range [0..1]");

		if (packetLoss > 0.0)
		{
			m_isSimulationEnabled = true;
			m_packetLossProbability = std::bernoulli_distribution(packetLoss);
		}
		else
			m_isSimulationEnabled = false;
	}
}

#include <Nazara/Network/DebugOff.hpp>
