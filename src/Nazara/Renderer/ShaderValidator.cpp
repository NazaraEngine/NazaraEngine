// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderValidator.hpp>
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

	struct ShaderValidator::Context
	{
		struct Local
		{
			std::string name;
			ShaderNodes::ExpressionType type;
		};

		const ShaderAst::Function* currentFunction;
		std::vector<Local> declaredLocals;
		std::vector<std::size_t> blockLocalIndex;
	};

	bool ShaderValidator::Validate(std::string* error)
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

	const ShaderNodes::ExpressionPtr& ShaderValidator::MandatoryExpr(const ShaderNodes::ExpressionPtr& node)
	{
		MandatoryNode(node);

		return node;
	}

	const ShaderNodes::NodePtr& ShaderValidator::MandatoryNode(const ShaderNodes::NodePtr& node)
	{
		if (!node)
			throw AstError{ "Invalid node" };

		return node;
	}

	void ShaderValidator::TypeMustMatch(const ShaderNodes::ExpressionPtr& left, const ShaderNodes::ExpressionPtr& right)
	{
		return TypeMustMatch(left->GetExpressionType(), right->GetExpressionType());
	}

	void ShaderValidator::TypeMustMatch(ShaderNodes::ExpressionType left, ShaderNodes::ExpressionType right)
	{
		if (left != right)
			throw AstError{ "Left expression type must match right expression type" };
	}

	void ShaderValidator::Visit(const ShaderNodes::AssignOp& node)
	{
		MandatoryNode(node.left);
		MandatoryNode(node.right);
		TypeMustMatch(node.left, node.right);

		if (node.left->GetExpressionCategory() != ShaderNodes::ExpressionCategory::LValue)
			throw AstError { "Assignation is only possible with a l-value" };

		Visit(node.left);
		Visit(node.right);
	}

	void ShaderValidator::Visit(const ShaderNodes::BinaryOp& node)
	{
		MandatoryNode(node.left);
		MandatoryNode(node.right);

		ShaderNodes::ExpressionType leftType = node.left->GetExpressionType();
		ShaderNodes::ExpressionType rightType = node.right->GetExpressionType();

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
					case ShaderNodes::ExpressionType::Float1:
					{
						if (ShaderNodes::Node::GetComponentType(rightType) != ShaderNodes::ExpressionType::Float1)
							throw AstError{ "Left expression type is not compatible with right expression type" };

						break;
					}

					case ShaderNodes::ExpressionType::Float2:
					case ShaderNodes::ExpressionType::Float3:
					case ShaderNodes::ExpressionType::Float4:
					{
						if (leftType != rightType && rightType != ShaderNodes::ExpressionType::Float1)
							throw AstError{ "Left expression type is not compatible with right expression type" };

						break;
					}

					case ShaderNodes::ExpressionType::Mat4x4:
					{
						switch (rightType)
						{
							case ShaderNodes::ExpressionType::Float1:
							case ShaderNodes::ExpressionType::Float4:
							case ShaderNodes::ExpressionType::Mat4x4:
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

	void ShaderValidator::Visit(const ShaderNodes::Branch& node)
	{
		for (const auto& condStatement : node.condStatements)
		{
			Visit(MandatoryNode(condStatement.condition));
			Visit(MandatoryNode(condStatement.statement));
		}
	}

	void ShaderValidator::Visit(const ShaderNodes::Cast& node)
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

	void ShaderValidator::Visit(const ShaderNodes::Constant& /*node*/)
	{
	}

	void ShaderValidator::Visit(const ShaderNodes::DeclareVariable& node)
	{
		assert(m_context);

		if (node.expression)
			Visit(node.expression);

		auto& local = m_context->declaredLocals.emplace_back();
		local.name = node.variable->name;
		local.type = node.variable->type;
	}

	void ShaderValidator::Visit(const ShaderNodes::ExpressionStatement& node)
	{
		Visit(MandatoryNode(node.expression));
	}

	void ShaderValidator::Visit(const ShaderNodes::Identifier& node)
	{
		assert(m_context);

		if (!node.var)
			throw AstError{ "Invalid variable" };

		//< FIXME: Use variable visitor
		switch (node.var->GetType())
		{
			case ShaderNodes::VariableType::BuiltinVariable:
				break;

			case ShaderNodes::VariableType::InputVariable:
			{
				auto& namedVar = static_cast<ShaderNodes::InputVariable&>(*node.var);

				for (std::size_t i = 0; i < m_shader.GetInputCount(); ++i)
				{
					const auto& input = m_shader.GetInput(i);
					if (input.name == namedVar.name)
					{
						TypeMustMatch(input.type, namedVar.type);
						return;
					}
				}

				throw AstError{ "Input not found" };
			}

			case ShaderNodes::VariableType::LocalVariable:
			{
				auto& localVar = static_cast<ShaderNodes::LocalVariable&>(*node.var);
				const auto& vars = m_context->declaredLocals;

				auto it = std::find_if(vars.begin(), vars.end(), [&](const auto& var) { return var.name == localVar.name; });
				if (it == vars.end())
					throw AstError{ "Local variable not found in this block" };

				TypeMustMatch(it->type, localVar.type);
				break;
			}

			case ShaderNodes::VariableType::OutputVariable:
			{
				auto& outputVar = static_cast<ShaderNodes::OutputVariable&>(*node.var);

				for (std::size_t i = 0; i < m_shader.GetOutputCount(); ++i)
				{
					const auto& input = m_shader.GetOutput(i);
					if (input.name == outputVar.name)
					{
						TypeMustMatch(input.type, outputVar.type);
						return;
					}
				}

				throw AstError{ "Output not found" };
			}

			case ShaderNodes::VariableType::ParameterVariable:
			{
				assert(m_context->currentFunction);

				auto& parameter = static_cast<ShaderNodes::ParameterVariable&>(*node.var);
				const auto& parameters = m_context->currentFunction->parameters;

				auto it = std::find_if(parameters.begin(), parameters.end(), [&](const auto& parameter) { return parameter.name == parameter.name; });
				if (it == parameters.end())
					throw AstError{ "Parameter not found in function" };

				TypeMustMatch(it->type, parameter.type);
				break;
			}

			case ShaderNodes::VariableType::UniformVariable:
			{
				auto& uniformVar = static_cast<ShaderNodes::UniformVariable&>(*node.var);

				for (std::size_t i = 0; i < m_shader.GetUniformCount(); ++i)
				{
					const auto& uniform = m_shader.GetUniform(i);
					if (uniform.name == uniformVar.name)
					{
						TypeMustMatch(uniform.type, uniformVar.type);
						return;
					}
				}

				throw AstError{ "Uniform not found" };
			}

			default:
				break;
		}
	}

	void ShaderValidator::Visit(const ShaderNodes::IntrinsicCall& node)
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

				ShaderNodes::ExpressionType type = node.parameters.front()->GetExpressionType();
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
				if (node.parameters[0]->GetExpressionType() != ShaderNodes::ExpressionType::Float3)
					throw AstError{ "CrossProduct only works with Float3 expressions" };

				break;
			}

			case ShaderNodes::IntrinsicType::DotProduct:
				break;
		}

		for (auto& param : node.parameters)
			Visit(param);
	}

	void ShaderValidator::Visit(const ShaderNodes::Sample2D& node)
	{
		if (MandatoryExpr(node.sampler)->GetExpressionType() != ShaderNodes::ExpressionType::Sampler2D)
			throw AstError{ "Sampler must be a Sampler2D" };

		if (MandatoryExpr(node.coordinates)->GetExpressionType() != ShaderNodes::ExpressionType::Float2)
			throw AstError{ "Coordinates must be a Float2" };

		Visit(node.sampler);
		Visit(node.coordinates);
	}

	void ShaderValidator::Visit(const ShaderNodes::StatementBlock& node)
	{
		assert(m_context);

		m_context->blockLocalIndex.push_back(m_context->declaredLocals.size());

		for (const auto& statement : node.statements)
			Visit(MandatoryNode(statement));

		assert(m_context->declaredLocals.size() >= m_context->blockLocalIndex.back());
		m_context->declaredLocals.resize(m_context->blockLocalIndex.back());
		m_context->blockLocalIndex.pop_back();
	}

	void ShaderValidator::Visit(const ShaderNodes::SwizzleOp& node)
	{
		if (node.componentCount > 4)
			throw AstError{ "Cannot swizzle more than four elements" };

		switch (MandatoryExpr(node.expression)->GetExpressionType())
		{
			case ShaderNodes::ExpressionType::Float1:
			case ShaderNodes::ExpressionType::Float2:
			case ShaderNodes::ExpressionType::Float3:
			case ShaderNodes::ExpressionType::Float4:
				break;

			default:
				throw AstError{ "Cannot swizzle this type" };
		}

		Visit(node.expression);
	}

	bool ValidateShader(const ShaderAst& shader, std::string* error)
	{
		ShaderValidator validator(shader);
		return validator.Validate(error);
	}
}
