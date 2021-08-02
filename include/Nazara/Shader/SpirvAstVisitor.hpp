// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPIRVASTVISITOR_HPP
#define NAZARA_SPIRVASTVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/SpirvBlock.hpp>
#include <Nazara/Shader/Ast/AstExpressionVisitorExcept.hpp>
#include <Nazara/Shader/Ast/AstStatementVisitorExcept.hpp>
#include <Nazara/Shader/Ast/ExpressionType.hpp>
#include <unordered_map>
#include <vector>

namespace Nz
{
	class SpirvWriter;

	class NAZARA_SHADER_API SpirvAstVisitor : public ShaderAst::ExpressionVisitorExcept, public ShaderAst::StatementVisitorExcept
	{
		public:
			struct EntryPoint;
			struct FuncData;
			struct Variable;

			inline SpirvAstVisitor(SpirvWriter& writer, SpirvSection& instructions, std::unordered_map<std::size_t, FuncData>& funcData);
			SpirvAstVisitor(const SpirvAstVisitor&) = delete;
			SpirvAstVisitor(SpirvAstVisitor&&) = delete;
			~SpirvAstVisitor() = default;

			UInt32 AllocateResultId();

			UInt32 EvaluateExpression(ShaderAst::ExpressionPtr& expr);

			const Variable& GetVariable(std::size_t varIndex) const;

			using ExpressionVisitorExcept::Visit;
			using StatementVisitorExcept::Visit;

			void Visit(ShaderAst::AccessIndexExpression& node) override;
			void Visit(ShaderAst::AssignExpression& node) override;
			void Visit(ShaderAst::BinaryExpression& node) override;
			void Visit(ShaderAst::BranchStatement& node) override;
			void Visit(ShaderAst::CallFunctionExpression& node) override;
			void Visit(ShaderAst::CastExpression& node) override;
			void Visit(ShaderAst::ConstantValueExpression& node) override;
			void Visit(ShaderAst::DeclareConstStatement& node) override;
			void Visit(ShaderAst::DeclareExternalStatement& node) override;
			void Visit(ShaderAst::DeclareFunctionStatement& node) override;
			void Visit(ShaderAst::DeclareOptionStatement& node) override;
			void Visit(ShaderAst::DeclareStructStatement& node) override;
			void Visit(ShaderAst::DeclareVariableStatement& node) override;
			void Visit(ShaderAst::DiscardStatement& node) override;
			void Visit(ShaderAst::ExpressionStatement& node) override;
			void Visit(ShaderAst::IntrinsicExpression& node) override;
			void Visit(ShaderAst::MultiStatement& node) override;
			void Visit(ShaderAst::NoOpStatement& node) override;
			void Visit(ShaderAst::ReturnStatement& node) override;
			void Visit(ShaderAst::SwizzleExpression& node) override;
			void Visit(ShaderAst::VariableExpression& node) override;
			void Visit(ShaderAst::UnaryExpression& node) override;

			SpirvAstVisitor& operator=(const SpirvAstVisitor&) = delete;
			SpirvAstVisitor& operator=(SpirvAstVisitor&&) = delete;

			struct EntryPoint
			{
				struct Input
				{
					UInt32 memberIndexConstantId;
					UInt32 memberPointerId;
					UInt32 varId;
				};

				struct Output
				{
					Int32 memberIndex;
					UInt32 typeId;
					UInt32 varId;
				};

				struct InputStruct
				{
					UInt32 pointerId;
					UInt32 typeId;
				};

				ShaderStageType stageType;
				std::optional<InputStruct> inputStruct;
				std::optional<UInt32> outputStructTypeId;
				std::vector<Input> inputs;
				std::vector<Output> outputs;
				std::vector<SpirvExecutionMode> executionModes;
			};

			struct FuncData
			{
				std::optional<EntryPoint> entryPointData;

				struct FuncCall
				{
					std::size_t firstVarIndex;
				};

				struct Parameter
				{
					UInt32 pointerTypeId;
					UInt32 typeId;
				};

				struct Variable
				{
					UInt32 typeId;
					UInt32 varId;
				};

				std::size_t funcIndex;
				std::string name;
				std::vector<FuncCall> funcCalls;
				std::vector<Parameter> parameters;
				std::vector<Variable> variables;
				std::unordered_map<std::size_t, std::size_t> varIndexToVarId;
				UInt32 funcId;
				UInt32 funcTypeId;
				UInt32 returnTypeId;
			};

			struct Variable
			{
				SpirvStorageClass storage;
				UInt32 pointerId;
				UInt32 pointedTypeId;
			};

		private:
			void PushResultId(UInt32 value);
			UInt32 PopResultId();

			inline void RegisterExternalVariable(std::size_t varIndex, const ShaderAst::ExpressionType& type);
			inline void RegisterStruct(std::size_t structIndex, ShaderAst::StructDescription* structDesc);
			inline void RegisterVariable(std::size_t varIndex, UInt32 typeId, UInt32 pointerId, SpirvStorageClass storageClass);

			std::size_t m_extVarIndex;
			std::size_t m_funcCallIndex;
			std::size_t m_funcIndex;
			std::unordered_map<std::size_t, FuncData>& m_funcData;
			std::unordered_map<std::size_t, ShaderAst::StructDescription*> m_structs;
			std::unordered_map<std::size_t, Variable> m_variables;
			std::vector<std::size_t> m_scopeSizes;
			std::vector<SpirvBlock> m_functionBlocks;
			std::vector<UInt32> m_resultIds;
			SpirvBlock* m_currentBlock;
			SpirvSection& m_instructions;
			SpirvWriter& m_writer;
	};
}

#include <Nazara/Shader/SpirvAstVisitor.inl>

#endif
