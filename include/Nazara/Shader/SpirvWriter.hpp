// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVWRITER_HPP
#define NAZARA_SPIRVWRITER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/ConstantValue.hpp>
#include <Nazara/Shader/Ast/Nodes.hpp>
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
		friend class SpirvBlock;
		friend class SpirvExpressionLoad;
		friend class SpirvExpressionStore;

		public:
			struct Environment;

			SpirvWriter();
			SpirvWriter(const SpirvWriter&) = delete;
			SpirvWriter(SpirvWriter&&) = delete;
			~SpirvWriter() = default;

			std::vector<UInt32> Generate(ShaderAst::StatementPtr& shader, const States& states = {});

			void SetEnv(Environment environment);

			struct Environment
			{
				UInt32 spvMajorVersion = 1;
				UInt32 spvMinorVersion = 0;
			};

		private:
			struct FunctionParameter;
			struct OnlyCache {};

			UInt32 AllocateResultId();

			void AppendHeader();

			SpirvConstantCache::TypePtr BuildFunctionType(const ShaderAst::DeclareFunctionStatement& functionNode);

			UInt32 GetConstantId(const ShaderAst::ConstantValue& value) const;
			UInt32 GetExtVarPointerId(std::size_t varIndex) const;
			UInt32 GetFunctionTypeId(const ShaderAst::DeclareFunctionStatement& functionNode);
			UInt32 GetPointerTypeId(const ShaderAst::ExpressionType& type, SpirvStorageClass storageClass) const;
			UInt32 GetTypeId(const ShaderAst::ExpressionType& type) const;

			bool IsOptionEnabled(std::size_t optionIndex) const;

			UInt32 RegisterConstant(const ShaderAst::ConstantValue& value);
			UInt32 RegisterFunctionType(const ShaderAst::DeclareFunctionStatement& functionNode);
			UInt32 RegisterPointerType(ShaderAst::ExpressionType type, SpirvStorageClass storageClass);
			UInt32 RegisterType(ShaderAst::ExpressionType type);

			static void MergeSections(std::vector<UInt32>& output, const SpirvSection& from);

			struct Context
			{
				const States* states = nullptr;
			};

			struct State;

			Context m_context;
			Environment m_environment;
			State* m_currentState;
	};
}

#include <Nazara/Shader/SpirvWriter.inl>

#endif
