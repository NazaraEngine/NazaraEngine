// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENETPROTOCOL_HPP
#define NAZARA_ENETPROTOCOL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Network/ENetPacket.hpp>
#include <vector>

namespace Nz
{
	constexpr UInt32 ENetTimeOverflow = 24 * 60 * 60 * 1000;

	inline UInt32 ENetTimeDifference(UInt32 a, UInt32 b);
	inline bool ENetTimeLess(UInt32 a, UInt32 b);
	inline bool ENetTimeLessEqual(UInt32 a, UInt32 b);
	inline bool ENetTimeGreater(UInt32 a, UInt32 b);
	inline bool ENetTimeGreaterEqual(UInt32 a, UInt32 b);

	class ENetPeer;

	// Constants for the ENet implementation and protocol
	enum ENetConstants
	{
		ENetHost_BandwidthThrottleInterval = 1000,
		ENetHost_DefaultMaximumPacketSize  = 32 * 1024 * 1024,
		ENetHost_DefaultMaximumWaitingData = 32 * 1024 * 1024,
		ENetHost_DefaultMTU                = 1400,
		ENetHost_ReceiveBufferSize         = 256 * 1024,
		ENetHost_SendBufferSize            = 256 * 1024,

		ENetPeer_DefaultPacketThrottle      = 32,
		ENetPeer_DefaultRoundTripTime       = 500,
		ENetPeer_FreeReliableWindows        = 8,
		ENetPeer_FreeUnsequencedWindows     = 32,
		ENetPeer_PacketLossInterval         = 10000,
		ENetPeer_PacketLossScale            = (1 << 16),
		ENetPeer_PacketThrottleAcceleration = 2,
		ENetPeer_PacketThrottleCounter      = 7,
		ENetPeer_PacketThrottleDeceleration = 2,
		ENetPeer_PacketThrottleInterval     = 5000,
		ENetPeer_PacketThrottleScale        = 32,
		ENetPeer_PingInterval               = 500,
		ENetPeer_ReliableWindows            = 16,
		ENetPeer_ReliableWindowSize         = 0x1000,
		ENetPeer_TimeoutLimit               = 32,
		ENetPeer_TimeoutMaximum             = 30000,
		ENetPeer_TimeoutMinimum             = 5000,
		ENetPeer_UnsequencedWindows         = 64,
		ENetPeer_UnsequencedWindowSize      = 1024,
		ENetPeer_WindowSizeScale            = 64 * 1024,

		ENetProtocol_MaximumChannelCount   = 255,
		ENetProtocol_MaximumFragmentCount  = 1024 * 1024,
		ENetProtocol_MaximumMTU            = 4096,
		ENetProtocol_MaximumPacketCommands = 32,
		ENetProtocol_MaximumPeerId         = 0xFFF,
		ENetProtocol_MaximumWindowSize     = 65536,
		ENetProtocol_MinimumChannelCount   = 1,
		ENetProtocol_MinimumMTU            = 576,
		ENetProtocol_MinimumWindowSize     = 4096
	};

	enum class ENetPeerState
	{
		AcknowledgingConnect    = 2,
		AcknowledgingDisconnect = 8,
		Connecting              = 1,
		ConnectionPending       = 3,
		ConnectionSucceeded     = 4,
		Connected               = 5,
		Disconnected            = 0,
		Disconnecting           = 7,
		DisconnectLater         = 6,
		Zombie                  = 9
	};

	enum ENetProtocolCommand
	{
		// Keeping the values is important for compatibility with the native ENet protocol
		ENetProtocolCommand_Acknowledge = 1,
		ENetProtocolCommand_BandwidthLimit = 10,
		ENetProtocolCommand_Connect = 2,
		ENetProtocolCommand_Disconnect = 4,
		ENetProtocolCommand_None = 0,
		ENetProtocolCommand_Ping = 5,
		ENetProtocolCommand_SendFragment = 8,
		ENetProtocolCommand_SendReliable = 6,
		ENetProtocolCommand_SendUnreliable = 7,
		ENetProtocolCommand_SendUnreliableFragment = 12,
		ENetProtocolCommand_SendUnsequenced = 9,
		ENetProtocolCommand_ThrottleConfigure = 11,
		ENetProtocolCommand_VerifyConnect = 3,
		ENetProtocolCommand_Count = 13,

		ENetProtocolCommand_Mask = 0x0F
	};

	enum ENetProtocolFlag
	{
		ENetProtocolFlag_Acknowledge = (1 << 7),
		ENetProtocolFlag_Unsequenced = (1 << 6),

		ENetProtocolHeaderFlag_Compressed = (1 << 14),
		ENetProtocolHeaderFlag_SentTime = (1 << 15),
		ENetProtocolHeaderFlag_Mask = ENetProtocolHeaderFlag_Compressed | ENetProtocolHeaderFlag_SentTime,

		ENetProtocolHeaderSessionMask = (3 << 12),
		ENetProtocolHeaderSessionShift = 12
	};

	enum class ENetEventType
	{
		/** no event occurred within the specified time limit */
		None,

		/** a peer has disconnected.  This event is generated on a successful
		* completion of a disconnect initiated by enet_peer_disconnect, if
		* a peer has timed out, or if a connection request initialized by
		* enet_host_connect has timed out.  The peer field contains the peer
		* which disconnected. The data field contains user supplied data
		* describing the disconnection, or 0, if none is available.
		*/
		Disconnect,

