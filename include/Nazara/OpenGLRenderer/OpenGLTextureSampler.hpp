// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLTEXTURESAMPLER_HPP
#define NAZARA_OPENGLRENDERER_OPENGLTEXTURESAMPLER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Sampler.hpp>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLTextureSampler : public TextureSampler
	{
		public:
			OpenGLTextureSampler(OpenGLDevice& device, TextureSamplerInfo samplerInfo);
			OpenGLTextureSampler(const OpenGLTextureSampler&) = default;
			OpenGLTextureSampler(OpenGLTextureSampler&&) noexcept = default;
			~OpenGLTextureSampler() = default;

			inline const GL::Sampler& GetSampler() const;

			OpenGLTextureSampler& operator=(const OpenGLTextureSampler&) = delete;
			OpenGLTextureSampler& operator=(OpenGLTextureSampler&&) = delete;

		private:
			GL::Sampler m_sampler;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLTextureSampler.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLTEXTURESAMPLER_HPP
