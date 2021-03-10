// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_ENUMS_HPP
#define NAZARA_SHADER_ENUMS_HPP

#include <Nazara/Prerequisites.hpp>

namespace Nz::ShaderAst
{
	enum class AssignType
	{
		Simple //< =
	};

	enum class BasicType
	{
		Boolean,   //< bool
		Float1,    //< float
		Float2,    //< vec2
		Float3,    //< vec3
		Float4,    //< vec4
		Int1,      //< int
		Int2,      //< ivec2
		Int3,      //< ivec3
		Int4,      //< ivec4
		Mat4x4,    //< mat4
		Sampler2D, //< sampler2D
		Void,      //< void
		UInt1,     //< uint
		UInt2,     //< uvec2
		UInt3,     //< uvec3
		UInt4      //< uvec4
	};

	enum class BinaryType
	{
		Add,       //< +
		Subtract, //< -
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
		VertexPosition, // gl_Position
	};

	enum class ExpressionCategory
	{
		LValue,
		RValue
	};

	enum class IntrinsicType
	{
		CrossProduct,
		DotProduct
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
#include <Nazara/Shader/ShaderAstNodes.hpp>
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
		None = -1,

		BuiltinVariable,
		InputVariable,
		LocalVariable,
		OutputVariable,
		ParameterVariable,
		UniformVariable
	};

	inline std::size_t GetComponentCount(BasicType type);
	inline BasicType GetComponentType(BasicType type);
}

#include <Nazara/Shader/ShaderEnums.inl>

#endif // NAZARA_SHADER_ENUMS_HPP
