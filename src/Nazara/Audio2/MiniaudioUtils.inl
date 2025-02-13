// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	constexpr ma_dither_mode ToMiniaudio(AudioDitherMode ditherMode)
	{
		switch (ditherMode)
		{
			case AudioDitherMode::None:      return ma_dither_mode_none;
			case AudioDitherMode::Rectangle: return ma_dither_mode_rectangle;
			case AudioDitherMode::Triangle:  return ma_dither_mode_triangle;
		}

		NAZARA_UNREACHABLE();
	}

	constexpr ma_channel ToMiniaudio(AudioChannel channel)
	{
		switch (channel)
		{
			case AudioChannel::None:             return MA_CHANNEL_NONE;
			case AudioChannel::Mono:             return MA_CHANNEL_MONO;
			case AudioChannel::FrontLeft:        return MA_CHANNEL_FRONT_LEFT;
			case AudioChannel::FrontRight:       return MA_CHANNEL_FRONT_RIGHT;
			case AudioChannel::FrontCenter:      return MA_CHANNEL_FRONT_CENTER;
			case AudioChannel::LFE:              return MA_CHANNEL_LFE;
			case AudioChannel::BackLeft:         return MA_CHANNEL_BACK_LEFT;
			case AudioChannel::BackRight:        return MA_CHANNEL_BACK_RIGHT;
			case AudioChannel::FrontLeftCenter:  return MA_CHANNEL_FRONT_LEFT_CENTER;
			case AudioChannel::FrontRightCenter: return MA_CHANNEL_FRONT_RIGHT_CENTER;
			case AudioChannel::BackCenter:       return MA_CHANNEL_BACK_CENTER;
			case AudioChannel::SideLeft:         return MA_CHANNEL_SIDE_LEFT;
			case AudioChannel::SideRight:        return MA_CHANNEL_SIDE_RIGHT;
			case AudioChannel::TopCenter:        return MA_CHANNEL_TOP_CENTER;
			case AudioChannel::TopFrontLeft:     return MA_CHANNEL_TOP_FRONT_LEFT;
			case AudioChannel::TopFrontCenter:   return MA_CHANNEL_TOP_FRONT_CENTER;
			case AudioChannel::TopFrontRight:    return MA_CHANNEL_TOP_FRONT_RIGHT;
			case AudioChannel::TopBackLeft:      return MA_CHANNEL_TOP_BACK_LEFT;
			case AudioChannel::TopBackCenter:    return MA_CHANNEL_TOP_BACK_CENTER;
			case AudioChannel::TopBackRight:     return MA_CHANNEL_TOP_BACK_RIGHT;
			case AudioChannel::Aux0:             return MA_CHANNEL_AUX_0;
			case AudioChannel::Aux1:             return MA_CHANNEL_AUX_1;
			case AudioChannel::Aux2:             return MA_CHANNEL_AUX_2;
			case AudioChannel::Aux3:             return MA_CHANNEL_AUX_3;
			case AudioChannel::Aux4:             return MA_CHANNEL_AUX_4;
			case AudioChannel::Aux5:             return MA_CHANNEL_AUX_5;
			case AudioChannel::Aux6:             return MA_CHANNEL_AUX_6;
			case AudioChannel::Aux7:             return MA_CHANNEL_AUX_7;
			case AudioChannel::Aux8:             return MA_CHANNEL_AUX_8;
			case AudioChannel::Aux9:             return MA_CHANNEL_AUX_9;
			case AudioChannel::Aux10:            return MA_CHANNEL_AUX_10;
			case AudioChannel::Aux11:            return MA_CHANNEL_AUX_11;
			case AudioChannel::Aux12:            return MA_CHANNEL_AUX_12;
			case AudioChannel::Aux13:            return MA_CHANNEL_AUX_13;
			case AudioChannel::Aux14:            return MA_CHANNEL_AUX_14;
			case AudioChannel::Aux15:            return MA_CHANNEL_AUX_15;
			case AudioChannel::Aux16:            return MA_CHANNEL_AUX_16;
			case AudioChannel::Aux17:            return MA_CHANNEL_AUX_17;
			case AudioChannel::Aux18:            return MA_CHANNEL_AUX_18;
			case AudioChannel::Aux19:            return MA_CHANNEL_AUX_19;
			case AudioChannel::Aux20:            return MA_CHANNEL_AUX_20;
			case AudioChannel::Aux21:            return MA_CHANNEL_AUX_21;
			case AudioChannel::Aux22:            return MA_CHANNEL_AUX_22;
			case AudioChannel::Aux23:            return MA_CHANNEL_AUX_23;
			case AudioChannel::Aux24:            return MA_CHANNEL_AUX_24;
			case AudioChannel::Aux25:            return MA_CHANNEL_AUX_25;
			case AudioChannel::Aux26:            return MA_CHANNEL_AUX_26;
			case AudioChannel::Aux27:            return MA_CHANNEL_AUX_27;
			case AudioChannel::Aux28:            return MA_CHANNEL_AUX_28;
			case AudioChannel::Aux29:            return MA_CHANNEL_AUX_29;
			case AudioChannel::Aux30:            return MA_CHANNEL_AUX_30;
			case AudioChannel::Aux31:            return MA_CHANNEL_AUX_31;
		}

		NAZARA_UNREACHABLE();
	}

	constexpr ma_format ToMiniaudio(AudioFormat format)
	{
		switch (format)
		{
			case AudioFormat::Floating32: return ma_format_f32;
			case AudioFormat::Signed16:   return ma_format_s16;
			case AudioFormat::Signed24:   return ma_format_s24;
			case AudioFormat::Signed32:   return ma_format_s32;
			case AudioFormat::Unsigned8:  return ma_format_u8;
		}

		NAZARA_UNREACHABLE();
	}
}
