// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLTEXTURE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLTEXTURE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Texture.hpp>
#include <Nazara/Renderer/Texture.hpp>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLTexture : public Texture
	{
		public:
			OpenGLTexture(OpenGLDevice& device, const TextureInfo& params);
			OpenGLTexture(const OpenGLTexture&) = delete;
			OpenGLTexture(OpenGLTexture&&) = delete;
			~OpenGLTexture() = default;

			bool Copy(const Texture& source, const Boxui& srcBox, const Vector3ui& dstPos) override;

			PixelFormat GetFormat() const override;
			UInt8 GetLevelCount() const override;
			Vector3ui GetSize(UInt8 level = 0) const override;
			inline const GL::Texture& GetTexture() const;
			ImageType GetType() const override;

			using Texture::Update;
			bool Update(const void* ptr, const Boxui& box, unsigned int srcWidth = 0, unsigned int srcHeight = 0, UInt8 level = 0) override;

			void UpdateDebugName(std::string_view name) override;

			OpenGLTexture& operator=(const OpenGLTexture&) = delete;
			OpenGLTexture& operator=(OpenGLTexture&&) = delete;

			static inline GL::TextureTarget ToTextureTarget(ImageType imageType);

		private:
			GL::Texture m_texture;
			TextureInfo m_params;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLTexture.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLTEXTURE_HPP
