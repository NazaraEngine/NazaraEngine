// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_VARIABLES_HPP
#define NAZARA_SHADER_VARIABLES_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderExpressionType.hpp>
#include <array>
#include <memory>
#include <optional>
#include <string>

namespace Nz
{
	class ShaderVarVisitor;

	namespace ShaderNodes
	{
		struct Variable;

		using VariablePtr = std::shared_ptr<Variable>;

		struct NAZARA_SHADER_API Variable : std::enable_shared_from_this<Variable>
		{
			virtual ~Variable();

			virtual VariableType GetType() const = 0;
			virtual void Visit(ShaderVarVisitor& visitor) = 0;

			ShaderExpressionType type;
		};

		struct BuiltinVariable;

		using BuiltinVariablePtr = std::shared_ptr<BuiltinVariable>;

		struct NAZARA_SHADER_API BuiltinVariable : public Variable
		{
			BuiltinEntry entry;

			VariableType GetType() const override;
			void Visit(ShaderVarVisitor& visitor) override;

			static inline std::shared_ptr<BuiltinVariable> Build(BuiltinEntry entry, ShaderExpressionType varType);
		};

		struct NamedVariable;

		using NamedVariablePtr = std::shared_ptr<NamedVariable>;

		struct NAZARA_SHADER_API NamedVariable : public Variable
		{
			std::string name;
		};

		struct InputVariable;

		using InputVariablePtr = std::shared_ptr<InputVariable>;

		struct NAZARA_SHADER_API InputVariable : public NamedVariable
		{
			VariableType GetType() const override;
			void Visit(ShaderVarVisitor& visitor) override;

			static inline std::shared_ptr<InputVariable> Build(std::string varName, ShaderExpressionType varType);
		};

		struct LocalVariable;

		using LocalVariablePtr = std::shared_ptr<LocalVariable>;

		struct NAZARA_SHADER_API LocalVariable : public NamedVariable
		{
			VariableType GetType() const override;
			void Visit(ShaderVarVisitor& visitor) override;

			static inline std::shared_ptr<LocalVariable> Build(std::string varName, ShaderExpressionType varType);
		};

		struct OutputVariable;

		using OutputVariablePtr = std::shared_ptr<OutputVariable>;

		struct NAZARA_SHADER_API OutputVariable : public NamedVariable
		{
			VariableType GetType() const override;
			void Visit(ShaderVarVisitor& visitor) override;

			static inline std::shared_ptr<OutputVariable> Build(std::string varName, ShaderExpressionType varType);
		};

		struct ParameterVariable;

		using ParameterVariablePtr = std::shared_ptr<ParameterVariable>;

		struct NAZARA_SHADER_API ParameterVariable : public NamedVariable
		{
			VariableType GetType() const override;
			void Visit(ShaderVarVisitor& visitor) override;

			static inline std::shared_ptr<ParameterVariable> Build(std::string varName, ShaderExpressionType varType);
		};

		struct UniformVariable;

		using UniformVariablePtr = std::shared_ptr<UniformVariable>;

		struct NAZARA_SHADER_API UniformVariable : public NamedVariable
		{
			VariableType GetType() const override;
			void Visit(ShaderVarVisitor& visitor) override;

			static inline std::shared_ptr<UniformVariable> Build(std::string varName, ShaderExpressionType varType);
		};
	}
}

#include <Nazara/Shader/ShaderVariables.inl>

#endif
