// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_NODES_HPP
#define NAZARA_SHADER_NODES_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderConstantValue.hpp>
#include <Nazara/Shader/ShaderEnums.hpp>
#include <Nazara/Shader/ShaderExpressionType.hpp>
#include <Nazara/Shader/ShaderVariables.hpp>
#include <array>
#include <optional>
#include <string>

namespace Nz
{
	class ShaderAstVisitor;

	namespace ShaderNodes
	{
		class Node;

		using NodePtr = std::shared_ptr<Node>;

		class NAZARA_SHADER_API Node
		{
			public:
				virtual ~Node();

				inline NodeType GetType() const;
				inline bool IsStatement() const;

				virtual void Visit(ShaderAstVisitor& visitor) = 0;

				static inline unsigned int GetComponentCount(BasicType type);
				static inline BasicType GetComponentType(BasicType type);

			protected:
				inline Node(NodeType type, bool isStatement);

			private:
				NodeType m_type;
				bool m_isStatement;
		};
		
		class Expression;

		using ExpressionPtr = std::shared_ptr<Expression>;

		class NAZARA_SHADER_API Expression : public Node
		{
			public:
				inline Expression(NodeType type);

				virtual ExpressionCategory GetExpressionCategory() const;
				virtual ShaderExpressionType GetExpressionType() const = 0;
		};

		class Statement;

		using StatementPtr = std::shared_ptr<Statement>;

		class NAZARA_SHADER_API Statement : public Node
		{
			public:
				inline Statement(NodeType type);
		};

		struct NAZARA_SHADER_API ExpressionStatement : public Statement
		{
			inline ExpressionStatement();

			void Visit(ShaderAstVisitor& visitor) override;

			ExpressionPtr expression;

			static inline std::shared_ptr<ExpressionStatement> Build(ExpressionPtr expr);
		};

		//////////////////////////////////////////////////////////////////////////

		struct NAZARA_SHADER_API ConditionalStatement : public Statement
		{
			inline ConditionalStatement();

			void Visit(ShaderAstVisitor& visitor) override;

			std::string conditionName;
			StatementPtr statement;

			static inline std::shared_ptr<ConditionalStatement> Build(std::string condition, StatementPtr statementPtr);
		};

		struct NAZARA_SHADER_API StatementBlock : public Statement
		{
			inline StatementBlock();

			void Visit(ShaderAstVisitor& visitor) override;

			std::vector<StatementPtr> statements;

			static inline std::shared_ptr<StatementBlock> Build(std::vector<StatementPtr> statements);
			template<typename... Args> static std::shared_ptr<StatementBlock> Build(Args&&... args);
		};

		struct NAZARA_SHADER_API DeclareVariable : public Statement
		{
			inline DeclareVariable();

			void Visit(ShaderAstVisitor& visitor) override;

			ExpressionPtr expression;
			VariablePtr variable;

			static inline std::shared_ptr<DeclareVariable> Build(VariablePtr variable, ExpressionPtr expression = nullptr);
		};

		struct NAZARA_SHADER_API Identifier : public Expression
		{
			inline Identifier();

			ExpressionCategory GetExpressionCategory() const override;
			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderAstVisitor& visitor) override;

			VariablePtr var;

			static inline std::shared_ptr<Identifier> Build(VariablePtr variable);
		};

		struct NAZARA_SHADER_API AccessMember : public Expression
		{
			inline AccessMember();

			ExpressionCategory GetExpressionCategory() const override;
			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderAstVisitor& visitor) override;

			ExpressionPtr structExpr;
			ShaderExpressionType exprType;
			std::vector<std::size_t> memberIndices;

