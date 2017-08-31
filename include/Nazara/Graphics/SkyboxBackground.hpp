// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SKYBOXBACKGROUND_HPP
#define NAZARA_SKYBOXBACKGROUND_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>

namespace Nz
{
	class SkyboxBackground;

	using SkyboxBackgroundConstRef = ObjectRef<const SkyboxBackground>;
	using SkyboxBackgroundRef = ObjectRef<SkyboxBackground>;

	class NAZARA_GRAPHICS_API SkyboxBackground : public AbstractBackground
	{
		friend class Graphics;

		public:
			SkyboxBackground(TextureRef cubemapTexture = TextureRef());
			~SkyboxBackground() = default;

			void Draw(const AbstractViewer* viewer) const override;

			BackgroundType GetBackgroundType() const override;
			inline const Vector3f& GetMovementOffset() const;
			inline float GetMovementScale() const;
			inline const TextureRef& GetTexture() const;
			inline TextureSampler& GetTextureSampler();
			inline const TextureSampler& GetTextureSampler() const;

			inline void SetMovementOffset(const Vector3f& offset);
			inline void SetMovementScale(float scale);
			inline void SetTexture(TextureRef cubemapTexture);
			inline void SetTextureSampler(const TextureSampler& sampler);

			template<typename... Args> static SkyboxBackgroundRef New(Args&&... args);

		private:
			static bool Initialize();
			static void Uninitialize();

			TextureRef m_texture;
			TextureSampler m_sampler;
			Vector3f m_movementOffset;
			float m_movementScale;
	};
}

#include <Nazara/Graphics/SkyboxBackground.inl>

#endif // NAZARA_SKYBOXBACKGROUND_HPP
