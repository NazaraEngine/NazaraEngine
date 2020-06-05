// Copyright (C) 2020 Jérôme Leclercq
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
	class ByteStream;
	class ShaderVisitor;
	class ShaderWriter;

	namespace ShaderAst
	{
		enum class AssignType
		{
			Simple //< =
		};

		enum class BinaryIntrinsic
		{
			CrossProduct,
			DotProduct
		};

		enum class BinaryType
		{
			Add,       //< +
			Substract, //< -
			Multiply,  //< *
			Divide,    //< /

			Equality  //< ==
		};

		enum class BuiltinEntry
		{
			VertexPosition, // gl_Position
		};

		enum class ExpressionCategory
		{
			LValue,
			RValue
		};

		enum class ExpressionType
		{
			Boolean,   // bool
			Float1,    // float
			Float2,    // vec2
			Float3,    // vec3
			Float4,    // vec4
			Mat4x4,    // mat4
			Sampler2D, // sampler2D

			Void     // void
		};

		enum class NodeType
		{
			None = -1,

			AssignOp,
			BinaryFunc,
			BinaryOp,
			Branch,
			BuiltinVariable,
			Cast,
			Constant,
			ConditionalStatement,
			DeclareVariable,
			ExpressionStatement,
			NamedVariable,
			Sample2D,
			SwizzleOp,
			StatementBlock
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
				virtual ~Node();

				inline NodeType GetType() const;

				virtual void Register(ShaderWriter& visitor) = 0;
				virtual void Visit(ShaderVisitor& visitor) = 0;

				static inline unsigned int GetComponentCount(ExpressionType type);
				static inline ExpressionType GetComponentType(ExpressionType type);

			protected:
				inline Node(NodeType type);

			private:
				NodeType m_type;
		};

		class Statement;

		using StatementPtr = std::shared_ptr<Statement>;

		class NAZARA_RENDERER_API Statement : public Node
		{
			public:
				using Node::Node;
		};

		class Expression;

		using ExpressionPtr = std::shared_ptr<Expression>;

		class NAZARA_RENDERER_API Expression : public Node
		{
			public:
				using Node::Node;

				virtual ExpressionCategory GetExpressionCategory() const;
				virtual ExpressionType GetExpressionType() const = 0;
		};

		struct NAZARA_RENDERER_API ExpressionStatement : public Statement
		{
			inline ExpressionStatement();

			void Register(ShaderWriter& visitor) override;
			void Visit(ShaderVisitor& visitor) override;

			ExpressionPtr expression;

			static inline std::shared_ptr<ExpressionStatement> Build(ExpressionPtr expr);
		};

		//////////////////////////////////////////////////////////////////////////

		struct NAZARA_RENDERER_API ConditionalStatement : public Statement
		{
			inline ConditionalStatement();

			void Register(ShaderWriter& visitor) override;
			void Visit(ShaderVisitor& visitor) override;

			std::string conditionName;
			StatementPtr statement;

			static inline std::shared_ptr<ConditionalStatement> Build(std::string condition, StatementPtr statementPtr);
		};

		struct NAZARA_RENDERER_API StatementBlock : public Statement
		{
			inline StatementBlock();

			void Register(ShaderWriter& visitor) override;
			void Visit(ShaderVisitor& visitor) override;

			std::vector<StatementPtr> statements;

			template<typename... Args> static std::shared_ptr<StatementBlock> Build(Args&&... args);
		};

		struct Variable;

		using VariablePtr = std::shared_ptr<Variable>;

		struct NAZARA_RENDERER_API Variable : public Expression
		{
			using Expression::Expression;

			ExpressionCategory GetExpressionCategory() const override;
			ExpressionType GetExpressionType() const override;

			ExpressionType type;
			VariableType   kind;
		};


		struct NAZARA_RENDERER_API BuiltinVariable : public Variable
		{
			inline BuiltinVariable();

			void Register(ShaderWriter& visitor) override;
			void Visit(ShaderVisitor& visitor) override;

			BuiltinEntry var;

			static inline std::shared_ptr<BuiltinVariable> Build(BuiltinEntry variable, ExpressionType varType);
		};


		struct NamedVariable;

		using NamedVariablePtr = std::shared_ptr<NamedVariable>;

		struct NAZARA_RENDERER_API NamedVariable : public Variable
		{
			inline NamedVariable();

			void Register(ShaderWriter& visitor) override;
			void Visit(ShaderVisitor& visitor) override;

			std::string name;

			static inline std::shared_ptr<NamedVariable> Build(VariableType varType, std::string varName, ExpressionType expressionType);
		};
		
		struct NAZARA_RENDERER_API DeclareVariable : public Statement
		{
			inline DeclareVariable();

			void Register(ShaderWriter& visitor) override;
			void Visit(ShaderVisitor& visitor) override;

			NamedVariablePtr variable;
			ExpressionPtr expression;

			static inline std::shared_ptr<DeclareVariable> Build(NamedVariablePtr variable, ExpressionPtr expression = nullptr);
		};

		//////////////////////////////////////////////////////////////////////////

		struct NAZARA_RENDERER_API AssignOp : public Expression
		{
			inline AssignOp();

			ExpressionType GetExpressionType() const override;
			void Register(ShaderWriter& visitor) override;
			void Visit(ShaderVisitor& visitor) override;

			AssignType    op;
			ExpressionPtr left;
			ExpressionPtr right;

			static inline std::shared_ptr<AssignOp> Build(AssignType op, ExpressionPtr left, ExpressionPtr right);
		};

		struct NAZARA_RENDERER_API BinaryOp : public Expression
		{
			inline BinaryOp();

			ExpressionType GetExpressionType() const override;
			void Register(ShaderWriter& visitor) override;
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

			void Register(ShaderWriter& visitor) override;
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

			ExpressionType GetExpressionType() const override;
			void Register(ShaderWriter& visitor) override;
			void Visit(ShaderVisitor& visitor) override;

			ExpressionType exprType;
			std::array<ExpressionPtr, 4> expressions;

			static inline std::shared_ptr<Cast> Build(ExpressionType castTo, ExpressionPtr first, ExpressionPtr second = nullptr, ExpressionPtr third = nullptr, ExpressionPtr fourth = nullptr);
			static inline std::shared_ptr<Cast> Build(ExpressionType castTo, ExpressionPtr* expressions, std::size_t expressionCount);
		};

		struct NAZARA_RENDERER_API Constant : public Expression
		{
			inline Constant();

			ExpressionType GetExpressionType() const override;
			void Register(ShaderWriter& visitor) override;
			void Visit(ShaderVisitor& visitor) override;

			ExpressionType exprType;

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
			ExpressionType GetExpressionType() const override;
			void Register(ShaderWriter& visitor) override;
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

			ExpressionType GetExpressionType() const override;
			void Register(ShaderWriter& visitor) override;
			void Visit(ShaderVisitor& visitor) override;

			ExpressionPtr sampler;
			ExpressionPtr coordinates;

			static inline std::shared_ptr<Sample2D> Build(ExpressionPtr samplerPtr, ExpressionPtr coordinatesPtr);
		};

		//////////////////////////////////////////////////////////////////////////

		struct NAZARA_RENDERER_API BinaryFunc : public Expression
		{
			inline BinaryFunc();

			ExpressionType GetExpressionType() const override;
			void Register(ShaderWriter& visitor) override;
			void Visit(ShaderVisitor& visitor) override;

			BinaryIntrinsic intrinsic;
			ExpressionPtr   left;
			ExpressionPtr   right;

			static inline std::shared_ptr<BinaryFunc> Build(BinaryIntrinsic intrinsic, ExpressionPtr left, ExpressionPtr right);
		};
	}
}

#include <Nazara/Renderer/ShaderAst.inl>

#endif // NAZARA_SHADER_AST_HPP
