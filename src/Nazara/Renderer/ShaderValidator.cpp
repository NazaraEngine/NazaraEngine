// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderValidator.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz::ShaderAst
{
	struct AstError
	{
		std::string errMsg;
	};

	bool ShaderValidator::Validate(const StatementPtr& shader, std::string* error)
	{
		try
		{
			shader->Visit(*this);
			return true;
		}
		catch (const AstError& e)
		{
			if (error)
				*error = e.errMsg;

			return false;
		}
	}

	const ExpressionPtr& ShaderValidator::MandatoryExpr(const ExpressionPtr& node)
	{
		MandatoryNode(node);

		return node;
	}

	const NodePtr& ShaderValidator::MandatoryNode(const NodePtr& node)
	{
		if (!node)
			throw AstError{ "Invalid node" };

		return node;
	}

	void ShaderValidator::TypeMustMatch(const ExpressionPtr& left, const ExpressionPtr& right)
	{
		if (left->GetExpressionType() != right->GetExpressionType())
			throw AstError{ "Left expression type must match right expression type" };
	}

	void ShaderValidator::Visit(const AssignOp& node)
	{
		MandatoryNode(node.left);
		MandatoryNode(node.right);
		TypeMustMatch(node.left, node.right);

		Visit(node.left);
		Visit(node.right);
	}

	void ShaderValidator::Visit(const BinaryFunc& node)
	{
		MandatoryNode(node.left);
		MandatoryNode(node.right);
		TypeMustMatch(node.left, node.right);

		switch (node.intrinsic)
		{
			case BinaryIntrinsic::CrossProduct:
			{
				if (node.left->GetExpressionType() != ExpressionType::Float3)
					throw AstError{ "CrossProduct only works with Float3 expressions" };
			}

			case BinaryIntrinsic::DotProduct:
				break;
		}

		Visit(node.left);
		Visit(node.right);
	}

	void ShaderValidator::Visit(const BinaryOp& node)
	{
		MandatoryNode(node.left);
		MandatoryNode(node.right);

		ExpressionType leftType = node.left->GetExpressionType();
		ExpressionType rightType = node.right->GetExpressionType();

		switch (node.op)
		{
			case BinaryType::Add:
			case BinaryType::Equality:
			case BinaryType::Substract:
				TypeMustMatch(node.left, node.right);
				break;

			case BinaryType::Multiply:
			case BinaryType::Divide:
			{
				switch (leftType)
				{
					case ExpressionType::Float2:
					case ExpressionType::Float3:
					case ExpressionType::Float4:
					{
						if (leftType != rightType && rightType != ExpressionType::Float1)
							throw AstError{ "Left expression type is not compatible with right expression type" };

						break;
					}

					case ExpressionType::Mat4x4:
					{
						switch (rightType)
						{
							case ExpressionType::Float1:
							case ExpressionType::Float4:
							case ExpressionType::Mat4x4:
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

		Visit(node.left);
		Visit(node.right);
	}

	void ShaderValidator::Visit(const Branch& node)
	{
		for (const auto& condStatement : node.condStatements)
		{
			Visit(MandatoryNode(condStatement.condition));
			Visit(MandatoryNode(condStatement.statement));
		}
	}

	void ShaderValidator::Visit(const BuiltinVariable& /*node*/)
	{
	}

	void ShaderValidator::Visit(const Cast& node)
	{
		unsigned int componentCount = 0;
		unsigned int requiredComponents = node.GetComponentCount(node.exprType);
		for (const auto& exprPtr : node.expressions)
		{
			if (!exprPtr)
				break;

			componentCount += node.GetComponentCount(exprPtr->GetExpressionType());
			Visit(exprPtr);
		}

		if (componentCount != requiredComponents)
			throw AstError{ "Component count doesn't match required component count" };
	}

	void ShaderValidator::Visit(const Constant& /*node*/)
	{
	}

	void ShaderValidator::Visit(const DeclareVariable& node)
	{
		Visit(MandatoryNode(node.expression));
	}

	void ShaderValidator::Visit(const ExpressionStatement& node)
	{
		Visit(MandatoryNode(node.expression));
	}

	void ShaderValidator::Visit(const NamedVariable& node)
	{
		if (node.name.empty())
			throw AstError{ "Variable has empty name" };
	}

	void ShaderValidator::Visit(const Sample2D& node)
	{
		if (MandatoryExpr(node.sampler)->GetExpressionType() != ExpressionType::Sampler2D)
			throw AstError{ "Sampler must be a Sampler2D" };

		if (MandatoryExpr(node.coordinates)->GetExpressionType() != ExpressionType::Float2)
			throw AstError{ "Coordinates must be a Float2" };

		Visit(node.sampler);
		Visit(node.coordinates);
	}

	void ShaderValidator::Visit(const StatementBlock& node)
	{
		for (const auto& statement : node.statements)
			Visit(MandatoryNode(statement));
	}

	void ShaderValidator::Visit(const SwizzleOp& node)
	{
		if (node.componentCount > 4)
			throw AstError{ "Cannot swizzle more than four elements" };

		switch (MandatoryExpr(node.expression)->GetExpressionType())
		{
			case ExpressionType::Float1:
			case ExpressionType::Float2:
			case ExpressionType::Float3:
			case ExpressionType::Float4:
				break;

			default:
				throw AstError{ "Cannot swizzle this type" };
		}

		Visit(node.expression);
	}

	bool Validate(const StatementPtr& shader, std::string* error)
	{
		ShaderValidator validator;
		return validator.Validate(shader, error);
	}
}
