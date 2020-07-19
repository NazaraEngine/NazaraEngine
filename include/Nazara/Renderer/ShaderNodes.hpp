// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_NODES_HPP
#define NAZARA_SHADER_NODES_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/ShaderEnums.hpp>
#include <Nazara/Renderer/ShaderExpressionType.hpp>
#include <Nazara/Renderer/ShaderVariables.hpp>
#include <array>
#include <optional>
#include <string>

namespace Nz
{
	class ShaderVisitor;

	namespace ShaderNodes
	{
		class Node;

		using NodePtr = std::shared_ptr<Node>;

		class NAZARA_RENDERER_API Node
		{
			public:
				virtual ~Node();

				inline NodeType GetType() const;
				inline bool IsStatement() const;

				virtual void Visit(ShaderVisitor& visitor) = 0;

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

		class NAZARA_RENDERER_API Expression : public Node
		{
			public:
				inline Expression(NodeType type);

				virtual ExpressionCategory GetExpressionCategory() const;
				virtual ShaderExpressionType GetExpressionType() const = 0;
		};

		class Statement;

		using StatementPtr = std::shared_ptr<Statement>;

		class NAZARA_RENDERER_API Statement : public Node
		{
			public:
				inline Statement(NodeType type);
		};

		struct NAZARA_RENDERER_API ExpressionStatement : public Statement
		{
			inline ExpressionStatement();

			void Visit(ShaderVisitor& visitor) override;

			ExpressionPtr expression;

			static inline std::shared_ptr<ExpressionStatement> Build(ExpressionPtr expr);
		};

		//////////////////////////////////////////////////////////////////////////

		struct NAZARA_RENDERER_API ConditionalStatement : public Statement
		{
			inline ConditionalStatement();

			void Visit(ShaderVisitor& visitor) override;

			std::string conditionName;
			StatementPtr statement;

			static inline std::shared_ptr<ConditionalStatement> Build(std::string condition, StatementPtr statementPtr);
		};

		struct NAZARA_RENDERER_API StatementBlock : public Statement
		{
			inline StatementBlock();

			void Visit(ShaderVisitor& visitor) override;

			std::vector<StatementPtr> statements;

			template<typename... Args> static std::shared_ptr<StatementBlock> Build(Args&&... args);
		};

		struct NAZARA_RENDERER_API DeclareVariable : public Statement
		{
			inline DeclareVariable();

			void Visit(ShaderVisitor& visitor) override;

			LocalVariablePtr variable;
			ExpressionPtr expression;

			static inline std::shared_ptr<DeclareVariable> Build(LocalVariablePtr variable, ExpressionPtr expression = nullptr);
		};

		struct NAZARA_RENDERER_API Identifier : public Expression
		{
			inline Identifier();

			ExpressionCategory GetExpressionCategory() const override;
			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderVisitor& visitor) override;

			VariablePtr var;

			static inline std::shared_ptr<Identifier> Build(VariablePtr variable);
		};

		struct NAZARA_RENDERER_API AccessMember : public Expression
		{
			inline AccessMember();

			ExpressionCategory GetExpressionCategory() const override;
			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderVisitor& visitor) override;

			std::size_t memberIndex;
			ExpressionPtr structExpr;
			ShaderExpressionType exprType; //< FIXME: Use ShaderAst to automate

			static inline std::shared_ptr<AccessMember> Build(ExpressionPtr structExpr, std::size_t memberIndex, ShaderExpressionType exprType);
		};

		//////////////////////////////////////////////////////////////////////////

		struct NAZARA_RENDERER_API AssignOp : public Expression
		{
			inline AssignOp();

			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderVisitor& visitor) override;

			AssignType    op;
			ExpressionPtr left;
			ExpressionPtr right;

			static inline std::shared_ptr<AssignOp> Build(AssignType op, ExpressionPtr left, ExpressionPtr right);
		};

		struct NAZARA_RENDERER_API BinaryOp : public Expression
		{
			inline BinaryOp();

			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderVisitor& visitor) override;

			BinaryType    op;
			ExpressionPtr left;
			ExpressionPtr right;

			static inline std::shared_ptr<BinaryOp> Build(BinaryType op, ExpressionPtr left, ExpressionPtr right);
		};

		struct NAZARA_RENDERER_API Branch : public Statement
		{
			struct ConditionalStatement;

			inline Branch();

			void Visit(ShaderVisitor& visitor) override;

			std::vector<ConditionalStatement> condStatements;
			StatementPtr elseStatement;

			struct ConditionalStatement
			{
				ExpressionPtr condition;
				StatementPtr  statement;
			};

			inline std::shared_ptr<Branch> Build(ExpressionPtr condition, StatementPtr trueStatement, StatementPtr falseStatement = nullptr);
		};

		struct NAZARA_RENDERER_API Cast : public Expression
		{
			inline Cast();

			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderVisitor& visitor) override;

			BasicType exprType;
			std::array<ExpressionPtr, 4> expressions;

			static inline std::shared_ptr<Cast> Build(BasicType castTo, ExpressionPtr first, ExpressionPtr second = nullptr, ExpressionPtr third = nullptr, ExpressionPtr fourth = nullptr);
			static inline std::shared_ptr<Cast> Build(BasicType castTo, ExpressionPtr* expressions, std::size_t expressionCount);
		};

		struct NAZARA_RENDERER_API Constant : public Expression
		{
			inline Constant();

			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderVisitor& visitor) override;

			BasicType exprType;

			union
			{
				bool bool1;
				float vec1;
				Vector2f vec2;
				Vector3f vec3;
				Vector4f vec4;
			} values;

			static inline std::shared_ptr<Constant> Build(bool value);
			static inline std::shared_ptr<Constant> Build(float value);
			static inline std::shared_ptr<Constant> Build(const Vector2f& value);
			static inline std::shared_ptr<Constant> Build(const Vector3f& value);
			static inline std::shared_ptr<Constant> Build(const Vector4f& value);
		};

		struct NAZARA_RENDERER_API SwizzleOp : public Expression
		{
			inline SwizzleOp();

			ExpressionCategory GetExpressionCategory() const override;
			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderVisitor& visitor) override;

			std::array<SwizzleComponent, 4> components;
			std::size_t componentCount;
			ExpressionPtr expression;

			static inline std::shared_ptr<SwizzleOp> Build(ExpressionPtr expressionPtr, std::initializer_list<SwizzleComponent> swizzleComponents);
		};

		//////////////////////////////////////////////////////////////////////////

		struct NAZARA_RENDERER_API Sample2D : public Expression
		{
			inline Sample2D();

			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderVisitor& visitor) override;

			ExpressionPtr sampler;
			ExpressionPtr coordinates;

			static inline std::shared_ptr<Sample2D> Build(ExpressionPtr samplerPtr, ExpressionPtr coordinatesPtr);
		};

		//////////////////////////////////////////////////////////////////////////

		struct NAZARA_RENDERER_API IntrinsicCall : public Expression
		{
			inline IntrinsicCall();

			ShaderExpressionType GetExpressionType() const override;
			void Visit(ShaderVisitor& visitor) override;

			IntrinsicType intrinsic;
			std::vector<ExpressionPtr> parameters;

			static inline std::shared_ptr<IntrinsicCall> Build(IntrinsicType intrinsic, std::vector<ExpressionPtr> parameters);
		};
	}
}

#include <Nazara/Renderer/ShaderNodes.inl>

#endif
