// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTUREBACKGROUND_HPP
#define NAZARA_TEXTUREBACKGROUND_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Renderer/ShaderProgram.hpp>
#include <Nazara/Renderer/Texture.hpp>

class NAZARA_API NzTextureBackground : public NzAbstractBackground
{
	public:
		NzTextureBackground();
		NzTextureBackground(NzTexture* texture);

		void Draw(const NzScene* scene) const;

		nzBackgroundType GetBackgroundType() const;
		NzTexture* GetTexture() const;

		void SetTexture(NzTexture* texture);

	private:
		NzShaderProgramConstRef m_program;
		NzTextureRef m_texture;
};

#endif // NAZARA_TEXTUREBACKGROUND_HPP
