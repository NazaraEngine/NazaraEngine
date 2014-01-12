// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_LUA_HPP
#define NAZARA_ENUMS_LUA_HPP

enum nzLuaComparison
{
	nzLuaComparison_Equality,
	nzLuaComparison_Less,
	nzLuaComparison_LessOrEqual,

	nzLuaComparison_Max = nzLuaComparison_LessOrEqual
};

enum nzLuaOperation
{
	nzLuaOperation_Addition,
	nzLuaOperation_Division,
	nzLuaOperation_Exponentiation,
	nzLuaOperation_Modulo,
	nzLuaOperation_Multiplication,
	nzLuaOperation_Negation,
	nzLuaOperation_Substraction,

	nzLuaOperation_Max = nzLuaOperation_Substraction
};

enum nzLuaType
{
	nzLuaType_Boolean,
	nzLuaType_Function,
	nzLuaType_LightUserdata,
	nzLuaType_Nil,
	nzLuaType_Number,
	nzLuaType_None,
	nzLuaType_String,
	nzLuaType_Table,
	nzLuaType_Thread,
	nzLuaType_Userdata,

	nzLuaType_Max = nzLuaType_Userdata
};

#endif // NAZARA_ENUMS_LUA_HPP
