// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/RUdpConnection.hpp>
#include <utility>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Closes the connection
	*/

	inline void RUdpConnection::Close()
	{
		m_socket.Close();
	}

	/*!
	* \brief Disconnects the connection
	*
	* \see Close
	*/

	inline void RUdpConnection::Disconnect()
	{
		Close();
	}

	/*!
	* \brief Gets the bound address
	* \return IpAddress we are linked to
	*/

	inline IpAddress RUdpConnection::GetBoundAddress() const
	{
		return m_socket.GetBoundAddress();
	}

	/*!
	* \brief Gets the port of the bound address
	* \return Port we are linked to
	*/

	inline UInt16 RUdpConnection::GetBoundPort() const
	{
		return m_socket.GetBoundPort();
	}

	/*!
	* \brief Gets the last error
	* \return Socket error
	*/

	inline SocketError RUdpConnection::GetLastError() const
	{
		return m_lastError;
	}

	/*!
	* \brief Listens to a socket
	* \return true If successfully bound
	*
	* \param protocol Net protocol to listen to
	* \param port Port to listen to
	*
	* \remark Produces a NazaraAssert if protocol is unknown or any
	*/

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

	/*!
	* \brief Sets the protocol id
	*
	* \param protocolId Protocol ID like NNet
	*/

	inline void RUdpConnection::SetProtocolId(UInt32 protocolId)
	{
		m_protocol = protocolId;
	}

	/*!
	* \brief Sets the time before ack
	*
	* \param Time before acking to send many together (in ms)
	*/

	inline void RUdpConnection::SetTimeBeforeAck(UInt32 ms)
	{
		m_forceAckSendTime = ms * 1000; //< Store in microseconds for easier handling
	}

	/*!
	* \brief Computes the difference of sequence
	* \return Delta between the two sequences
	*
	* \param sequence First sequence
	* \param sequence2 Second sequence
	*/

	inline unsigned int RUdpConnection::ComputeSequenceDifference(SequenceIndex sequence, SequenceIndex sequence2)
	{
		unsigned int difference;
		if (sequence2 > sequence)
			difference = std::numeric_limits<SequenceIndex>::max() - sequence2 + sequence;
		else
			difference = sequence - sequence2;

		return difference;
	}

	/*!
	* \brief Checks whether the peer has pending packets
	* \return true If it is the case
	*
	* \param peer Data relative to the peer
	*/

	inline bool RUdpConnection::HasPendingPackets(PeerData& peer)
	{
		for (unsigned int priority = PacketPriority_Highest; priority <= PacketPriority_Lowest; ++priority)
		{
			std::vector<PendingPacket>& pendingPackets = peer.pendingPackets[priority];
			if (!pendingPackets.empty())
				return true;
		}

		return false;
	}

	/*!
	* \brief Checks whether the ack is more recent
	* \return true If it is the case
	*
	* \param ack First sequence
	* \param ack2 Second sequence
	*/

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

	/*!
	* \brief Checks whether the connection is reliable
	* \return true If it is the case
	*
	* \remark Produces a NazaraError if enumeration is invalid
	*/

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

	/*!
	* \brief Simulates the loss of packets on network
	*
	* \param packetLoss Ratio of packet loss according to bernoulli distribution
	*
	* \remark Produces a NazaraAssert if packetLoss is not in between 0.0 and 1.0
	*/

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
