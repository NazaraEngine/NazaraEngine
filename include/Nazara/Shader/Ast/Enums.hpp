// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_ENUMS_HPP
#define NAZARA_SHADER_AST_ENUMS_HPP

#include <Nazara/Prerequisites.hpp>

namespace Nz::ShaderAst
{
	enum class AssignType
	{
		Simple //< =
	};

	enum class AttributeType
	{
		Binding,  //< Binding (external var only) - has argument index
		Builtin,  //< Builtin (struct member only) - has argument type
		Entry,    //< Entry point (function only) - has argument type
		Layout,   //< Struct layout (struct only) - has argument style
		Location, //< Location (struct member only) - has argument index
		Option,   //< Conditional compilation option - has argument expr
	};

	enum class BinaryType
	{
		Add,       //< +
		Subtract,  //< -
		Multiply,  //< *
		Divide,    //< /

		CompEq,    //< ==
		CompGe,    //< >=
		CompGt,    //< >
		CompLe,    //< <=
		CompLt,    //< <
		CompNe     //< <=
	};

	enum class BuiltinEntry
	{
		FragCoord      = 1, // gl_FragCoord
		VertexPosition = 0, // gl_Position
	};

	enum class ExpressionCategory
	{
		LValue,
		RValue
	};

	enum class IntrinsicType
	{
		CrossProduct = 0,
		DotProduct = 1,
		Length = 3,
		Max = 4,
		Min = 5,
		SampleTexture = 2,
	};

	enum class MemoryLayout
	{
		Std140
	};

	enum class NodeType
	{
		None = -1,

#define NAZARA_SHADERAST_NODE(Node) Node,
#define NAZARA_SHADERAST_STATEMENT_LAST(Node) Node, Max = Node
#include <Nazara/Shader/Ast/AstNodeList.hpp>
	};

	enum class PrimitiveType
	{
		Boolean, //< bool
		Float32, //< f32
		Int32,   //< i32
		UInt32,  //< ui32
	};

	enum class SwizzleComponent
	{
		First,
		Second,
		Third,
		Fourth
	};

	enum class UnaryType
	{
		LogicalNot, //< !v
		Minus,      //< -v
		Plus,       //< +v
	};
}

#endif // NAZARA_SHADER_ENUMS_HPP
