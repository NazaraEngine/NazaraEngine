// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_ENUMS_HPP
#define NAZARA_SHADER_AST_ENUMS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Flags.hpp>

namespace Nz
{
	namespace ShaderAst
	{
		enum class AssignType
		{
			Simple,             //< a = b
			CompoundAdd,        //< a += b
			CompoundDivide,     //< a /= b
			CompoundMultiply,   //< a *= b
			CompoundLogicalAnd, //< a &&= b
			CompoundLogicalOr,  //< a ||= b
			CompoundSubtract,   //< a -= b
		};

		enum class AttributeType
		{
			Binding,            //< Binding (external var only) - has argument index
			Builtin,            //< Builtin (struct member only) - has argument type
			Cond,               //< Conditional compilation option - has argument expr
			DepthWrite,         //< Depth write mode (function only) - has argument type
			EarlyFragmentTests, //< Entry point (function only) - has argument on/off
			Entry,              //< Entry point (function only) - has argument type
			Layout,             //< Struct layout (struct only) - has argument style
			Location,           //< Location (struct member only) - has argument index
			Set,                //< Binding set (external var only) - has argument index
		};

		enum class BinaryType
		{
			Add,        //< +
			CompEq,     //< ==
			CompGe,     //< >=
			CompGt,     //< >
			CompLe,     //< <=
			CompLt,     //< <
			CompNe,     //< <=
			Divide,     //< /
			Multiply,   //< *
			LogicalAnd, //< &&
			LogicalOr,  //< ||
			Subtract,   //< -
		};

		enum class BuiltinEntry
		{
			FragCoord      = 1, // gl_FragCoord
			FragDepth      = 2, // gl_FragDepth
			VertexPosition = 0, // gl_Position
		};

		enum class DepthWriteMode
		{
			Greater,
			Less,
			Replace,
			Unchanged,
		};

		enum class ExpressionCategory
		{
			LValue,
			RValue
		};

		enum class FunctionFlag
		{
			DoesDiscard,
			DoesWriteFragDepth,

			Max = DoesWriteFragDepth
		};
	}

	template<>
	struct EnumAsFlags<ShaderAst::FunctionFlag>
	{
		static constexpr ShaderAst::FunctionFlag max = ShaderAst::FunctionFlag::Max;
	};

	namespace ShaderAst
	{
		using FunctionFlags = Flags<FunctionFlag>;

		enum class IntrinsicType
		{
			CrossProduct = 0,
			DotProduct = 1,
			Exp = 7,
			Length = 3,
			Max = 4,
			Min = 5,
			Pow = 6,
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

		enum class UnaryType
		{
			LogicalNot, //< !v
			Minus,      //< -v
			Plus,       //< +v
		};
	}
}

#endif // NAZARA_SHADER_AST_ENUMS_HPP
