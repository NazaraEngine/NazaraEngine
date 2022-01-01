// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLSHADERMODULE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLSHADERMODULE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Program.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Shader.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/ShaderModule.hpp>
#include <Nazara/Shader/GlslWriter.hpp>
#include <Nazara/Shader/Ast/Nodes.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLShaderModule : public ShaderModule
	{
		public:
			OpenGLShaderModule(OpenGLDevice& device, ShaderStageTypeFlags shaderStages, ShaderAst::Statement& shaderAst, const ShaderWriter::States& states = {});
			OpenGLShaderModule(OpenGLDevice& device, ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const void* source, std::size_t sourceSize, const ShaderWriter::States& states = {});
			OpenGLShaderModule(const OpenGLShaderModule&) = delete;
			OpenGLShaderModule(OpenGLShaderModule&&) noexcept = default;
			~OpenGLShaderModule() = default;

			ShaderStageTypeFlags Attach(GL::Program& program, const GlslWriter::BindingMapping& bindingMapping) const;

			OpenGLShaderModule& operator=(const OpenGLShaderModule&) = delete;
			OpenGLShaderModule& operator=(OpenGLShaderModule&&) noexcept = default;

		private:
			void Create(OpenGLDevice& device, ShaderStageTypeFlags shaderStages, ShaderAst::Statement& shaderAst, const ShaderWriter::States& states);

			static void CheckCompilationStatus(GL::Shader& shader);

			struct GlslShader
			{
				std::string sourceCode;
			};

			struct ShaderStatement
			{
				std::shared_ptr<ShaderAst::Statement> ast;
			};

			struct Shader
			{
				ShaderStageType stage;
				std::variant<GlslShader, ShaderStatement> shader;
			};

			OpenGLDevice& m_device;
			ShaderWriter::States m_states;
			std::vector<Shader> m_shaders;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLShaderModule.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLSHADERMODULE_HPP
