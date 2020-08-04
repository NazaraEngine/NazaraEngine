// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderAstValidator.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Renderer/ShaderAst.hpp>
#include <Nazara/Renderer/ShaderVariables.hpp>
#include <vector>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	struct AstError
	{
		std::string errMsg;
	};

	struct ShaderAstValidator::Context
	{
		struct Local
		{
			std::string name;
			ShaderExpressionType type;
		};

		const ShaderAst::Function* currentFunction;
		std::vector<Local> declaredLocals;
		std::vector<std::size_t> blockLocalIndex;
	};

	bool ShaderAstValidator::Validate(std::string* error)
	{
		try
		{
			for (std::size_t i = 0; i < m_shader.GetFunctionCount(); ++i)
			{
				const auto& func = m_shader.GetFunction(i);

				Context currentContext;
				currentContext.currentFunction = &func;

				m_context = &currentContext;
				CallOnExit resetContext([&] { m_context = nullptr; });

				func.statement->Visit(*this);
			}

			return true;
		}
		catch (const AstError& e)
		{
			if (error)
				*error = e.errMsg;

			return false;
		}
	}

	const ShaderNodes::ExpressionPtr& ShaderAstValidator::MandatoryExpr(const ShaderNodes::ExpressionPtr& node)
	{
		MandatoryNode(node);

		return node;
	}

	const ShaderNodes::NodePtr& ShaderAstValidator::MandatoryNode(const ShaderNodes::NodePtr& node)
	{
		if (!node)
			throw AstError{ "Invalid node" };

		return node;
	}

	void ShaderAstValidator::TypeMustMatch(const ShaderNodes::ExpressionPtr& left, const ShaderNodes::ExpressionPtr& right)
	{
		return TypeMustMatch(left->GetExpressionType(), right->GetExpressionType());
	}

	void ShaderAstValidator::TypeMustMatch(const ShaderExpressionType& left, const ShaderExpressionType& right)
	{
		if (left != right)
			throw AstError{ "Left expression type must match right expression type" };
	}

	void ShaderAstValidator::Visit(const ShaderNodes::AccessMember& node)
	{
		const ShaderExpressionType& exprType = MandatoryExpr(node.structExpr)->GetExpressionType();
		if (!std::holds_alternative<std::string>(exprType))
			throw AstError{ "expression is not a structure" };

		const std::string& structName = std::get<std::string>(exprType);

		const auto& structs = m_shader.GetStructs();
		auto it = std::find_if(structs.begin(), structs.end(), [&](const auto& s) { return s.name == structName; });
		if (it == structs.end())
			throw AstError{ "invalid structure" };

		const ShaderAst::Struct& s = *it;
		if (node.memberIndex >= s.members.size())
			throw AstError{ "member index out of bounds" };

		const auto& member = s.members[node.memberIndex];
		if (member.type != node.exprType)
			throw AstError{ "member type does not match node type" };
	}

	void ShaderAstValidator::Visit(const ShaderNodes::AssignOp& node)
	{
		MandatoryNode(node.left);
		MandatoryNode(node.right);
		TypeMustMatch(node.left, node.right);

		if (node.left->GetExpressionCategory() != ShaderNodes::ExpressionCategory::LValue)
			throw AstError { "Assignation is only possible with a l-value" };

		ShaderAstRecursiveVisitor::Visit(node);
	}

	void ShaderAstValidator::Visit(const ShaderNodes::BinaryOp& node)
	{
		MandatoryNode(node.left);
		MandatoryNode(node.right);

		const ShaderExpressionType& leftExprType = MandatoryExpr(node.left)->GetExpressionType();
		if (!std::holds_alternative<ShaderNodes::BasicType>(leftExprType))
			throw AstError{ "left expression type does not support binary operation" };

		const ShaderExpressionType& rightExprType = MandatoryExpr(node.right)->GetExpressionType();
		if (!std::holds_alternative<ShaderNodes::BasicType>(rightExprType))
			throw AstError{ "right expression type does not support binary operation" };

		ShaderNodes::BasicType leftType = std::get<ShaderNodes::BasicType>(leftExprType);
		ShaderNodes::BasicType rightType = std::get<ShaderNodes::BasicType>(rightExprType);

		switch (node.op)
		{
			case ShaderNodes::BinaryType::Add:
			case ShaderNodes::BinaryType::Equality:
			case ShaderNodes::BinaryType::Substract:
				TypeMustMatch(node.left, node.right);
				break;

			case ShaderNodes::BinaryType::Multiply:
			case ShaderNodes::BinaryType::Divide:
			{
				switch (leftType)
				{
					case ShaderNodes::BasicType::Float1:
					{
						if (ShaderNodes::Node::GetComponentType(rightType) != ShaderNodes::BasicType::Float1)
							throw AstError{ "Left expression type is not compatible with right expression type" };

						break;
					}

					case ShaderNodes::BasicType::Float2:
					case ShaderNodes::BasicType::Float3:
					case ShaderNodes::BasicType::Float4:
					{
						if (leftType != rightType && rightType != ShaderNodes::BasicType::Float1)
							throw AstError{ "Left expression type is not compatible with right expression type" };

						break;
					}

					case ShaderNodes::BasicType::Mat4x4:
					{
						switch (rightType)
						{
							case ShaderNodes::BasicType::Float1:
							case ShaderNodes::BasicType::Float4:
							case ShaderNodes::BasicType::Mat4x4:
								break;

							default:
								TypeMustMatch(node.left, node.right);
						}

						break;
					}

					default:
						TypeMustMatch(node.left, node.right);
				}
			}
		}

		ShaderAstRecursiveVisitor::Visit(node);
	}

	void ShaderAstValidator::Visit(const ShaderNodes::Branch& node)
	{
		for (const auto& condStatement : node.condStatements)
		{
			MandatoryNode(condStatement.condition);
			MandatoryNode(condStatement.statement);
		}

		ShaderAstRecursiveVisitor::Visit(node);
	}

	void ShaderAstValidator::Visit(const ShaderNodes::Cast& node)
	{
		unsigned int componentCount = 0;
		unsigned int requiredComponents = node.GetComponentCount(node.exprType);
		for (const auto& exprPtr : node.expressions)
		{
			if (!exprPtr)
				break;

			const ShaderExpressionType& exprType = exprPtr->GetExpressionType();
			if (!std::holds_alternative<ShaderNodes::BasicType>(exprType))
				throw AstError{ "incompatible type" };

			componentCount += node.GetComponentCount(std::get<ShaderNodes::BasicType>(exprType));
		}

		if (componentCount != requiredComponents)
			throw AstError{ "Component count doesn't match required component count" };

		ShaderAstRecursiveVisitor::Visit(node);
	}

	void ShaderAstValidator::Visit(const ShaderNodes::Constant& /*node*/)
	{
	}

	void ShaderAstValidator::Visit(const ShaderNodes::DeclareVariable& node)
	{
		assert(m_context);

		auto& local = m_context->declaredLocals.emplace_back();
		local.name = node.variable->name;
		local.type = node.variable->type;

		ShaderAstRecursiveVisitor::Visit(node);
	}

	void ShaderAstValidator::Visit(const ShaderNodes::ExpressionStatement& node)
	{
		MandatoryNode(node.expression);

		ShaderAstRecursiveVisitor::Visit(node);
	}

	void ShaderAstValidator::Visit(const ShaderNodes::Identifier& node)
	{
		assert(m_context);

		if (!node.var)
			throw AstError{ "Invalid variable" };

		Visit(node.var);
	}

	void ShaderAstValidator::Visit(const ShaderNodes::IntrinsicCall& node)
	{
		switch (node.intrinsic)
		{
			case ShaderNodes::IntrinsicType::CrossProduct:
			case ShaderNodes::IntrinsicType::DotProduct:
			{
				if (node.parameters.size() != 2)
					throw AstError { "Expected 2 parameters" };

				for (auto& param : node.parameters)
					MandatoryNode(param);

				ShaderExpressionType type = node.parameters.front()->GetExpressionType();
				for (std::size_t i = 1; i < node.parameters.size(); ++i)
				{
					if (type != node.parameters[i]->GetExpressionType())
						throw AstError{ "All type must match" };
				}

				break;
			}
		}

		switch (node.intrinsic)
		{
			case ShaderNodes::IntrinsicType::CrossProduct:
			{
				if (node.parameters[0]->GetExpressionType() != ShaderExpressionType{ ShaderNodes::BasicType::Float3 })
					throw AstError{ "CrossProduct only works with Float3 expressions" };

				break;
			}

			case ShaderNodes::IntrinsicType::DotProduct:
				break;
		}

		ShaderAstRecursiveVisitor::Visit(node);
	}

	void ShaderAstValidator::Visit(const ShaderNodes::Sample2D& node)
	{
		if (MandatoryExpr(node.sampler)->GetExpressionType() != ShaderExpressionType{ ShaderNodes::BasicType::Sampler2D })
			throw AstError{ "Sampler must be a Sampler2D" };

		if (MandatoryExpr(node.coordinates)->GetExpressionType() != ShaderExpressionType{ ShaderNodes::BasicType::Float2 })
			throw AstError{ "Coordinates must be a Float2" };

		ShaderAstRecursiveVisitor::Visit(node);
	}

	void ShaderAstValidator::Visit(const ShaderNodes::StatementBlock& node)
	{
		assert(m_context);

		m_context->blockLocalIndex.push_back(m_context->declaredLocals.size());

		for (const auto& statement : node.statements)
			MandatoryNode(statement);

		assert(m_context->declaredLocals.size() >= m_context->blockLocalIndex.back());
		m_context->declaredLocals.resize(m_context->blockLocalIndex.back());
		m_context->blockLocalIndex.pop_back();

		ShaderAstRecursiveVisitor::Visit(node);
	}

	void ShaderAstValidator::Visit(const ShaderNodes::SwizzleOp& node)
	{
		if (node.componentCount > 4)
			throw AstError{ "Cannot swizzle more than four elements" };

		const ShaderExpressionType& exprType = MandatoryExpr(node.expression)->GetExpressionType();
		if (!std::holds_alternative<ShaderNodes::BasicType>(exprType))
			throw AstError{ "Cannot swizzle this type" };

		switch (std::get<ShaderNodes::BasicType>(exprType))
		{
			case ShaderNodes::BasicType::Float1:
			case ShaderNodes::BasicType::Float2:
			case ShaderNodes::BasicType::Float3:
			case ShaderNodes::BasicType::Float4:
				break;

			default:
				throw AstError{ "Cannot swizzle this type" };
		}

		ShaderAstRecursiveVisitor::Visit(node);
	}

	void ShaderAstValidator::Visit(const ShaderNodes::BuiltinVariable& /*var*/)
	{
		/* Nothing to do */
	}

	void ShaderAstValidator::Visit(const ShaderNodes::InputVariable& var)
	{
		for (std::size_t i = 0; i < m_shader.GetInputCount(); ++i)
		{
			const auto& input = m_shader.GetInput(i);
			if (input.name == var.name)
			{
				TypeMustMatch(input.type, var.type);
				return;
			}
		}

		throw AstError{ "Input not found" };
	}

	void ShaderAstValidator::Visit(const ShaderNodes::LocalVariable& var)
	{
		const auto& vars = m_context->declaredLocals;

		auto it = std::find_if(vars.begin(), vars.end(), [&](const auto& v) { return v.name == var.name; });
		if (it == vars.end())
			throw AstError{ "Local variable not found in this block" };

		TypeMustMatch(it->type, var.type);
	}

	void ShaderAstValidator::Visit(const ShaderNodes::OutputVariable& var)
	{
		for (std::size_t i = 0; i < m_shader.GetOutputCount(); ++i)
		{
			const auto& input = m_shader.GetOutput(i);
			if (input.name == var.name)
			{
				TypeMustMatch(input.type, var.type);
				return;
			}
		}

		throw AstError{ "Output not found" };
	}

	void ShaderAstValidator::Visit(const ShaderNodes::ParameterVariable& var)
	{
		assert(m_context->currentFunction);

		const auto& parameters = m_context->currentFunction->parameters;

		auto it = std::find_if(parameters.begin(), parameters.end(), [&](const auto& parameter) { return parameter.name == var.name; });
		if (it == parameters.end())
			throw AstError{ "Parameter not found in function" };

		TypeMustMatch(it->type, var.type);
	}

	void ShaderAstValidator::Visit(const ShaderNodes::UniformVariable& var)
	{
		for (std::size_t i = 0; i < m_shader.GetUniformCount(); ++i)
		{
			const auto& uniform = m_shader.GetUniform(i);
			if (uniform.name == var.name)
			{
				TypeMustMatch(uniform.type, var.type);
				return;
			}
		}

		throw AstError{ "Uniform not found" };
	}

	bool ValidateShader(const ShaderAst& shader, std::string* error)
	{
		ShaderAstValidator validator(shader);
		return validator.Validate(error);
	}
}
