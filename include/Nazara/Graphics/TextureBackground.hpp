// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTUREBACKGROUND_HPP
#define NAZARA_TEXTUREBACKGROUND_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Renderer/UberShader.hpp>
#include <Nazara/Renderer/Texture.hpp>

namespace Nz
{
	class TextureBackground;

	using TextureBackgroundConstRef = ObjectRef<const TextureBackground>;
	using TextureBackgroundRef = ObjectRef<TextureBackground>;

	class NAZARA_GRAPHICS_API TextureBackground : public AbstractBackground
	{
		public:
			TextureBackground(TextureRef texture = TextureRef());

			void Draw(const AbstractViewer* viewer) const override;

			BackgroundType GetBackgroundType() const override;
			inline const TextureRef& GetTexture() const;

			inline void SetTexture(TextureRef texture);

			template<typename... Args> static TextureBackgroundRef New(Args&&... args);

		private:
			TextureRef m_texture;
			UberShaderConstRef m_uberShader;
			const UberShaderInstance* m_uberShaderInstance;
			int m_materialDiffuseUniform;
			int m_materialDiffuseMapUniform;
			int m_vertexDepthUniform;
	};
}

#include <Nazara/Graphics/TextureBackground.inl>

#endif // NAZARA_TEXTUREBACKGROUND_HPP
