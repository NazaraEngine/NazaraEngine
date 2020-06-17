// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_HPP
#define NAZARA_SHADER_AST_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/ShaderNodes.hpp>
#include <optional>
#include <string>
#include <vector>

namespace Nz
{
	class NAZARA_RENDERER_API ShaderAst
	{
		public:
			struct Function;
			struct FunctionParameter;
			struct InputOutput;
			struct VariableBase;
			struct Uniform;

			ShaderAst() = default;
			~ShaderAst() = default;

			void AddFunction(std::string name, ShaderNodes::StatementPtr statement, std::vector<FunctionParameter> parameters = {}, ShaderNodes::ExpressionType returnType = ShaderNodes::ExpressionType::Void);
			void AddInput(std::string name, ShaderNodes::ExpressionType type, std::optional<std::size_t> locationIndex);
			void AddOutput(std::string name, ShaderNodes::ExpressionType type, std::optional<std::size_t> locationIndex);
			void AddUniform(std::string name, ShaderNodes::ExpressionType type, std::optional<std::size_t> bindingIndex);

			inline const Function& GetFunction(std::size_t i) const;
			inline std::size_t GetFunctionCount() const;
			inline const std::vector<Function>& GetFunctions() const;
			inline const InputOutput& GetInput(std::size_t i) const;
			inline std::size_t GetInputCount() const;
			inline const std::vector<InputOutput>& GetInputs() const;
			inline const InputOutput& GetOutput(std::size_t i) const;
			inline std::size_t GetOutputCount() const;
			inline const std::vector<InputOutput>& GetOutputs() const;
			inline const Uniform& GetUniform(std::size_t i) const;
			inline std::size_t GetUniformCount() const;
			inline const std::vector<Uniform>& GetUniforms() const;

			struct VariableBase
			{
				std::string name;
				ShaderNodes::ExpressionType type;
			};

			struct FunctionParameter : VariableBase
			{
			};

			struct Function
			{
				std::string name;
				std::vector<FunctionParameter> parameters;
				ShaderNodes::ExpressionType returnType;
				ShaderNodes::StatementPtr statement;
			};

			struct InputOutput : VariableBase
			{
				std::optional<std::size_t> locationIndex;
			};

			struct Uniform : VariableBase
			{
				std::optional<std::size_t> bindingIndex;
			};

		private:
			std::vector<Function> m_functions;
			std::vector<InputOutput> m_inputs;
			std::vector<InputOutput> m_outputs;
			std::vector<Uniform> m_uniforms;
	};
}

#include <Nazara/Renderer/ShaderAst.inl>

#endif // NAZARA_SHADER_AST_HPP
