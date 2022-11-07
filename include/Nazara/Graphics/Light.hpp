// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_LIGHT_HPP
#define NAZARA_GRAPHICS_LIGHT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Math/BoundingVolume.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Utils/Signal.hpp>
#include <memory>

namespace Nz
{
	class CommandBufferBuilder;
	class RenderBuffer;
	class RenderFrame;
	class Texture;

	class NAZARA_GRAPHICS_API Light
	{
		public:
			Light(UInt8 lightType);
			Light(const Light&) = delete;
			Light(Light&&) noexcept = default;
			virtual ~Light();

			virtual float ComputeContributionScore(const BoundingVolumef& boundingVolume) const = 0;

			inline void EnableShadowCasting(bool castShadows);

			virtual void FillLightData(void* data) const = 0;

			inline const BoundingVolumef& GetBoundingVolume() const;
			inline UInt8 GetLightType() const;
			inline PixelFormat GetShadowMapFormat() const;
			inline UInt32 GetShadowMapSize() const;

			inline bool IsShadowCaster() const;

			inline void UpdateShadowMapFormat(PixelFormat format);
			inline void UpdateShadowMapSettings(UInt32 size, PixelFormat format);
			inline void UpdateShadowMapSize(UInt32 size);

			virtual void UpdateTransform(const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale) = 0;

			Light& operator=(const Light&) = delete;
			Light& operator=(Light&&) noexcept = default;

			NazaraSignal(OnLightDataInvalided, Light* /*emitter*/);
			NazaraSignal(OnLightShadowCastingChanged, Light* /*light*/, bool /*isShadowCasting*/);
			NazaraSignal(OnLightShadowMapSettingChange, Light* /*light*/, PixelFormat /*newPixelFormat*/, UInt32 /*newSize*/);

		protected:
			inline void UpdateBoundingVolume(const BoundingVolumef& boundingVolume);

		private:
			BoundingVolumef m_boundingVolume;
			PixelFormat m_shadowMapFormat;
			UInt8 m_lightType;
			UInt32 m_shadowMapSize;
			bool m_isShadowCaster;
	};
}

#include <Nazara/Graphics/Light.inl>

#endif // NAZARA_GRAPHICS_LIGHT_HPP
