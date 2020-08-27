// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderNodes.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderNodes
{
	inline Node::Node(NodeType type, bool isStatement) :
	m_type(type),
	m_isStatement(isStatement)
	{
	}

	inline NodeType ShaderNodes::Node::GetType() const
	{
		return m_type;
	}

	inline bool Node::IsStatement() const
	{
		return m_isStatement;
	}

	inline unsigned int Node::GetComponentCount(BasicType type)
	{
		switch (type)
		{
			case BasicType::Float2:
			case BasicType::Int2:
				return 2;

			case BasicType::Float3:
			case BasicType::Int3:
				return 3;

			case BasicType::Float4:
			case BasicType::Int4:
				return 4;

			case BasicType::Mat4x4:
				return 4;

			default:
				return 1;
		}
	}

	inline BasicType Node::GetComponentType(BasicType type)
	{
		switch (type)
		{
			case BasicType::Float2:
			case BasicType::Float3:
			case BasicType::Float4:
				return BasicType::Float1;

			case BasicType::Int2:
			case BasicType::Int3:
			case BasicType::Int4:
				return BasicType::Int1;

			case BasicType::Mat4x4:
				return BasicType::Float4;

			default:
				return type;
		}
	}


	inline Expression::Expression(NodeType type) :
	Node(type, false)
	{
	}

	inline Statement::Statement(NodeType type) :
	Node(type, true)
	{
	}



	inline ExpressionStatement::ExpressionStatement() :
	Statement(NodeType::ExpressionStatement)
	{
	}
		
	inline std::shared_ptr<ExpressionStatement> ExpressionStatement::Build(ExpressionPtr expr)
	{
		auto node = std::make_shared<ExpressionStatement>();
		node->expression = std::move(expr);

		return node;
	}

	inline ConditionalStatement::ConditionalStatement() :
	Statement(NodeType::ConditionalStatement)
	{
	}

	inline std::shared_ptr<ConditionalStatement> ConditionalStatement::Build(std::string condition, StatementPtr statementPtr)
	{
		auto node = std::make_shared<ConditionalStatement>();
		node->conditionName = std::move(condition);
		node->statement = std::move(statementPtr);

		return node;
	}


	inline StatementBlock::StatementBlock() :
	Statement(NodeType::StatementBlock)
	{
	}

	inline std::shared_ptr<StatementBlock> StatementBlock::Build(std::vector<StatementPtr> statements)
	{
		auto node = std::make_shared<StatementBlock>();
		node->statements = std::move(statements);

		return node;
	}

	template<typename... Args>
	std::shared_ptr<StatementBlock> StatementBlock::Build(Args&&... args)
	{
		auto node = std::make_shared<StatementBlock>();
		node->statements = std::vector<StatementPtr>({ std::forward<Args>(args)... });

		return node;
	}


	inline DeclareVariable::DeclareVariable() :
	Statement(NodeType::DeclareVariable)
	{
	}

	inline std::shared_ptr<DeclareVariable> DeclareVariable::Build(VariablePtr variable, ExpressionPtr expression)
	{
		auto node = std::make_shared<DeclareVariable>();
		node->expression = std::move(expression);
		node->variable = std::move(variable);

		return node;
	}


	inline Identifier::Identifier() :
	Expression(NodeType::Identifier)
	{
	}

	inline std::shared_ptr<Identifier> Identifier::Build(VariablePtr variable)
	{
		auto node = std::make_shared<Identifier>();
		node->var = std::move(variable);

		return node;
	}


	inline AccessMember::AccessMember() :
	Expression(NodeType::AccessMember)
	{
	}

	inline std::shared_ptr<AccessMember> AccessMember::Build(ExpressionPtr structExpr, std::size_t memberIndex, ShaderExpressionType exprType)
	{
		return Build(std::move(structExpr), std::vector<std::size_t>{ memberIndex }, exprType);
	}

	inline std::shared_ptr<AccessMember> AccessMember::Build(ExpressionPtr structExpr, std::vector<std::size_t> memberIndices, ShaderExpressionType exprType)
	{
		auto node = std::make_shared<AccessMember>();
		node->exprType = std::move(exprType);
		node->memberIndices = std::move(memberIndices);
		node->structExpr = std::move(structExpr);

		return node;
	}


	inline AssignOp::AssignOp() :
	Expression(NodeType::AssignOp)
	{
	}

	inline std::shared_ptr<AssignOp> AssignOp::Build(AssignType op, ExpressionPtr left, ExpressionPtr right)
	{
		auto node = std::make_shared<AssignOp>();
		node->op = op;
		node->left = std::move(left);
		node->right = std::move(right);

		return node;
	}

		
	inline BinaryOp::BinaryOp() :
	Expression(NodeType::BinaryOp)
	{
	}

	inline std::shared_ptr<BinaryOp> BinaryOp::Build(BinaryType op, ExpressionPtr left, ExpressionPtr right)
	{
		auto node = std::make_shared<BinaryOp>();
		node->op = op;
		node->left = std::move(left);
		node->right = std::move(right);

		return node;
	}


	inline Branch::Branch() :
	Statement(NodeType::Branch)
	{
	}

	inline std::shared_ptr<Branch> Branch::Build(ExpressionPtr condition, StatementPtr trueStatement, StatementPtr falseStatement)
	{
		auto node = std::make_shared<Branch>();
		node->condStatements.emplace_back(ConditionalStatement{ std::move(condition), std::move(trueStatement) });
		node->elseStatement = std::move(falseStatement);

		return node;
	}

	inline std::shared_ptr<Branch> Branch::Build(std::vector<ConditionalStatement> statements, StatementPtr elseStatement)
	{
		auto node = std::make_shared<Branch>();
		node->condStatements = std::move(statements);
		node->elseStatement = std::move(elseStatement);

		return node;
	}


	inline Cast::Cast() :
	Expression(NodeType::Cast)
	{
	}

	inline std::shared_ptr<Cast> Cast::Build(BasicType castTo, ExpressionPtr first, ExpressionPtr second, ExpressionPtr third, ExpressionPtr fourth)
	{
		auto node = std::make_shared<Cast>();
		node->exprType = castTo;
		node->expressions = { {first, second, third, fourth} };

		return node;
	}

	inline std::shared_ptr<Cast> Cast::Build(BasicType castTo, ExpressionPtr* Expressions, std::size_t expressionCount)
	{
		auto node = std::make_shared<Cast>();
		node->exprType = castTo;
		for (std::size_t i = 0; i < expressionCount; ++i)
			node->expressions[i] = Expressions[i];

		return node;
	}


	inline Constant::Constant() :
	Expression(NodeType::Constant)
	{
	}

	template<typename T>
	std::shared_ptr<Constant> Nz::ShaderNodes::Constant::Build(const T& value)
	{
		auto node = std::make_shared<Constant>();
		node->value = value;

		return node;
	}


	inline SwizzleOp::SwizzleOp() :
	Expression(NodeType::SwizzleOp)
	{
	}

	inline std::shared_ptr<SwizzleOp> SwizzleOp::Build(ExpressionPtr expressionPtr, SwizzleComponent swizzleComponent)
	{
		return Build(std::move(expressionPtr), { swizzleComponent });
	}

	inline std::shared_ptr<SwizzleOp> SwizzleOp::Build(ExpressionPtr expressionPtr, std::initializer_list<SwizzleComponent> swizzleComponents)
	{
		auto node = std::make_shared<SwizzleOp>();
		node->componentCount = swizzleComponents.size();
		node->expression = std::move(expressionPtr);

		std::copy(swizzleComponents.begin(), swizzleComponents.end(), node->components.begin());

		return node;
	}

	inline std::shared_ptr<SwizzleOp> SwizzleOp::Build(ExpressionPtr expressionPtr, const SwizzleComponent* components, std::size_t componentCount)
	{
		auto node = std::make_shared<SwizzleOp>();

		assert(componentCount < node->components.size());

		node->componentCount = componentCount;
		node->expression = std::move(expressionPtr);

		std::copy(components, components + componentCount, node->components.begin());

		return node;
	}


	inline Sample2D::Sample2D() :
	Expression(NodeType::Sample2D)
	{
	}

	inline std::shared_ptr<Sample2D> Sample2D::Build(ExpressionPtr samplerPtr, ExpressionPtr coordinatesPtr)
	{
		auto node = std::make_shared<Sample2D>();
		node->coordinates = std::move(coordinatesPtr);
		node->sampler = std::move(samplerPtr);

		return node;
	}


	inline IntrinsicCall::IntrinsicCall() :
	Expression(NodeType::IntrinsicCall)
	{
	}

	inline std::shared_ptr<IntrinsicCall> IntrinsicCall::Build(IntrinsicType intrinsic, std::vector<ExpressionPtr> parameters)
	{
		auto node = std::make_shared<IntrinsicCall>();
		node->intrinsic = intrinsic;
		node->parameters = std::move(parameters);

		return node;
	}
}

#include <Nazara/Shader/DebugOff.hpp>
