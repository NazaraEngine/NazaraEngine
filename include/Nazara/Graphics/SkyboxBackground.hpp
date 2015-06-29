// Copyright (C) 2015 Jérôme Leclercq
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

class NzSkyboxBackground;

using NzSkyboxBackgroundConstRef = NzObjectRef<const NzSkyboxBackground>;
using NzSkyboxBackgroundRef = NzObjectRef<NzSkyboxBackground>;

class NAZARA_GRAPHICS_API NzSkyboxBackground : public NzAbstractBackground
{
	friend class NzGraphics;

	public:
		NzSkyboxBackground(NzTextureRef cubemapTexture = NzTextureRef());
		~NzSkyboxBackground() = default;

		void Draw(const NzAbstractViewer* viewer) const;

		nzBackgroundType GetBackgroundType() const;
		inline const NzTextureRef& GetTexture() const;
		inline NzTextureSampler& GetTextureSampler();
		inline const NzTextureSampler& GetTextureSampler() const;

		inline void SetTexture(NzTextureRef cubemapTexture);
		inline void SetTextureSampler(const NzTextureSampler& sampler);

		template<typename... Args> static NzSkyboxBackgroundRef New(Args&&... args);

	private:
		static bool Initialize();
		static void Uninitialize();

		NzTextureRef m_texture;
		NzTextureSampler m_sampler;
};

#include <Nazara/Graphics/SkyboxBackground.inl>

#endif // NAZARA_SKYBOXBACKGROUND_HPP
