// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_HPP
#define NAZARA_SHADER_AST_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <array>

namespace Nz
{
	class ShaderWriter;

	namespace ShaderAst
	{
		enum class AssignType
		{
			Simple //< =
		};

		enum class BinaryType
		{
			Add,       //< +
			Substract, //< -
			Multiply,  //< *
			Divide,    //< /
			Equality   //< ==
		};

		enum class BuiltinEntry
		{
			VertexPosition, // gl_Position
		};

		enum class ExpressionType
		{
			Boolean, // bool
			Float1,  // float
			Float2,  // vec2
			Float3,  // vec3
			Float4,  // vec4
			Mat4x4,  // mat4

			Void     // void
		};

		enum class SwizzleComponent
		{
			First,
			Second,
			Third,
			Fourth
		};

		enum class VariableType
		{
			Builtin,
			Input,
			Output,
			Parameter,
			Uniform,
			Variable
		};

		//////////////////////////////////////////////////////////////////////////

		class Node;

		using NodePtr = std::shared_ptr<Node>;

		class NAZARA_RENDERER_API Node
		{
			public:
				virtual ~Node() = default;

				virtual void Register(ShaderWriter& visitor) = 0;
				virtual void Visit(ShaderWriter& visitor) = 0;

				static inline unsigned int GetComponentCount(ExpressionType type);
				static inline ExpressionType GetComponentType(ExpressionType type);
		};

		class Statement;

		using StatementPtr = std::shared_ptr<Statement>;

		class NAZARA_RENDERER_API Statement : public Node
		{
		};

		class Expression;

		using ExpressionPtr = std::shared_ptr<Expression>;

		class NAZARA_RENDERER_API Expression : public Node
		{
			public:
				virtual ExpressionType GetExpressionType() const = 0;
		};

		class NAZARA_RENDERER_API ExpressionStatement : public Statement
		{
			public:
				inline explicit ExpressionStatement(ExpressionPtr expr);

				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				ExpressionPtr expression;
		};

		//////////////////////////////////////////////////////////////////////////

		class NAZARA_RENDERER_API ConditionalStatement : public Statement
		{
			public:
				inline ConditionalStatement(const String& condition, StatementPtr statementPtr);

				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				String conditionName;
				StatementPtr statement;
		};

		class NAZARA_RENDERER_API StatementBlock : public Statement
		{
			public:
				template<typename... Args> explicit StatementBlock(Args&&... args);

				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				std::vector<StatementPtr> statements;
		};

		class Variable;

		using VariablePtr = std::shared_ptr<Variable>;

		class NAZARA_RENDERER_API Variable : public Expression
		{
			public:
				inline Variable(VariableType varKind, ExpressionType varType);

				ExpressionType GetExpressionType() const override;

				ExpressionType type;
				VariableType   kind;
		};


		class NAZARA_RENDERER_API BuiltinVariable : public Variable
		{
			public:
				inline BuiltinVariable(BuiltinEntry variable, ExpressionType varType);

				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				BuiltinEntry var;
		};


		class NamedVariable;

		using NamedVariablePtr = std::shared_ptr<NamedVariable>;

		class NAZARA_RENDERER_API NamedVariable : public Variable
		{
			public:
				inline NamedVariable(VariableType varKind, const Nz::String& varName, ExpressionType varType);

				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				Nz::String name;
		};

		//////////////////////////////////////////////////////////////////////////

		class NAZARA_RENDERER_API AssignOp : public Expression
		{
			public:
				inline AssignOp(AssignType Op, VariablePtr Var, ExpressionPtr Right);

				ExpressionType GetExpressionType() const override;
				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				AssignType    op;
				VariablePtr   variable;
				ExpressionPtr right;
		};

		class NAZARA_RENDERER_API BinaryOp : public Expression
		{
			public:
				inline BinaryOp(BinaryType Op, ExpressionPtr Left, ExpressionPtr Right);

				ExpressionType GetExpressionType() const override;
				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				BinaryType    op;
				ExpressionPtr left;
				ExpressionPtr right;
		};

		class NAZARA_RENDERER_API Branch : public Statement
		{
			public:
				inline Branch(ExpressionPtr condition, StatementPtr trueStatement, StatementPtr falseStatement = nullptr);

				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				struct ConditionalStatement
				{
					ExpressionPtr condition;
					StatementPtr  statement;
				};

				std::vector<ConditionalStatement> condStatements;
				StatementPtr elseStatement;
		};

		class NAZARA_RENDERER_API Cast : public Expression
		{
			public:
				inline Cast(ExpressionType castTo, ExpressionPtr first, ExpressionPtr second = nullptr, ExpressionPtr third = nullptr, ExpressionPtr fourth = nullptr);

				ExpressionType GetExpressionType() const override;
				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				ExpressionType exprType;
				std::array<ExpressionPtr, 4> expressions;
		};

		class NAZARA_RENDERER_API Constant : public Expression
		{
			public:
				inline explicit Constant(bool value);
				inline explicit Constant(float value);
				inline explicit Constant(const Vector2f& value);
				inline explicit Constant(const Vector3f& value);
				inline explicit Constant(const Vector4f& value);

				ExpressionType GetExpressionType() const override;
				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				ExpressionType exprType;

				union
				{
					bool bool1;
					float vec1;
					Vector2f vec2;
					Vector3f vec3;
					Vector4f vec4;
				} values;
		};

		class NAZARA_RENDERER_API SwizzleOp : public Expression
		{
			public:
				inline SwizzleOp(ExpressionPtr expressionPtr, std::initializer_list<SwizzleComponent> swizzleComponents);

				ExpressionType GetExpressionType() const override;
				void Register(ShaderWriter& visitor) override;
				void Visit(ShaderWriter& visitor) override;

				std::array<SwizzleComponent, 4> components;
				std::size_t componentCount;
				ExpressionPtr expression;
		};
	}
}

#include <Nazara/Renderer/ShaderAst.inl>

#endif // NAZARA_SHADER_AST_HPP
