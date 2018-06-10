// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_LUA_HPP
#define NAZARA_ENUMS_LUA_HPP

#include <Nazara/Core/Flags.hpp>

namespace Nz
{
	enum LuaBindMode
	{
		LuaBindMode_Table,
		LuaBindMode_Userdata,

		LuaBindMode_Max = LuaBindMode_Userdata
	};

	enum LuaComparison
	{
		LuaComparison_Equality,
		LuaComparison_Less,
		LuaComparison_LessOrEqual,

		LuaComparison_Max = LuaComparison_LessOrEqual
	};

	enum LuaLib
	{
		LuaLib_Coroutine,
		LuaLib_Debug,
		LuaLib_Math,
		LuaLib_Io,
		LuaLib_Package,
		LuaLib_Os,
		LuaLib_String,
		LuaLib_Table,
		LuaLib_Utf8,

		LuaLib_Max = LuaLib_Utf8
	};

	enum LuaOperation
	{
		LuaOperation_Addition,
		LuaOperation_BitwiseAnd,
		LuaOperation_BitwiseLeftShift,
		LuaOperation_BitwiseNot,
		LuaOperation_BitwiseOr,
		LuaOperation_BitwideRightShift,
		LuaOperation_BitwiseXOr,
		LuaOperation_Division,
		LuaOperation_Exponentiation,
		LuaOperation_FloorDivision,
		LuaOperation_Modulo,
		LuaOperation_Multiplication,
		LuaOperation_Negation,
		LuaOperation_Substraction,

		LuaOperation_Max = LuaOperation_Substraction
	};

	enum LuaType
	{
		LuaType_Boolean,
		LuaType_Function,
		LuaType_LightUserdata,
		LuaType_Nil,
		LuaType_Number,
		LuaType_None,
		LuaType_String,
		LuaType_Table,
		LuaType_Thread,
		LuaType_Userdata,

		LuaType_Max = LuaType_Userdata
	};

	template<>
	struct EnumAsFlags<LuaLib>
	{
		static constexpr LuaLib max = LuaLib_Max;
	};

	using LuaLibFlags = Flags<LuaLib>;

	constexpr LuaLibFlags LuaLib_All = LuaLibFlags(LuaLibFlags::ValueMask);
}

#endif // NAZARA_ENUMS_LUA_HPP
