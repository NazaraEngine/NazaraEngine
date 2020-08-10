// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_ENUMS_HPP
#define NAZARA_SHADER_ENUMS_HPP

#include <Nazara/Prerequisites.hpp>

namespace Nz::ShaderNodes
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

		Void       //< void
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

		AccessMember,
		AssignOp,
		BinaryOp,
		Branch,
		Cast,
		Constant,
		ConditionalStatement,
		DeclareVariable,
		ExpressionStatement,
		Identifier,
		IntrinsicCall,
		Sample2D,
		SwizzleOp,
		StatementBlock
	};

	enum class SsaInstruction
	{
		OpAdd,
		OpDiv,
		OpMul,
		OpSub,
		OpSample
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
}

#endif // NAZARA_SHADER_ENUMS_HPP
