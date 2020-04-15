// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLSHADERSTAGE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLSHADERSTAGE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/ShaderStageImpl.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/ShaderModule.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLShaderStage : public ShaderStageImpl
	{
		public:
			OpenGLShaderStage() = default;
			OpenGLShaderStage(const OpenGLShaderStage&) = delete;
			OpenGLShaderStage(OpenGLShaderStage&&) noexcept = default;
			~OpenGLShaderStage() = default;

			bool Create(Vk::Device& device, ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize);

			inline const Vk::ShaderModule& GetHandle() const;
			inline ShaderStageType GetStageType() const;

			OpenGLShaderStage& operator=(const OpenGLShaderStage&) = delete;
			OpenGLShaderStage& operator=(OpenGLShaderStage&&) noexcept = default;

		private:
			Vk::ShaderModule m_shaderModule;
			ShaderStageType m_stage;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLShaderStage.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLSHADERSTAGE_HPP