		/** a connection request initiated by enet_host_connect from this host has completed.
		* The peer field contains the peer which successfully connected.
		*/
		OutgoingConnect,

		/** a connection request initiated by enet_host_connect from another host has completed.
		* The peer field contains the peer which successfully connected.
		*/
		IncomingConnect,

		/** a packet has been received from a peer.  The peer field specifies the
		* peer which sent the packet.  The channelID field specifies the channel
		* number upon which the packet was received.  The packet field contains
		* the packet that was received;
		*/
		Receive
	};

	struct ENetEvent
	{
		ENetEventType type;
		ENetPeer*     peer;
		UInt8         channelId;
		UInt32        data;
		ENetPacketRef packet;
	};


	#ifdef _MSC_VER
	#pragma pack(push, 1)
	#define NAZARA_PACKED
	#elif defined(__GNUC__) || defined(__clang__)
	#define NAZARA_PACKED __attribute__ ((packed))
	#else
	#define NAZARA_PACKED
	#endif


	struct NAZARA_PACKED ENetProtocolHeader
	{
		UInt16 peerID;
		UInt16 sentTime;
	};

	struct NAZARA_PACKED ENetProtocolCommandHeader
	{
		UInt8  command;
		UInt8  channelID;
		UInt16 reliableSequenceNumber;
	};

	struct NAZARA_PACKED ENetProtocolAcknowledge
	{
		ENetProtocolCommandHeader header;
		UInt16 receivedReliableSequenceNumber;
		UInt16 receivedSentTime;
	};

	struct NAZARA_PACKED ENetProtocolConnect
	{
		ENetProtocolCommandHeader header;
		UInt16 outgoingPeerID;
		UInt8  incomingSessionID;
		UInt8  outgoingSessionID;
		UInt32 mtu;
		UInt32 windowSize;
		UInt32 channelCount;
		UInt32 incomingBandwidth;
		UInt32 outgoingBandwidth;
		UInt32 packetThrottleInterval;
		UInt32 packetThrottleAcceleration;
		UInt32 packetThrottleDeceleration;
		UInt32 connectID;
		UInt32 data;
	};

	struct NAZARA_PACKED ENetProtocolBandwidthLimit
	{
		ENetProtocolCommandHeader header;
		UInt32 incomingBandwidth;
		UInt32 outgoingBandwidth;
	};

	struct NAZARA_PACKED ENetProtocolDisconnect
	{
		ENetProtocolCommandHeader header;
		UInt32 data;
	};

	struct NAZARA_PACKED ENetProtocolPing
	{
		ENetProtocolCommandHeader header;
	};

	struct NAZARA_PACKED ENetProtocolSendFragment
	{
		ENetProtocolCommandHeader header;
		UInt16 startSequenceNumber;
		UInt16 dataLength;
		UInt32 fragmentCount;
		UInt32 fragmentNumber;
		UInt32 totalLength;
		UInt32 fragmentOffset;
	};

	struct NAZARA_PACKED ENetProtocolSendReliable
	{
		ENetProtocolCommandHeader header;
		UInt16 dataLength;
	};

	struct NAZARA_PACKED ENetProtocolSendUnreliable
	{
		ENetProtocolCommandHeader header;
		UInt16 unreliableSequenceNumber;
		UInt16 dataLength;
	};

	struct NAZARA_PACKED ENetProtocolSendUnsequenced
	{
		ENetProtocolCommandHeader header;
		UInt16 unsequencedGroup;
		UInt16 dataLength;
	};

	struct NAZARA_PACKED ENetProtocolThrottleConfigure
	{
		ENetProtocolCommandHeader header;
		UInt32 packetThrottleInterval;
		UInt32 packetThrottleAcceleration;
		UInt32 packetThrottleDeceleration;
	};

	struct NAZARA_PACKED ENetProtocolVerifyConnect
	{
		ENetProtocolCommandHeader header;
		UInt16 outgoingPeerID;
		UInt8  incomingSessionID;
		UInt8  outgoingSessionID;
		UInt32 mtu;
		UInt32 windowSize;
		UInt32 channelCount;
		UInt32 incomingBandwidth;
		UInt32 outgoingBandwidth;
		UInt32 packetThrottleInterval;
		UInt32 packetThrottleAcceleration;
		UInt32 packetThrottleDeceleration;
		UInt32 connectID;
	};

	union NAZARA_PACKED ENetProtocol
	{
		ENetProtocol() = default;

		ENetProtocol(UInt8 command, UInt8 channel)
		{
			header.command = command;
			header.channelID = channel;
		}

		ENetProtocolCommandHeader header;
		ENetProtocolAcknowledge acknowledge;
		ENetProtocolBandwidthLimit bandwidthLimit;
		ENetProtocolConnect connect;
		ENetProtocolDisconnect disconnect;
		ENetProtocolPing ping;
		ENetProtocolSendReliable sendReliable;
		ENetProtocolSendUnreliable sendUnreliable;
		ENetProtocolSendUnsequenced sendUnsequenced;
		ENetProtocolSendFragment sendFragment;
		ENetProtocolThrottleConfigure throttleConfigure;
		ENetProtocolVerifyConnect verifyConnect;
	};

	#ifdef _MSC_VER
	#pragma pack(pop)
	#endif
}

#include <Nazara/Network/ENetProtocol.inl>

#endif // NAZARA_ENETPROTOCOL_HPP
