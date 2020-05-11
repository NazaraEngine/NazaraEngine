// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLSHADERSTAGE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLSHADERSTAGE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/ShaderStageImpl.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Shader.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLShaderStage : public ShaderStageImpl
	{
		public:
			OpenGLShaderStage(OpenGLDevice& device, ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize);
			OpenGLShaderStage(const OpenGLShaderStage&) = delete;
			OpenGLShaderStage(OpenGLShaderStage&&) noexcept = default;
			~OpenGLShaderStage() = default;

			inline const GL::Shader& GetShader() const;

			OpenGLShaderStage& operator=(const OpenGLShaderStage&) = delete;
			OpenGLShaderStage& operator=(OpenGLShaderStage&&) noexcept = default;

		private:
			GL::Shader m_shader;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLShaderStage.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLSHADERSTAGE_HPP
