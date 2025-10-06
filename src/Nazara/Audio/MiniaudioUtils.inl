// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	constexpr AudioDitherMode FromMiniaudio(ma_dither_mode ditherMode)
	{
		switch (ditherMode)
		{
			case ma_dither_mode_none:      return AudioDitherMode::None;
			case ma_dither_mode_rectangle: return AudioDitherMode::Rectangle;
			case ma_dither_mode_triangle:  return AudioDitherMode::Triangle;
		}

		NAZARA_UNREACHABLE();
	}

	constexpr AudioChannel FromMiniaudio(ma_channel channel)
	{
		switch (channel)
		{
			case MA_CHANNEL_NONE:               return AudioChannel::None;
			case MA_CHANNEL_MONO:               return AudioChannel::Mono;
			case MA_CHANNEL_FRONT_LEFT:         return AudioChannel::FrontLeft;
			case MA_CHANNEL_FRONT_RIGHT:        return AudioChannel::FrontRight;
			case MA_CHANNEL_FRONT_CENTER:       return AudioChannel::FrontCenter;
			case MA_CHANNEL_LFE:                return AudioChannel::LFE;
			case MA_CHANNEL_BACK_LEFT:          return AudioChannel::BackLeft;
			case MA_CHANNEL_BACK_RIGHT:         return AudioChannel::BackRight;
			case MA_CHANNEL_FRONT_LEFT_CENTER:  return AudioChannel::FrontLeftCenter;
			case MA_CHANNEL_FRONT_RIGHT_CENTER: return AudioChannel::FrontRightCenter;
			case MA_CHANNEL_BACK_CENTER:        return AudioChannel::BackCenter;
			case MA_CHANNEL_SIDE_LEFT:          return AudioChannel::SideLeft;
			case MA_CHANNEL_SIDE_RIGHT:         return AudioChannel::SideRight;
			case MA_CHANNEL_TOP_CENTER:         return AudioChannel::TopCenter;
			case MA_CHANNEL_TOP_FRONT_LEFT:     return AudioChannel::TopFrontLeft;
			case MA_CHANNEL_TOP_FRONT_CENTER:   return AudioChannel::TopFrontCenter;
			case MA_CHANNEL_TOP_FRONT_RIGHT:    return AudioChannel::TopFrontRight;
			case MA_CHANNEL_TOP_BACK_LEFT:      return AudioChannel::TopBackLeft;
			case MA_CHANNEL_TOP_BACK_CENTER:    return AudioChannel::TopBackCenter;
			case MA_CHANNEL_TOP_BACK_RIGHT:     return AudioChannel::TopBackRight;
			case MA_CHANNEL_AUX_0:              return AudioChannel::Aux0;
			case MA_CHANNEL_AUX_1:              return AudioChannel::Aux1;
			case MA_CHANNEL_AUX_2:              return AudioChannel::Aux2;
			case MA_CHANNEL_AUX_3:              return AudioChannel::Aux3;
			case MA_CHANNEL_AUX_4:              return AudioChannel::Aux4;
			case MA_CHANNEL_AUX_5:              return AudioChannel::Aux5;
			case MA_CHANNEL_AUX_6:              return AudioChannel::Aux6;
			case MA_CHANNEL_AUX_7:              return AudioChannel::Aux7;
			case MA_CHANNEL_AUX_8:              return AudioChannel::Aux8;
			case MA_CHANNEL_AUX_9:              return AudioChannel::Aux9;
			case MA_CHANNEL_AUX_10:             return AudioChannel::Aux10;
			case MA_CHANNEL_AUX_11:             return AudioChannel::Aux11;
			case MA_CHANNEL_AUX_12:             return AudioChannel::Aux12;
			case MA_CHANNEL_AUX_13:             return AudioChannel::Aux13;
			case MA_CHANNEL_AUX_14:             return AudioChannel::Aux14;
			case MA_CHANNEL_AUX_15:             return AudioChannel::Aux15;
			case MA_CHANNEL_AUX_16:             return AudioChannel::Aux16;
			case MA_CHANNEL_AUX_17:             return AudioChannel::Aux17;
			case MA_CHANNEL_AUX_18:             return AudioChannel::Aux18;
			case MA_CHANNEL_AUX_19:             return AudioChannel::Aux19;
			case MA_CHANNEL_AUX_20:             return AudioChannel::Aux20;
			case MA_CHANNEL_AUX_21:             return AudioChannel::Aux21;
			case MA_CHANNEL_AUX_22:             return AudioChannel::Aux22;
			case MA_CHANNEL_AUX_23:             return AudioChannel::Aux23;
			case MA_CHANNEL_AUX_24:             return AudioChannel::Aux24;
			case MA_CHANNEL_AUX_25:             return AudioChannel::Aux25;
			case MA_CHANNEL_AUX_26:             return AudioChannel::Aux26;
			case MA_CHANNEL_AUX_27:             return AudioChannel::Aux27;
			case MA_CHANNEL_AUX_28:             return AudioChannel::Aux28;
			case MA_CHANNEL_AUX_29:             return AudioChannel::Aux29;
			case MA_CHANNEL_AUX_30:             return AudioChannel::Aux30;
			case MA_CHANNEL_AUX_31:             return AudioChannel::Aux31;
		}

		NAZARA_UNREACHABLE();
	}

	constexpr AudioFormat FromMiniaudio(ma_format format)
	{
		switch (format)
		{
			case ma_format_f32:     return AudioFormat::Floating32;
			case ma_format_s16:     return AudioFormat::Signed16;
			case ma_format_s24:     return AudioFormat::Signed24;
			case ma_format_s32:     return AudioFormat::Signed32;
			case ma_format_u8:      return AudioFormat::Unsigned8;
			case ma_format_unknown: return AudioFormat::Unknown;

			case ma_format_count: break;
		}

		NAZARA_UNREACHABLE();
	}

	constexpr SoundAttenuationModel FromMiniaudio(ma_attenuation_model attenuation)
	{
		switch (attenuation)
		{
			case ma_attenuation_model_exponential: return SoundAttenuationModel::Exponential;
			case ma_attenuation_model_inverse:     return SoundAttenuationModel::Inverse;
			case ma_attenuation_model_linear:      return SoundAttenuationModel::Linear;
			case ma_attenuation_model_none:        return SoundAttenuationModel::None;
		}

		NAZARA_UNREACHABLE();
	}

	constexpr SoundPanMode FromMiniaudio(ma_pan_mode panMode)
	{
		switch (panMode)
		{
			case ma_pan_mode_balance: return SoundPanMode::Balance;
			case ma_pan_mode_pan:     return SoundPanMode::Pan;
		}

		NAZARA_UNREACHABLE();
	}

	constexpr SoundPositioning FromMiniaudio(ma_positioning positioning)
	{
		switch (positioning)
		{
			case ma_positioning_absolute: return SoundPositioning::Absolute;
			case ma_positioning_relative: return SoundPositioning::Relative;
		}

		NAZARA_UNREACHABLE();
	}

	constexpr Vector3f FromMiniaudio(const ma_vec3f& vec)
	{
		return Vector3f{ vec.x, vec.y, vec.z };
	}

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
			case AudioFormat::Unknown:    return ma_format_unknown;
			case AudioFormat::Unsigned8:  return ma_format_u8;
		}

		NAZARA_UNREACHABLE();
	}

	constexpr ma_attenuation_model ToMiniaudio(SoundAttenuationModel attenuation)
	{
		switch (attenuation)
		{
			case SoundAttenuationModel::Exponential: return ma_attenuation_model_exponential;
			case SoundAttenuationModel::Inverse:     return ma_attenuation_model_inverse;
			case SoundAttenuationModel::Linear:      return ma_attenuation_model_linear;
			case SoundAttenuationModel::None:        return ma_attenuation_model_none;
		}

		NAZARA_UNREACHABLE();
	}

	constexpr ma_pan_mode ToMiniaudio(SoundPanMode panMode)
	{
		switch (panMode)
		{
			case SoundPanMode::Balance: return ma_pan_mode_balance;
			case SoundPanMode::Pan:     return ma_pan_mode_pan;
		}

		NAZARA_UNREACHABLE();
	}

	constexpr ma_positioning ToMiniaudio(SoundPositioning positioning)
	{
		switch (positioning)
		{
			case SoundPositioning::Absolute: return ma_positioning_absolute;
			case SoundPositioning::Relative: return ma_positioning_relative;
		}

		NAZARA_UNREACHABLE();
	}

	constexpr ma_vec3f ToMiniaudio(const Vector3f& vec)
	{
		return ma_vec3f{ vec.x, vec.y, vec.z };
	}
}
