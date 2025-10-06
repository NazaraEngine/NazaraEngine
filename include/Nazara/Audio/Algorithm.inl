// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <array>

namespace Nz
{
	inline std::span<const AudioChannel> GetAudioChannelMap(UInt32 channelCount)
	{
		switch (channelCount)
		{
			case 0:
				return {};

			case 1:
			{
				static constexpr std::array s_channels = { AudioChannel::Mono };
				return s_channels;
			}

			case 2:
			{
				static constexpr std::array s_channels = { AudioChannel::FrontLeft, AudioChannel::FrontRight };
				return s_channels;
			}

			case 3:
			{
				static constexpr std::array s_channels = { AudioChannel::FrontLeft, AudioChannel::FrontRight, AudioChannel::FrontCenter };
				return s_channels;
			}

			case 4:
			{
				static constexpr std::array s_channels = { AudioChannel::FrontLeft, AudioChannel::FrontRight, AudioChannel::BackLeft, AudioChannel::BackRight };
				return s_channels;
			}

			case 5:
			{
				static constexpr std::array s_channels = { AudioChannel::FrontLeft, AudioChannel::FrontRight, AudioChannel::FrontCenter, AudioChannel::BackLeft, AudioChannel::BackRight };
				return s_channels;
			}

			case 6:
			{
				static constexpr std::array s_channels = { AudioChannel::FrontLeft, AudioChannel::FrontRight, AudioChannel::FrontCenter, AudioChannel::LFE, AudioChannel::BackLeft, AudioChannel::BackRight };
				return s_channels;
			}

			case 7:
			{
				static constexpr std::array s_channels = { AudioChannel::FrontLeft, AudioChannel::FrontRight, AudioChannel::FrontCenter, AudioChannel::LFE, AudioChannel::BackCenter, AudioChannel::SideLeft, AudioChannel::SideRight };
				return s_channels;
			}

			case 8:
			default:
			{
				static constexpr std::array s_channels = {
					AudioChannel::FrontLeft, AudioChannel::FrontRight, AudioChannel::FrontCenter, AudioChannel::LFE, AudioChannel::BackLeft, AudioChannel::BackRight, AudioChannel::SideLeft, AudioChannel::SideRight,
					AudioChannel::Aux0, AudioChannel::Aux1, AudioChannel::Aux2, AudioChannel::Aux3, AudioChannel::Aux4, AudioChannel::Aux5, AudioChannel::Aux6, AudioChannel::Aux7, AudioChannel::Aux8, AudioChannel::Aux9, AudioChannel::Aux10, AudioChannel::Aux11, AudioChannel::Aux12, AudioChannel::Aux13, AudioChannel::Aux14, AudioChannel::Aux15, AudioChannel::Aux16, AudioChannel::Aux17, AudioChannel::Aux18, AudioChannel::Aux19, AudioChannel::Aux20, AudioChannel::Aux21, AudioChannel::Aux22, AudioChannel::Aux23, AudioChannel::Aux24, AudioChannel::Aux25, AudioChannel::Aux26, AudioChannel::Aux27, AudioChannel::Aux28, AudioChannel::Aux29, AudioChannel::Aux30, AudioChannel::Aux31
				};

				NazaraAssert(channelCount < s_channels.size());
				return std::span(s_channels.data(), channelCount);
			}
		}
	}
}
