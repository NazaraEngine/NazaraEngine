// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvAstVisitor.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/Shader/SpirvSection.hpp>
#include <Nazara/Shader/SpirvExpressionLoad.hpp>
#include <Nazara/Shader/SpirvExpressionStore.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
#include <SpirV/GLSL.std.450.h>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	namespace
	{
		template<typename T> const T& Retrieve(const std::unordered_map<std::size_t, T>& map, std::size_t id)
		{
			auto it = map.find(id);
			assert(it != map.end());
			return it->second;
		}
	}

	UInt32 SpirvAstVisitor::AllocateResultId()
	{
		return m_writer.AllocateResultId();
	}

	UInt32 SpirvAstVisitor::EvaluateExpression(ShaderAst::ExpressionPtr& expr)
	{
		expr->Visit(*this);

		assert(m_resultIds.size() == 1);
		return PopResultId();
	}

	auto SpirvAstVisitor::GetVariable(std::size_t varIndex) const -> const Variable&
	{
		return Retrieve(m_variables, varIndex);
	}

	void SpirvAstVisitor::Visit(ShaderAst::AccessIndexExpression& node)
	{
		SpirvExpressionLoad accessMemberVisitor(m_writer, *this, *m_currentBlock);
		PushResultId(accessMemberVisitor.Evaluate(node));
	}

	void SpirvAstVisitor::Visit(ShaderAst::AssignExpression& node)
	{
		UInt32 resultId = EvaluateExpression(node.right);

		SpirvExpressionStore storeVisitor(m_writer, *this, *m_currentBlock);
		storeVisitor.Store(node.left, resultId);

		PushResultId(resultId);
	}

	void SpirvAstVisitor::Visit(ShaderAst::BinaryExpression& node)
	{
		auto RetrieveBaseType = [](const ShaderAst::ExpressionType& exprType)
		{
			if (IsPrimitiveType(exprType))
				return std::get<ShaderAst::PrimitiveType>(exprType);
			else if (IsVectorType(exprType))
				return std::get<ShaderAst::VectorType>(exprType).type;
			else if (IsMatrixType(exprType))
				return std::get<ShaderAst::MatrixType>(exprType).type;
			else
				throw std::runtime_error("unexpected type");
		};

		const ShaderAst::ExpressionType& resultType = GetExpressionType(node);
		const ShaderAst::ExpressionType& leftType = GetExpressionType(*node.left);
		const ShaderAst::ExpressionType& rightType = GetExpressionType(*node.right);

		ShaderAst::PrimitiveType leftTypeBase = RetrieveBaseType(leftType);
		//ShaderAst::PrimitiveType rightTypeBase = RetrieveBaseType(rightType);


		UInt32 leftOperand = EvaluateExpression(node.left);
		UInt32 rightOperand = EvaluateExpression(node.right);
		UInt32 resultId = m_writer.AllocateResultId();

		bool swapOperands = false;

		SpirvOp op = [&]
		{
			switch (node.op)
			{
				case ShaderAst::BinaryType::Add:
				{
					switch (leftTypeBase)
					{
						case ShaderAst::PrimitiveType::Float32:
							return SpirvOp::OpFAdd;

						case ShaderAst::PrimitiveType::Int32:
						case ShaderAst::PrimitiveType::UInt32:
							return SpirvOp::OpIAdd;

						case ShaderAst::PrimitiveType::Boolean:
							break;
					}

					break;
				}

				case ShaderAst::BinaryType::Subtract:
				{
					switch (leftTypeBase)
					{
						case ShaderAst::PrimitiveType::Float32:
							return SpirvOp::OpFSub;

						case ShaderAst::PrimitiveType::Int32:
						case ShaderAst::PrimitiveType::UInt32:
							return SpirvOp::OpISub;

						case ShaderAst::PrimitiveType::Boolean:
							break;
					}

					break;
				}

				case ShaderAst::BinaryType::Divide:
				{
					switch (leftTypeBase)
					{
						case ShaderAst::PrimitiveType::Float32:
							return SpirvOp::OpFDiv;

						case ShaderAst::PrimitiveType::Int32:
							return SpirvOp::OpSDiv;

						case ShaderAst::PrimitiveType::UInt32:
							return SpirvOp::OpUDiv;

						case ShaderAst::PrimitiveType::Boolean:
							break;
					}

					break;
				}

				case ShaderAst::BinaryType::Multiply:
				{
					switch (leftTypeBase)
					{
						case ShaderAst::PrimitiveType::Float32:
						{
							if (IsPrimitiveType(leftType))
							{
								// Handle float * matrix|vector as matrix|vector * float
								if (IsMatrixType(rightType))
								{
									swapOperands = true;
									return SpirvOp::OpMatrixTimesScalar;
								}
								else if (IsVectorType(rightType))
								{
									swapOperands = true;
									return SpirvOp::OpVectorTimesScalar;
								}
							}
							else if (IsPrimitiveType(rightType))
							{
								if (IsMatrixType(leftType))
									return SpirvOp::OpMatrixTimesScalar;
								else if (IsVectorType(leftType))
									return SpirvOp::OpVectorTimesScalar;
							}
							else if (IsMatrixType(leftType))
							{
								if (IsMatrixType(rightType))
									return SpirvOp::OpMatrixTimesMatrix;
								else if (IsVectorType(rightType))
									return SpirvOp::OpMatrixTimesVector;
							}
							else if (IsMatrixType(rightType))
							{
								assert(IsVectorType(leftType));
								return SpirvOp::OpVectorTimesMatrix;
							}

							return SpirvOp::OpFMul;
						}

						case ShaderAst::PrimitiveType::Int32:
						case ShaderAst::PrimitiveType::UInt32:
							return SpirvOp::OpIMul;

						default:
							break;
					}

					break;
				}

				case ShaderAst::BinaryType::CompEq:
				{
					switch (leftTypeBase)
					{
						case ShaderAst::PrimitiveType::Boolean:
							return SpirvOp::OpLogicalEqual;

						case ShaderAst::PrimitiveType::Float32:
							return SpirvOp::OpFOrdEqual;

						case ShaderAst::PrimitiveType::Int32:
						case ShaderAst::PrimitiveType::UInt32:
							return SpirvOp::OpIEqual;
					}

					break;
				}
				
				case ShaderAst::BinaryType::CompGe:
				{
					switch (leftTypeBase)
					{
						case ShaderAst::PrimitiveType::Float32:
							return SpirvOp::OpFOrdGreaterThan;

						case ShaderAst::PrimitiveType::Int32:
							return SpirvOp::OpSGreaterThan;

						case ShaderAst::PrimitiveType::UInt32:
							return SpirvOp::OpUGreaterThan;

						case ShaderAst::PrimitiveType::Boolean:
							break;
					}

					break;
				}
				
				case ShaderAst::BinaryType::CompGt:
				{
					switch (leftTypeBase)
					{
						case ShaderAst::PrimitiveType::Float32:
							return SpirvOp::OpFOrdGreaterThanEqual;

						case ShaderAst::PrimitiveType::Int32:
							return SpirvOp::OpSGreaterThanEqual;

						case ShaderAst::PrimitiveType::UInt32:
							return SpirvOp::OpUGreaterThanEqual;

						case ShaderAst::PrimitiveType::Boolean:
							break;
					}

					break;
				}
				
				case ShaderAst::BinaryType::CompLe:
				{
					switch (leftTypeBase)
					{
						case ShaderAst::PrimitiveType::Float32:
							return SpirvOp::OpFOrdLessThanEqual;

						case ShaderAst::PrimitiveType::Int32:
							return SpirvOp::OpSLessThanEqual;

						case ShaderAst::PrimitiveType::UInt32:
							return SpirvOp::OpULessThanEqual;

						case ShaderAst::PrimitiveType::Boolean:
							break;
					}

					break;
				}
				
				case ShaderAst::BinaryType::CompLt:
				{
					switch (leftTypeBase)
					{
						case ShaderAst::PrimitiveType::Float32:
							return SpirvOp::OpFOrdLessThan;

						case ShaderAst::PrimitiveType::Int32:
							return SpirvOp::OpSLessThan;

						case ShaderAst::PrimitiveType::UInt32:
							return SpirvOp::OpULessThan;

						case ShaderAst::PrimitiveType::Boolean:
							break;
					}

					break;
				}
				
				case ShaderAst::BinaryType::CompNe:
				{
					switch (leftTypeBase)
					{
						case ShaderAst::PrimitiveType::Boolean:
							return SpirvOp::OpLogicalNotEqual;

						case ShaderAst::PrimitiveType::Float32:
							return SpirvOp::OpFOrdNotEqual;

						case ShaderAst::PrimitiveType::Int32:
						case ShaderAst::PrimitiveType::UInt32:
							return SpirvOp::OpINotEqual;
					}

					break;
				}

				case ShaderAst::BinaryType::LogicalAnd:
					return SpirvOp::OpLogicalAnd;

				case ShaderAst::BinaryType::LogicalOr:
					return SpirvOp::OpLogicalOr;
			}

			assert(false);
			throw std::runtime_error("unexpected binary operation");
		}();

		if (swapOperands)
			std::swap(leftOperand, rightOperand);

		if (node.op == ShaderAst::BinaryType::Divide)
		{
			//TODO: Handle other cases
			if (IsVectorType(leftType) && IsPrimitiveType(rightType))
			{
				const ShaderAst::VectorType& leftVec = std::get<ShaderAst::VectorType>(leftType);

				UInt32 vecType = m_writer.GetTypeId(leftType);

				UInt32 rightAsVec = m_writer.AllocateResultId();
				m_currentBlock->AppendVariadic(SpirvOp::OpCompositeConstruct, [&](auto&& append)
				{
					append(vecType);
					append(rightAsVec);

					for (std::size_t i = 0; i < leftVec.componentCount; ++i)
						append(rightOperand);
				});

				rightOperand = rightAsVec;
			}
			else if (leftType != rightType)
				throw std::runtime_error("unexpected division operands");
		}

		m_currentBlock->Append(op, m_writer.GetTypeId(resultType), resultId, leftOperand, rightOperand);
		PushResultId(resultId);
	}

	void SpirvAstVisitor::Visit(ShaderAst::BranchStatement& node)
	{
		assert(!node.condStatements.empty());
		auto& firstCond = node.condStatements.front();

		UInt32 previousConditionId = EvaluateExpression(firstCond.condition);
		SpirvBlock previousContentBlock(m_writer);
		m_currentBlock = &previousContentBlock;

		firstCond.statement->Visit(*this);

		SpirvBlock mergeBlock(m_writer);
		m_functionBlocks.back().Append(SpirvOp::OpSelectionMerge, mergeBlock.GetLabelId(), SpirvSelectionControl::None);

		std::optional<std::size_t> nextBlock;
		for (std::size_t statementIndex = 1; statementIndex < node.condStatements.size(); ++statementIndex)
		{
			auto& statement = node.condStatements[statementIndex];

			SpirvBlock contentBlock(m_writer);

			m_functionBlocks.back().Append(SpirvOp::OpBranchConditional, previousConditionId, previousContentBlock.GetLabelId(), contentBlock.GetLabelId());

			previousConditionId = EvaluateExpression(statement.condition);
			m_functionBlocks.emplace_back(std::move(previousContentBlock));
			previousContentBlock = std::move(contentBlock);

			m_currentBlock = &previousContentBlock;

			statement.statement->Visit(*this);
		}

		if (node.elseStatement)
		{
			SpirvBlock elseBlock(m_writer);

			m_currentBlock = &elseBlock;

			node.elseStatement->Visit(*this);

			elseBlock.Append(SpirvOp::OpBranch, mergeBlock.GetLabelId()); //< FIXME: Shouldn't terminate twice

			m_functionBlocks.back().Append(SpirvOp::OpBranchConditional, previousConditionId, previousContentBlock.GetLabelId(), elseBlock.GetLabelId());
			m_functionBlocks.emplace_back(std::move(previousContentBlock));
			m_functionBlocks.emplace_back(std::move(elseBlock));
		}
		else
		{
			m_functionBlocks.back().Append(SpirvOp::OpBranchConditional, previousConditionId, previousContentBlock.GetLabelId(), mergeBlock.GetLabelId());
			m_functionBlocks.emplace_back(std::move(previousContentBlock));
		}

		m_functionBlocks.emplace_back(std::move(mergeBlock));

		m_currentBlock = &m_functionBlocks.back();
	}

	void SpirvAstVisitor::Visit(ShaderAst::CallFunctionExpression& node)
	{
		assert(std::holds_alternative<std::size_t>(node.targetFunction));
		std::size_t functionIndex = std::get<std::size_t>(node.targetFunction);

		UInt32 funcId = 0;
		for (const auto& [funcIndex, func] : m_funcData)
		{
			if (funcIndex == functionIndex)
			{
				funcId = func.funcId;
				break;
			}
		}
		assert(funcId != 0);

		const FuncData& funcData = Retrieve(m_funcData, m_funcIndex);
		const auto& funcCall = funcData.funcCalls[m_funcCallIndex++];

		StackArray<UInt32> parameterIds = NazaraStackArrayNoInit(UInt32, node.parameters.size());
		for (std::size_t i = 0; i < node.parameters.size(); ++i)
		{
			UInt32 resultId = EvaluateExpression(node.parameters[i]);
			UInt32 varId = funcData.variables[funcCall.firstVarIndex + i].varId;
			m_currentBlock->Append(SpirvOp::OpStore, varId, resultId);

			parameterIds[i] = varId;
		}

		UInt32 resultId = AllocateResultId();
		m_currentBlock->AppendVariadic(SpirvOp::OpFunctionCall, [&](auto&& appender)
		{
			appender(m_writer.GetTypeId(ShaderAst::GetExpressionType(node)));
			appender(resultId);
			appender(funcId);

			for (std::size_t i = 0; i < node.parameters.size(); ++i)
				appender(parameterIds[i]);
		});

		PushResultId(resultId);
	}

	void SpirvAstVisitor::Visit(ShaderAst::CastExpression& node)
	{
		const ShaderAst::ExpressionType& targetExprType = node.targetType;
		if (IsPrimitiveType(targetExprType))
		{
			ShaderAst::PrimitiveType targetType = std::get<ShaderAst::PrimitiveType>(targetExprType);

			assert(node.expressions[0] && !node.expressions[1]);
			ShaderAst::ExpressionPtr& expression = node.expressions[0];

			assert(expression->cachedExpressionType.has_value());
			const ShaderAst::ExpressionType& exprType = expression->cachedExpressionType.value();
			assert(IsPrimitiveType(exprType));
			ShaderAst::PrimitiveType fromType = std::get<ShaderAst::PrimitiveType>(exprType);

			UInt32 fromId = EvaluateExpression(expression);
			if (targetType == fromType)
				return PushResultId(fromId);

			std::optional<SpirvOp> castOp;
			switch (targetType)
			{
				case ShaderAst::PrimitiveType::Boolean:
					throw std::runtime_error("unsupported cast to boolean");

				case ShaderAst::PrimitiveType::Float32:
				{
					switch (fromType)
					{
						case ShaderAst::PrimitiveType::Boolean:
							throw std::runtime_error("unsupported cast from boolean");

						case ShaderAst::PrimitiveType::Float32:
							break; //< Already handled

						case ShaderAst::PrimitiveType::Int32:
							castOp = SpirvOp::OpConvertSToF;
							break;

						case ShaderAst::PrimitiveType::UInt32:
							castOp = SpirvOp::OpConvertUToF;
							break;
					}
					break;
				}

				case ShaderAst::PrimitiveType::Int32:
				{
					switch (fromType)
					{
						case ShaderAst::PrimitiveType::Boolean:
							throw std::runtime_error("unsupported cast from boolean");

						case ShaderAst::PrimitiveType::Float32:
							castOp = SpirvOp::OpConvertFToS;
							break;

						case ShaderAst::PrimitiveType::Int32:
							break; //< Already handled

						case ShaderAst::PrimitiveType::UInt32:
							castOp = SpirvOp::OpSConvert;
							break;
					}
					break;
				}

				case ShaderAst::PrimitiveType::UInt32:
				{
					switch (fromType)
					{
						case ShaderAst::PrimitiveType::Boolean:
							throw std::runtime_error("unsupported cast from boolean");

						case ShaderAst::PrimitiveType::Float32:
							castOp = SpirvOp::OpConvertFToU;
							break;

						case ShaderAst::PrimitiveType::Int32:
							castOp = SpirvOp::OpUConvert;
							break;

						case ShaderAst::PrimitiveType::UInt32:
							break; //< Already handled
					}
					break;
				}
			}

			assert(castOp);

			UInt32 resultId = m_writer.AllocateResultId();
			m_currentBlock->Append(*castOp, m_writer.GetTypeId(targetType), resultId, fromId);

			throw std::runtime_error("toudou");
		}
		else
		{
			assert(IsVectorType(targetExprType));
			StackVector<UInt32> exprResults = NazaraStackVector(UInt32, node.expressions.size());

			for (auto& exprPtr : node.expressions)
			{
				if (!exprPtr)
					break;

				exprResults.push_back(EvaluateExpression(exprPtr));
			}

			UInt32 resultId = m_writer.AllocateResultId();

			m_currentBlock->AppendVariadic(SpirvOp::OpCompositeConstruct, [&](const auto& appender)
			{
				appender(m_writer.GetTypeId(targetExprType));
				appender(resultId);

				for (UInt32 exprResultId : exprResults)
					appender(exprResultId);
			});

			PushResultId(resultId);
		}
	}

	void SpirvAstVisitor::Visit(ShaderAst::ConstantValueExpression& node)
	{
		std::visit([&] (const auto& value)
		{
			PushResultId(m_writer.GetConstantId(value));
		}, node.value);
	}

	void SpirvAstVisitor::Visit(ShaderAst::DeclareConstStatement& /*node*/)
	{
		/* nothing to do */
	}

	void SpirvAstVisitor::Visit(ShaderAst::DeclareExternalStatement& node)
	{
		assert(node.varIndex);

		std::size_t varIndex = *node.varIndex;
		for (auto&& extVar : node.externalVars)
			RegisterExternalVariable(varIndex++, extVar.type);
	}

	void SpirvAstVisitor::Visit(ShaderAst::DeclareFunctionStatement& node)
	{
		assert(node.funcIndex);
		m_funcIndex = *node.funcIndex;
		m_funcCallIndex = 0;

		auto& func = m_funcData[m_funcIndex];

		m_instructions.Append(SpirvOp::OpFunction, func.returnTypeId, func.funcId, 0, func.funcTypeId);

		if (!func.parameters.empty())
		{
			std::size_t varIndex = *node.varIndex;
			for (const auto& param : func.parameters)
			{
				UInt32 paramResultId = m_writer.AllocateResultId();
				m_instructions.Append(SpirvOp::OpFunctionParameter, param.pointerTypeId, paramResultId);

				RegisterVariable(varIndex++, param.typeId, paramResultId, SpirvStorageClass::Function);
			}
		}

		m_functionBlocks.clear();

		m_currentBlock = &m_functionBlocks.emplace_back(m_writer);
		CallOnExit resetCurrentBlock([&] { m_currentBlock = nullptr; });

		for (auto& var : func.variables)
		{
			var.varId = m_writer.AllocateResultId();
			m_currentBlock->Append(SpirvOp::OpVariable, var.typeId, var.varId, SpirvStorageClass::Function);
		}

		if (func.entryPointData)
		{
			auto& entryPointData = *func.entryPointData;
			if (entryPointData.inputStruct)
			{
				auto& inputStruct = *entryPointData.inputStruct;

				std::size_t varIndex = *node.varIndex;

				UInt32 paramId = m_writer.AllocateResultId();
				m_currentBlock->Append(SpirvOp::OpVariable, inputStruct.pointerId, paramId, SpirvStorageClass::Function);

				for (const auto& input : entryPointData.inputs)
				{
					UInt32 resultId = m_writer.AllocateResultId();
					m_currentBlock->Append(SpirvOp::OpAccessChain, input.memberPointerId, resultId, paramId, input.memberIndexConstantId);
					m_currentBlock->Append(SpirvOp::OpCopyMemory, resultId, input.varId);
				}

				RegisterVariable(varIndex, inputStruct.typeId, paramId, SpirvStorageClass::Function);
			}
		}

		for (auto& statementPtr : node.statements)
			statementPtr->Visit(*this);

		// Add implicit return
		if (!m_functionBlocks.back().IsTerminated())
			m_functionBlocks.back().Append(SpirvOp::OpReturn);

		for (SpirvBlock& block : m_functionBlocks)
			m_instructions.AppendSection(block);

		m_instructions.Append(SpirvOp::OpFunctionEnd);
	}

	void SpirvAstVisitor::Visit(ShaderAst::DeclareOptionStatement& /*node*/)
	{
		/* nothing to do */
	}

	void SpirvAstVisitor::Visit(ShaderAst::DeclareStructStatement& node)
	{
		assert(node.structIndex);
		RegisterStruct(*node.structIndex, &node.description);
	}

	void SpirvAstVisitor::Visit(ShaderAst::DeclareVariableStatement& node)
	{
		const auto& func = m_funcData[m_funcIndex];

		UInt32 typeId = m_writer.GetTypeId(node.varType);

		assert(node.varIndex);
		auto varIt = func.varIndexToVarId.find(*node.varIndex);
		UInt32 varId = func.variables[varIt->second].varId;

		RegisterVariable(*node.varIndex, typeId, varId, SpirvStorageClass::Function);

		if (node.initialExpression)
		{
			UInt32 value = EvaluateExpression(node.initialExpression);
			m_currentBlock->Append(SpirvOp::OpStore, varId, value);
		}
	}

	void SpirvAstVisitor::Visit(ShaderAst::DiscardStatement& /*node*/)
	{
		m_currentBlock->Append(SpirvOp::OpKill);
	}

	void SpirvAstVisitor::Visit(ShaderAst::ExpressionStatement& node)
	{
		node.expression->Visit(*this);

		PopResultId();
	}

	void SpirvAstVisitor::Visit(ShaderAst::IntrinsicExpression& node)
	{
		switch (node.intrinsic)
		{
			case ShaderAst::IntrinsicType::DotProduct:
			{
				const ShaderAst::ExpressionType& vecExprType = GetExpressionType(*node.parameters[0]);
				assert(IsVectorType(vecExprType));

				const ShaderAst::VectorType& vecType = std::get<ShaderAst::VectorType>(vecExprType);

				UInt32 typeId = m_writer.GetTypeId(vecType.type);

				UInt32 vec1 = EvaluateExpression(node.parameters[0]);
				UInt32 vec2 = EvaluateExpression(node.parameters[1]);

				UInt32 resultId = m_writer.AllocateResultId();

				m_currentBlock->Append(SpirvOp::OpDot, typeId, resultId, vec1, vec2);
				PushResultId(resultId);
				break;
			}

			case ShaderAst::IntrinsicType::Length:
			{
				UInt32 glslInstructionSet = m_writer.GetExtendedInstructionSet("GLSL.std.450");

				const ShaderAst::ExpressionType& vecExprType = GetExpressionType(*node.parameters[0]);
				assert(IsVectorType(vecExprType));

				const ShaderAst::VectorType& vecType = std::get<ShaderAst::VectorType>(vecExprType);
				UInt32 typeId = m_writer.GetTypeId(vecType.type);

				UInt32 vec = EvaluateExpression(node.parameters[0]);

				UInt32 resultId = m_writer.AllocateResultId();

				m_currentBlock->Append(SpirvOp::OpExtInst, typeId, resultId, glslInstructionSet, GLSLstd450Length, vec);
				PushResultId(resultId);
				break;
			}

			case ShaderAst::IntrinsicType::Max:
			case ShaderAst::IntrinsicType::Min:
			{
				UInt32 glslInstructionSet = m_writer.GetExtendedInstructionSet("GLSL.std.450");

				const ShaderAst::ExpressionType& parameterType = GetExpressionType(*node.parameters[0]);
				assert(IsPrimitiveType(parameterType) || IsVectorType(parameterType));
				UInt32 typeId = m_writer.GetTypeId(parameterType);

				ShaderAst::PrimitiveType basicType;
				if (IsPrimitiveType(parameterType))
					basicType = std::get<ShaderAst::PrimitiveType>(parameterType);
				else if (IsVectorType(parameterType))
					basicType = std::get<ShaderAst::VectorType>(parameterType).type;
				else
					throw std::runtime_error("unexpected expression type");

				GLSLstd450 op;
				switch (basicType)
				{
					case ShaderAst::PrimitiveType::Boolean:
						throw std::runtime_error("unexpected boolean for max/min intrinsic");

					case ShaderAst::PrimitiveType::Float32:
						op = (node.intrinsic == ShaderAst::IntrinsicType::Max) ? GLSLstd450FMax : GLSLstd450FMin;
						break;

					case ShaderAst::PrimitiveType::Int32:
						op = (node.intrinsic == ShaderAst::IntrinsicType::Max) ? GLSLstd450SMax : GLSLstd450SMin;
						break;

					case ShaderAst::PrimitiveType::UInt32:
						op = (node.intrinsic == ShaderAst::IntrinsicType::Max) ? GLSLstd450UMax : GLSLstd450UMin;
						break;
				}

				UInt32 firstParam = EvaluateExpression(node.parameters[0]);
				UInt32 secondParam = EvaluateExpression(node.parameters[1]);
				UInt32 resultId = m_writer.AllocateResultId();

				m_currentBlock->Append(SpirvOp::OpExtInst, typeId, resultId, glslInstructionSet, op, firstParam, secondParam);
				PushResultId(resultId);
				break;
			}

			case ShaderAst::IntrinsicType::Pow:
			{
				UInt32 glslInstructionSet = m_writer.GetExtendedInstructionSet("GLSL.std.450");

				const ShaderAst::ExpressionType& parameterType = GetExpressionType(*node.parameters[0]);
				assert(IsPrimitiveType(parameterType) || IsVectorType(parameterType));
				UInt32 typeId = m_writer.GetTypeId(parameterType);

				ShaderAst::PrimitiveType basicType;
				if (IsPrimitiveType(parameterType))
					basicType = std::get<ShaderAst::PrimitiveType>(parameterType);
				else if (IsVectorType(parameterType))
					basicType = std::get<ShaderAst::VectorType>(parameterType).type;
				else
					throw std::runtime_error("unexpected expression type");

				UInt32 firstParam = EvaluateExpression(node.parameters[0]);
				UInt32 secondParam = EvaluateExpression(node.parameters[1]);
				UInt32 resultId = m_writer.AllocateResultId();

				m_currentBlock->Append(SpirvOp::OpExtInst, typeId, resultId, glslInstructionSet, GLSLstd450Pow, firstParam, secondParam);
				PushResultId(resultId);
				break;
			}

			case ShaderAst::IntrinsicType::SampleTexture:
			{
				UInt32 typeId = m_writer.GetTypeId(ShaderAst::VectorType{4, ShaderAst::PrimitiveType::Float32});

				UInt32 samplerId = EvaluateExpression(node.parameters[0]);
				UInt32 coordinatesId = EvaluateExpression(node.parameters[1]);
				UInt32 resultId = m_writer.AllocateResultId();

				m_currentBlock->Append(SpirvOp::OpImageSampleImplicitLod, typeId, resultId, samplerId, coordinatesId);
				PushResultId(resultId);
				break;
			}

			case ShaderAst::IntrinsicType::CrossProduct:
			default:
				throw std::runtime_error("not yet implemented");
		}
	}

	void SpirvAstVisitor::Visit(ShaderAst::NoOpStatement& /*node*/)
	{
		// nothing to do
	}

	void SpirvAstVisitor::Visit(ShaderAst::MultiStatement& node)
	{
		for (auto& statement : node.statements)
			statement->Visit(*this);
	}

	void SpirvAstVisitor::Visit(ShaderAst::ReturnStatement& node)
	{
		if (node.returnExpr)
		{
			// Handle entry point return
			const auto& func = m_funcData[m_funcIndex];
			if (func.entryPointData)
			{
				auto& entryPointData = *func.entryPointData;
				if (entryPointData.outputStructTypeId)
				{
					UInt32 paramId = EvaluateExpression(node.returnExpr);
					for (const auto& output : entryPointData.outputs)
					{
						UInt32 resultId = m_writer.AllocateResultId();
						m_currentBlock->Append(SpirvOp::OpCompositeExtract, output.typeId, resultId, paramId, output.memberIndex);
						m_currentBlock->Append(SpirvOp::OpStore, output.varId, resultId);
					}
				}

				m_currentBlock->Append(SpirvOp::OpReturn);
			}
			else
				m_currentBlock->Append(SpirvOp::OpReturnValue, EvaluateExpression(node.returnExpr));
		}
		else
			m_currentBlock->Append(SpirvOp::OpReturn);
	}

	void SpirvAstVisitor::Visit(ShaderAst::SwizzleExpression& node)
	{
		UInt32 exprResultId = EvaluateExpression(node.expression);
		UInt32 resultId = m_writer.AllocateResultId();

		const ShaderAst::ExpressionType& targetExprType = GetExpressionType(node);

		if (node.componentCount > 1)
		{
			assert(IsVectorType(targetExprType));

			const ShaderAst::VectorType& targetType = std::get<ShaderAst::VectorType>(targetExprType);

			// Swizzling is implemented via SpirvOp::OpVectorShuffle using the same vector twice as operands
			m_currentBlock->AppendVariadic(SpirvOp::OpVectorShuffle, [&](const auto& appender)
			{
				appender(m_writer.GetTypeId(targetType));
				appender(resultId);
				appender(exprResultId);
				appender(exprResultId);

				for (std::size_t i = 0; i < node.componentCount; ++i)
					appender(UInt32(node.components[i]));
			});
		}
		else
		{
			assert(IsPrimitiveType(targetExprType));

			ShaderAst::PrimitiveType targetType = std::get<ShaderAst::PrimitiveType>(targetExprType);

			// Extract a single component from the vector
			assert(node.componentCount == 1);

			m_currentBlock->Append(SpirvOp::OpCompositeExtract, m_writer.GetTypeId(targetType), resultId, exprResultId, UInt32(node.components[0]) - UInt32(ShaderAst::SwizzleComponent::First) );
		}

		PushResultId(resultId);
	}

	void SpirvAstVisitor::Visit(ShaderAst::VariableExpression& node)
	{
		SpirvExpressionLoad loadVisitor(m_writer, *this, *m_currentBlock);
		PushResultId(loadVisitor.Evaluate(node));
	}

	void SpirvAstVisitor::Visit(ShaderAst::UnaryExpression& node)
	{
		const ShaderAst::ExpressionType& resultType = GetExpressionType(node);
		const ShaderAst::ExpressionType& exprType = GetExpressionType(*node.expression);

		UInt32 operand = EvaluateExpression(node.expression);

		UInt32 resultId = [&]
		{
			switch (node.op)
			{
				case ShaderAst::UnaryType::LogicalNot:
				{
					assert(IsPrimitiveType(exprType));
					assert(std::get<ShaderAst::PrimitiveType>(resultType) == ShaderAst::PrimitiveType::Boolean);

					UInt32 resultId = m_writer.AllocateResultId();
					m_currentBlock->Append(SpirvOp::OpLogicalNot, m_writer.GetTypeId(resultType), resultId, operand);

					return resultId;
				}

				case ShaderAst::UnaryType::Minus:
				{
					ShaderAst::PrimitiveType basicType;
					if (IsPrimitiveType(exprType))
						basicType = std::get<ShaderAst::PrimitiveType>(exprType);
					else if (IsVectorType(exprType))
						basicType = std::get<ShaderAst::VectorType>(exprType).type;
					else
						throw std::runtime_error("unexpected expression type");

					UInt32 resultId = m_writer.AllocateResultId();

					switch (basicType)
					{
						case ShaderAst::PrimitiveType::Float32:
							m_currentBlock->Append(SpirvOp::OpFNegate, m_writer.GetTypeId(resultType), resultId, operand);
							return resultId;

						case ShaderAst::PrimitiveType::Int32:
						case ShaderAst::PrimitiveType::UInt32:
							m_currentBlock->Append(SpirvOp::OpSNegate, m_writer.GetTypeId(resultType), resultId, operand);
							return resultId;

						default:
							break;
					}
				}

				case ShaderAst::UnaryType::Plus:
					PushResultId(operand); //< No-op
					break;
			}

			throw std::runtime_error("unexpected unary operation");
		}();

		PushResultId(resultId);
	}

	void SpirvAstVisitor::PushResultId(UInt32 value)
	{
		m_resultIds.push_back(value);
	}

	UInt32 SpirvAstVisitor::PopResultId()
	{
		if (m_resultIds.empty())
			throw std::runtime_error("invalid operation");

		UInt32 resultId = m_resultIds.back();
		m_resultIds.pop_back();

		return resultId;
	}
}
