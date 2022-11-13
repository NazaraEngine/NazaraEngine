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
#include <NZSL/GlslWriter.hpp>
#include <NZSL/Ast/Module.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLShaderModule : public ShaderModule
	{
		public:
			struct ExplicitBinding;

			OpenGLShaderModule(OpenGLDevice& device, nzsl::ShaderStageTypeFlags shaderStages, const nzsl::Ast::Module& shaderModule, const nzsl::ShaderWriter::States& states = {});
			OpenGLShaderModule(OpenGLDevice& device, nzsl::ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const void* source, std::size_t sourceSize, const nzsl::ShaderWriter::States& states = {});

			nzsl::ShaderStageTypeFlags Attach(GL::Program& program, const nzsl::GlslWriter::BindingMapping& bindingMapping, std::vector<ExplicitBinding>* explicitBindings) const;

			inline const std::vector<ExplicitBinding>& GetExplicitBindings() const;

			struct ExplicitBinding
			{
				std::string name;
				unsigned int binding;
				bool isBlock;
			};

		private:
			void Create(OpenGLDevice& device, nzsl::ShaderStageTypeFlags shaderStages, const nzsl::Ast::Module& shaderModule, const nzsl::ShaderWriter::States& states);

			static void CheckCompilationStatus(GL::Shader& shader);

			struct GlslShader
			{
				std::string sourceCode;
			};

			struct ShaderStatement
			{
				nzsl::Ast::ModulePtr ast;
			};

			struct Shader
			{
				nzsl::ShaderStageType stage;
				std::variant<GlslShader, ShaderStatement> shader;
			};

			OpenGLDevice& m_device;
			nzsl::ShaderWriter::States m_states;
			std::vector<ExplicitBinding> m_explicitBindings;
			std::vector<Shader> m_shaders;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLShaderModule.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLSHADERMODULE_HPP
