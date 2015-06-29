// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTUREBACKGROUND_HPP
#define NAZARA_TEXTUREBACKGROUND_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Renderer/UberShader.hpp>
#include <Nazara/Renderer/Texture.hpp>

class NzTextureBackground;

using NzTextureBackgroundConstRef = NzObjectRef<const NzTextureBackground>;
using NzTextureBackgroundRef = NzObjectRef<NzTextureBackground>;

class NAZARA_GRAPHICS_API NzTextureBackground : public NzAbstractBackground
{
	public:
		NzTextureBackground(NzTextureRef texture = NzTextureRef());

		void Draw(const NzAbstractViewer* viewer) const;

		nzBackgroundType GetBackgroundType() const;
		inline const NzTextureRef& GetTexture() const;

		inline void SetTexture(NzTextureRef texture);

		template<typename... Args> static NzTextureBackgroundRef New(Args&&... args);

	private:
		NzTextureRef m_texture;
		NzUberShaderConstRef m_uberShader;
		const NzUberShaderInstance* m_uberShaderInstance;
		int m_materialDiffuseUniform;
		int m_materialDiffuseMapUniform;
		int m_vertexDepthUniform;
};

#include <Nazara/Graphics/TextureBackground.inl>

#endif // NAZARA_TEXTUREBACKGROUND_HPP
