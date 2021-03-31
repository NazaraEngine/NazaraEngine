// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLSHADERMODULE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLSHADERMODULE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/ShaderModule.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Shader.hpp>
#include <Nazara/Shader/ShaderWriter.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLShaderModule : public ShaderModule
	{
		public:
			OpenGLShaderModule(OpenGLDevice& device, ShaderStageTypeFlags shaderStages, ShaderAst::StatementPtr& shaderAst, const ShaderWriter::States& states);
			OpenGLShaderModule(OpenGLDevice& device, ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const void* source, std::size_t sourceSize);
			OpenGLShaderModule(const OpenGLShaderModule&) = delete;
			OpenGLShaderModule(OpenGLShaderModule&&) noexcept = default;
			~OpenGLShaderModule() = default;

			inline const std::vector<GL::Shader>& GetShaders() const;

			OpenGLShaderModule& operator=(const OpenGLShaderModule&) = delete;
			OpenGLShaderModule& operator=(OpenGLShaderModule&&) noexcept = default;

		private:
			void Create(OpenGLDevice& device, ShaderStageTypeFlags shaderStages, ShaderAst::StatementPtr& shaderAst, const ShaderWriter::States& states);

			static void CheckCompilationStatus(GL::Shader& shader);

			std::vector<GL::Shader> m_shaders;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLShaderModule.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLSHADERSTAGE_HPP
