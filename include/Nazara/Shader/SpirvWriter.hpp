// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVWRITER_HPP
#define NAZARA_SPIRVWRITER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAst.hpp>
#include <Nazara/Shader/ShaderAstVisitor.hpp>
#include <Nazara/Shader/ShaderConstantValue.hpp>
#include <Nazara/Shader/ShaderVarVisitor.hpp>
#include <Nazara/Shader/ShaderWriter.hpp>
#include <Nazara/Shader/SpirvConstantCache.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Nz
{
	class SpirvSection;

	class NAZARA_SHADER_API SpirvWriter : public ShaderWriter
	{
		friend class SpirvAstVisitor;
		friend class SpirvExpressionLoad;
		friend class SpirvExpressionStore;
		friend class SpirvVisitor;

		public:
			struct Environment;

			SpirvWriter();
			SpirvWriter(const SpirvWriter&) = delete;
			SpirvWriter(SpirvWriter&&) = delete;
			~SpirvWriter() = default;

			std::vector<UInt32> Generate(const ShaderAst& shader, const States& conditions = {});

			void SetEnv(Environment environment);

			struct Environment
			{
				UInt32 spvMajorVersion = 1;
				UInt32 spvMinorVersion = 0;
			};

		private:
			struct ExtVar;
			struct OnlyCache {};

			UInt32 AllocateResultId();

			void AppendHeader();

			UInt32 GetConstantId(const ShaderConstantValue& value) const;
			UInt32 GetFunctionTypeId(ShaderExpressionType retType, const std::vector<ShaderAst::FunctionParameter>& parameters);
			const ExtVar& GetBuiltinVariable(ShaderNodes::BuiltinEntry builtin) const;
			const ExtVar& GetInputVariable(const std::string& name) const;
			const ExtVar& GetOutputVariable(const std::string& name) const;
			const ExtVar& GetUniformVariable(const std::string& name) const;
			SpirvSection& GetInstructions();
			UInt32 GetPointerTypeId(const ShaderExpressionType& type, SpirvStorageClass storageClass) const;
			UInt32 GetTypeId(const ShaderExpressionType& type) const;

			inline bool IsConditionEnabled(const std::string& condition) const;

			UInt32 ReadInputVariable(const std::string& name);
			std::optional<UInt32> ReadInputVariable(const std::string& name, OnlyCache);
			UInt32 ReadLocalVariable(const std::string& name);
			std::optional<UInt32> ReadLocalVariable(const std::string& name, OnlyCache);
			UInt32 ReadUniformVariable(const std::string& name);
			std::optional<UInt32> ReadUniformVariable(const std::string& name, OnlyCache);
			UInt32 ReadVariable(ExtVar& var);
			std::optional<UInt32> ReadVariable(const ExtVar& var, OnlyCache);

			UInt32 RegisterConstant(const ShaderConstantValue& value);
			UInt32 RegisterFunctionType(ShaderExpressionType retType, const std::vector<ShaderAst::FunctionParameter>& parameters);
			UInt32 RegisterPointerType(ShaderExpressionType type, SpirvStorageClass storageClass);
			UInt32 RegisterType(ShaderExpressionType type);

			void WriteLocalVariable(std::string name, UInt32 resultId);

			static void MergeBlocks(std::vector<UInt32>& output, const SpirvSection& from);

			struct Context
			{
				const ShaderAst* shader = nullptr;
				const ShaderAst::Function* currentFunction = nullptr;
				const States* states = nullptr;
			};

			struct ExtVar
			{
				UInt32 pointerTypeId;
				UInt32 typeId;
				UInt32 varId;
				std::optional<UInt32> valueId;
			};

			struct State;

			Context m_context;
			Environment m_environment;
			State* m_currentState;
	};
}

#include <Nazara/Shader/SpirvWriter.inl>

#endif
