// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <cassert>

namespace Nz
{
	inline void Light::EnableShadowCasting(bool castShadows)
	{
		if (m_isShadowCaster != castShadows)
		{
			m_isShadowCaster = castShadows;
			OnLightShadowCastingChanged(this, castShadows);
		}
	}

	inline const BoundingVolumef& Light::GetBoundingVolume() const
	{
		return m_boundingVolume;
	}

	inline UInt8 Light::GetLightType() const
	{
		return m_lightType;
	}

	inline PixelFormat Light::GetShadowMapFormat() const
	{
		return m_shadowMapFormat;
	}

	inline UInt32 Light::GetShadowMapSize() const
	{
		return m_shadowMapSize;
	}

	inline bool Light::IsShadowCaster() const
	{
		return m_isShadowCaster;
	}

	inline void Light::UpdateShadowMapFormat(PixelFormat format)
	{
		if (m_shadowMapFormat != format)
		{
			[[maybe_unused]] PixelFormatContent content = PixelFormatInfo::GetContent(format);
			NazaraAssert(content != PixelFormatContent::Depth && content != PixelFormatContent::DepthStencil, "invalid shadow map format (has no depth)");

			OnLightShadowMapSettingChange(this, format, m_shadowMapSize);
			m_shadowMapFormat = format;
		}
	}

	inline void Light::UpdateShadowMapSettings(UInt32 size, PixelFormat format)
	{
		if (m_shadowMapFormat != format || m_shadowMapSize != size)
		{
			NazaraAssert(size > 0, "invalid shadow map size");
			[[maybe_unused]] PixelFormatContent content = PixelFormatInfo::GetContent(format);
			NazaraAssert(content != PixelFormatContent::Depth && content != PixelFormatContent::DepthStencil, "invalid shadow map format (has no depth)");

			OnLightShadowMapSettingChange(this, format, size);
			m_shadowMapFormat = format;
			m_shadowMapSize = size;
		}
	}

	inline void Light::UpdateShadowMapSize(UInt32 size)
	{
		if (m_shadowMapSize != size)
		{
			NazaraAssert(size > 0, "invalid shadow map size");

			OnLightShadowMapSettingChange(this, m_shadowMapFormat, size);
			m_shadowMapSize = size;
		}
	}

	inline void Light::UpdateBoundingVolume(const BoundingVolumef& boundingVolume)
	{
		m_boundingVolume = boundingVolume;

		OnLightDataInvalided(this);
	}
}

