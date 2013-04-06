// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SKYBOXBACKGROUND_HPP
#define NAZARA_SKYBOXBACKGROUND_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/Background.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>

class NzIndexBuffer;
class NzShader;
class NzVertexBuffer;

class NAZARA_API NzSkyboxBackground : public NzBackground
{
	public:
		NzSkyboxBackground();
		NzSkyboxBackground(NzTexture* cubemapTexture);
		~NzSkyboxBackground();

		void Draw(const NzScene* scene) const;

		nzBackgroundType GetBackgroundType() const;
		NzTexture* GetTexture() const;
		const NzTextureSampler& GetTextureSampler();

		void SetTexture(NzTexture* cubemapTexture);
		void SetTextureSampler(const NzTextureSampler& sampler);

	private:
		NzTextureRef m_texture;
		NzTextureSampler m_sampler;
		NzIndexBuffer* m_indexBuffer;
		NzShader* m_shader;
		NzVertexBuffer* m_vertexBuffer;
};

#endif // NAZARA_SKYBOXBACKGROUND_HPP
