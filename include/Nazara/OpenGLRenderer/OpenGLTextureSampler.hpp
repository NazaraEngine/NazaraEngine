// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLTEXTURESAMPLER_HPP
#define NAZARA_OPENGLRENDERER_OPENGLTEXTURESAMPLER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Sampler.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLTextureSampler : public TextureSampler
	{
		public:
			OpenGLTextureSampler(OpenGLDevice& device, TextureSamplerInfo samplerInfo);
			OpenGLTextureSampler(const OpenGLTextureSampler&) = delete;
			OpenGLTextureSampler(OpenGLTextureSampler&&) = delete;
			~OpenGLTextureSampler() = default;

			inline const GL::Sampler& GetSampler(bool mipmaps) const;

			void UpdateDebugName(std::string_view name) override;

			OpenGLTextureSampler& operator=(const OpenGLTextureSampler&) = delete;
			OpenGLTextureSampler& operator=(OpenGLTextureSampler&&) = delete;

		private:
			static void BuildSampler(OpenGLDevice& device, GL::Sampler& sampler, const TextureSamplerInfo& samplerInfo, bool withMipmaps);

			GL::Sampler m_samplerWithMipmaps;
			GL::Sampler m_samplerWithoutMipmaps;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLTextureSampler.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLTEXTURESAMPLER_HPP