			static inline std::shared_ptr<AccessMember> Build(ExpressionPtr structExpr, std::size_t memberIndex, ShaderExpressionType exprType);
			static inline std::shared_ptr<AccessMember> Build(ExpressionPtr structExpr, std::vector<std::size_t> memberIndices, ShaderExpressionType exprType);
		};

		//////////////////////////////////////////////////////////////////////////

		struct NAZARA_SHADER_API AssignOp : public Expression
		{
			inline AssignOp();

			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderAstVisitor& visitor) override;

			AssignType    op;
			ExpressionPtr left;
			ExpressionPtr right;

			static inline std::shared_ptr<AssignOp> Build(AssignType op, ExpressionPtr left, ExpressionPtr right);
		};

		struct NAZARA_SHADER_API BinaryOp : public Expression
		{
			inline BinaryOp();

			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderAstVisitor& visitor) override;

			BinaryType    op;
			ExpressionPtr left;
			ExpressionPtr right;

			static inline std::shared_ptr<BinaryOp> Build(BinaryType op, ExpressionPtr left, ExpressionPtr right);
		};

		struct NAZARA_SHADER_API Branch : public Statement
		{
			struct ConditionalStatement;

			inline Branch();

			void Visit(ShaderAstVisitor& visitor) override;

			std::vector<ConditionalStatement> condStatements;
			StatementPtr elseStatement;

			struct ConditionalStatement
			{
				ExpressionPtr condition;
				StatementPtr  statement;
			};

			static inline std::shared_ptr<Branch> Build(ExpressionPtr condition, StatementPtr trueStatement, StatementPtr falseStatement = nullptr);
			static inline std::shared_ptr<Branch> Build(std::vector<ConditionalStatement> statements, StatementPtr elseStatement = nullptr);
		};

		struct NAZARA_SHADER_API Cast : public Expression
		{
			inline Cast();

			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderAstVisitor& visitor) override;

			BasicType exprType;
			std::array<ExpressionPtr, 4> expressions;

			static inline std::shared_ptr<Cast> Build(BasicType castTo, ExpressionPtr first, ExpressionPtr second = nullptr, ExpressionPtr third = nullptr, ExpressionPtr fourth = nullptr);
			static inline std::shared_ptr<Cast> Build(BasicType castTo, ExpressionPtr* expressions, std::size_t expressionCount);
		};

		struct NAZARA_SHADER_API Constant : public Expression
		{
			inline Constant();

			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderAstVisitor& visitor) override;

			ShaderConstantValue value;

			template<typename T> static std::shared_ptr<Constant> Build(const T& value);
		};

		struct NAZARA_SHADER_API SwizzleOp : public Expression
		{
			inline SwizzleOp();

			ExpressionCategory GetExpressionCategory() const override;
			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderAstVisitor& visitor) override;

			std::array<SwizzleComponent, 4> components;
			std::size_t componentCount;
			ExpressionPtr expression;

			static inline std::shared_ptr<SwizzleOp> Build(ExpressionPtr expressionPtr, std::initializer_list<SwizzleComponent> swizzleComponents);
			static inline std::shared_ptr<SwizzleOp> Build(ExpressionPtr expressionPtr, const SwizzleComponent* components, std::size_t componentCount);
		};

		//////////////////////////////////////////////////////////////////////////

		struct NAZARA_SHADER_API Sample2D : public Expression
		{
			inline Sample2D();

			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderAstVisitor& visitor) override;

			ExpressionPtr sampler;
			ExpressionPtr coordinates;

			static inline std::shared_ptr<Sample2D> Build(ExpressionPtr samplerPtr, ExpressionPtr coordinatesPtr);
		};

		//////////////////////////////////////////////////////////////////////////

		struct NAZARA_SHADER_API IntrinsicCall : public Expression
		{
			inline IntrinsicCall();

			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderAstVisitor& visitor) override;

			IntrinsicType intrinsic;
			std::vector<ExpressionPtr> parameters;

			static inline std::shared_ptr<IntrinsicCall> Build(IntrinsicType intrinsic, std::vector<ExpressionPtr> parameters);
		};
	}
}

#include <Nazara/Shader/ShaderNodes.inl>

#endif
