// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Lua/LuaState.hpp>
#include <Lua/lauxlib.h>
#include <Lua/lua.h>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Lua/LuaCoroutine.hpp>
#include <Nazara/Lua/LuaInstance.hpp>
#include <Nazara/Lua/Debug.hpp>

namespace Nz
{
	namespace
	{
		LuaType FromLuaType(int type)
		{
			switch (type)
			{
				case LUA_TBOOLEAN:
					return LuaType_Boolean;

				case LUA_TFUNCTION:
					return LuaType_Function;

				case LUA_TLIGHTUSERDATA:
					return LuaType_LightUserdata;

				case LUA_TNIL:
					return LuaType_Nil;

				case LUA_TNONE:
					return LuaType_None;

				case LUA_TNUMBER:
					return LuaType_Number;

				case LUA_TSTRING:
					return LuaType_String;

				case LUA_TTABLE:
					return LuaType_Table;

				case LUA_TTHREAD:
					return LuaType_Thread;

				case LUA_TUSERDATA:
					return LuaType_Userdata;

				default:
					return LuaType_None;
			}
		}

		struct StreamData
		{
			Stream* stream;
			char buffer[NAZARA_CORE_FILE_BUFFERSIZE];
		};

		const char* StreamReader(lua_State* internalState, void* data, std::size_t* size)
		{
			NazaraUnused(internalState);

			StreamData* streamData = static_cast<StreamData*>(data);

			if (streamData->stream->EndOfStream())
				return nullptr;
			else
			{
				*size = streamData->stream->Read(streamData->buffer, NAZARA_CORE_FILE_BUFFERSIZE);
				return streamData->buffer;
			}
		}

		int s_comparisons[] = {
			LUA_OPEQ, // LuaComparison_Equality
			LUA_OPLT, // LuaComparison_Less
			LUA_OPLE  // LuaComparison_LessOrEqual
		};

		static_assert(sizeof(s_comparisons)/sizeof(int) == LuaComparison_Max+1, "Lua comparison array is incomplete");

		int s_operations[] = {
			LUA_OPADD,  // LuaOperation_Addition
			LUA_OPBAND, // LuaOperation_BitwiseAnd
			LUA_OPSHL,  // LuaOperation_BitwiseLeftShift
			LUA_OPBNOT, // LuaOperation_BitwiseNot
			LUA_OPBOR,  // LuaOperation_BitwiseOr
			LUA_OPSHR,  // LuaOperation_BitwiseRightShift
			LUA_OPBXOR, // LuaOperation_BitwiseXOr
			LUA_OPDIV,  // LuaOperation_Division
			LUA_OPPOW,  // LuaOperation_Exponentiation
			LUA_OPIDIV, // LuaOperation_FloorDivision
			LUA_OPMUL,  // LuaOperation_Multiplication
			LUA_OPMOD,  // LuaOperation_Modulo
			LUA_OPUNM,  // LuaOperation_Negation
			LUA_OPSUB   // LuaOperation_Substraction
		};

		static_assert(sizeof(s_operations)/sizeof(int) == LuaOperation_Max+1, "Lua operation array is incomplete");

		int s_types[] = {
			LUA_TBOOLEAN,       // LuaType_Boolean
			LUA_TFUNCTION,      // LuaType_Function
			LUA_TLIGHTUSERDATA, // LuaType_LightUserdata
			LUA_TNIL,           // LuaType_Nil
			LUA_TNUMBER,        // LuaType_Number
			LUA_TNONE,          // LuaType_None
			LUA_TSTRING,        // LuaType_String
			LUA_TTABLE,         // LuaType_Table
			LUA_TTHREAD,        // LuaType_Thread
			LUA_TUSERDATA       // LuaType_Userdata
		};

		static_assert(sizeof(s_types)/sizeof(int) == LuaType_Max+1, "Lua type array is incomplete");
	}

	void LuaState::ArgCheck(bool condition, unsigned int argNum, const char* error) const
	{
		luaL_argcheck(m_state, condition, argNum, error);
	}

	void LuaState::ArgCheck(bool condition, unsigned int argNum, const String& error) const
	{
		luaL_argcheck(m_state, condition, argNum, error.GetConstBuffer());
	}

	int LuaState::ArgError(unsigned int argNum, const char* error) const
	{
		return luaL_argerror(m_state, argNum, error);
	}

	int LuaState::ArgError(unsigned int argNum, const String& error) const
	{
		return luaL_argerror(m_state, argNum, error.GetConstBuffer());
	}

	bool LuaState::Call(unsigned int argCount)
	{
		return Run(argCount, LUA_MULTRET);
	}

	bool LuaState::Call(unsigned int argCount, unsigned int resultCount)
	{
		return Run(argCount, resultCount);
	}

	void LuaState::CheckAny(int index) const
	{
		luaL_checkany(m_state, index);
	}

	bool LuaState::CheckBoolean(int index) const
	{
		if (lua_isnoneornil(m_state, index))
		{
			const char* msg = lua_pushfstring(m_state, "%s expected, got %s", lua_typename(m_state, LUA_TBOOLEAN), luaL_typename(m_state, index));
			luaL_argerror(m_state, index, msg); // Lance une exception
			return false;
		}

		return lua_toboolean(m_state, index) != 0;
	}

	bool LuaState::CheckBoolean(int index, bool defValue) const
	{
		if (lua_isnoneornil(m_state, index))
			return defValue;

		return lua_toboolean(m_state, index) != 0;
	}

	long long LuaState::CheckInteger(int index) const
	{
		return luaL_checkinteger(m_state, index);
	}

	long long LuaState::CheckInteger(int index, long long defValue) const
	{
		return luaL_optinteger(m_state, index, defValue);
	}

	double LuaState::CheckNumber(int index) const
	{
		return luaL_checknumber(m_state, index);
	}

	double LuaState::CheckNumber(int index, double defValue) const
	{
		return luaL_optnumber(m_state, index, defValue);
	}

	void LuaState::CheckStack(int space, const char* error) const
	{
		luaL_checkstack(m_state, space, error);
	}

	void LuaState::CheckStack(int space, const String& error) const
	{
		CheckStack(space, error.GetConstBuffer());
	}

	const char* LuaState::CheckString(int index, std::size_t* length) const
	{
		return luaL_checklstring(m_state, index, length);
	}

	const char* LuaState::CheckString(int index, const char* defValue, std::size_t* length) const
	{
		return luaL_optlstring(m_state, index, defValue, length);
	}

	void LuaState::CheckType(int index, LuaType type) const
	{
		#ifdef NAZARA_DEBUG
		if (type > LuaType_Max)
		{
			NazaraError("Lua type out of enum");
			return;
		}
		#endif

		luaL_checktype(m_state, index, s_types[type]);
	}

	void* LuaState::CheckUserdata(int index, const char* tname) const
	{
		return luaL_checkudata(m_state, index, tname);
	}

	void* LuaState::CheckUserdata(int index, const String& tname) const
	{
		return luaL_checkudata(m_state, index, tname.GetConstBuffer());
	}

	bool LuaState::Compare(int index1, int index2, LuaComparison comparison) const
	{
		#ifdef NAZARA_DEBUG
		if (comparison > LuaComparison_Max)
		{
			NazaraError("Lua comparison out of enum");
			return false;
		}
		#endif

		return (lua_compare(m_state, index1, index2, s_comparisons[comparison]) != 0);
	}

	void LuaState::Compute(LuaOperation operation) const
	{
		#ifdef NAZARA_DEBUG
		if (operation > LuaOperation_Max)
		{
			NazaraError("Lua operation out of enum");
			return;
		}
		#endif

		lua_arith(m_state, s_operations[operation]);
	}

	void LuaState::Concatenate(int count) const
	{
		lua_concat(m_state, count);
	}

	int LuaState::CreateReference()
	{
		return luaL_ref(m_state, LUA_REGISTRYINDEX);
	}

	void LuaState::DestroyReference(int ref)
	{
		luaL_unref(m_state, LUA_REGISTRYINDEX, ref);
	}

	String LuaState::DumpStack() const
	{
		StringStream stream;
		unsigned int stackTop = GetStackTop();
		stream << stackTop << " entries\n";

		for (unsigned int i = 1; i <= stackTop; ++i)
		{
			stream << i << ": ";
			switch (GetType(i))
			{
				case LuaType_Boolean:
					stream << "Boolean(" << ToBoolean(i) << ')';
					break;

				case LuaType_Function:
					stream << "Function(" << ToPointer(i) << ')';
					break;

				case LuaType_LightUserdata:
				case LuaType_Userdata:
					stream << "Userdata(" << ToUserdata(i) << ')';
					break;

				case LuaType_Nil:
					stream << "Nil";
					break;

				case LuaType_None:
					stream << "None";
					break;

				case LuaType_Number:
					stream << "Number(" << ToNumber(i) << ')';
					break;

				case LuaType_String:
					stream << "String(" << ToString(i) << ')';
					break;

				case LuaType_Table:
					stream << "Table(" << ToPointer(i) << ')';
					break;

				case LuaType_Thread:
					stream << "Thread(" << ToPointer(i) << ')';
					break;

				default:
					stream << "Unknown(" << ToPointer(i) << ')';
					break;
			}

			stream << '\n';
		}

		return stream.ToString();
	}

	void LuaState::Error(const char* message) const
	{
		luaL_error(m_state, message);
	}

	void LuaState::Error(const String& message) const
	{
		luaL_error(m_state, message.GetConstBuffer());
	}

	bool LuaState::Execute(const String& code)
	{
		if (code.IsEmpty())
			return true;

		if (!Load(code))
			return false;

		return Call(0);
	}

	bool LuaState::ExecuteFromFile(const String& filePath)
	{
		if (!LoadFromFile(filePath))
			return false;

		return Call(0);
	}

	bool LuaState::ExecuteFromMemory(const void* data, std::size_t size)
	{
		MemoryView stream(data, size);
		return ExecuteFromStream(stream);
	}

	bool LuaState::ExecuteFromStream(Stream& stream)
	{
		if (!LoadFromStream(stream))
			return false;

		return Call(0);
	}

	int LuaState::GetAbsIndex(int index) const
	{
		return lua_absindex(m_state, index);
	}

	LuaType LuaState::GetField(const char* fieldName, int tableIndex) const
	{
		return FromLuaType(lua_getfield(m_state, tableIndex, fieldName));
	}

	LuaType LuaState::GetField(const String& fieldName, int tableIndex) const
	{
		return FromLuaType(lua_getfield(m_state, tableIndex, fieldName.GetConstBuffer()));
	}

	LuaType LuaState::GetGlobal(const char* name) const
	{
		return FromLuaType(lua_getglobal(m_state, name));
	}

	LuaType LuaState::GetGlobal(const String& name) const
	{
		return FromLuaType(lua_getglobal(m_state, name.GetConstBuffer()));
	}

	LuaType LuaState::GetMetatable(const char* tname) const
	{
		return FromLuaType(luaL_getmetatable(m_state, tname));
	}

	LuaType LuaState::GetMetatable(const String& tname) const
	{
		return FromLuaType(luaL_getmetatable(m_state, tname.GetConstBuffer()));
	}

	bool LuaState::GetMetatable(int index) const
	{
		return lua_getmetatable(m_state, index) != 0;
	}

	unsigned int LuaState::GetStackTop() const
	{
		return static_cast<int>(lua_gettop(m_state));
	}

	LuaType LuaState::GetTable(int index) const
	{
		return FromLuaType(lua_gettable(m_state, index));
	}

	LuaType LuaState::GetTableRaw(int index) const
	{
		return FromLuaType(lua_rawget(m_state, index));
	}

	LuaType LuaState::GetType(int index) const
	{
		return FromLuaType(lua_type(m_state, index));
	}

	const char* LuaState::GetTypeName(LuaType type) const
	{
		#ifdef NAZARA_DEBUG
		if (type > LuaType_Max)
		{
			NazaraError("Lua type out of enum");
			return nullptr;
		}
		#endif

		return lua_typename(m_state, s_types[type]);
	}

	void LuaState::Insert(int index) const
	{
		lua_insert(m_state, index);
	}

	bool LuaState::IsOfType(int index, LuaType type) const
	{
		switch (type)
		{
			case LuaType_Boolean:
				return lua_isboolean(m_state, index) != 0;

			case LuaType_Function:
				return lua_isfunction(m_state, index) != 0;

			case LuaType_LightUserdata:
				return lua_islightuserdata(m_state, index) != 0;

			case LuaType_Nil:
				return lua_isnil(m_state, index) != 0;

			case LuaType_None:
				return lua_isnone(m_state, index) != 0;

			case LuaType_Number:
				return lua_isnumber(m_state, index) != 0;

			case LuaType_String:
				return lua_isstring(m_state, index) != 0;

			case LuaType_Table:
				return lua_istable(m_state, index) != 0;

			case LuaType_Thread:
				return lua_isthread(m_state, index) != 0;

			case LuaType_Userdata:
				return lua_isuserdata(m_state, index) != 0;
		}

		NazaraError("Lua type not handled (0x" + String::Number(type, 16) + ')');
		return false;
	}

	bool LuaState::IsOfType(int index, const char* tname) const
	{
		void* ud = luaL_testudata(m_state, index, tname);
		return ud != nullptr;
	}

	bool LuaState::IsOfType(int index, const String& tname) const
	{
		return IsOfType(index, tname.GetConstBuffer());
	}

	bool LuaState::IsValid(int index) const
	{
		return lua_isnoneornil(m_state, index) == 0;
	}

	bool LuaState::Load(const String& code)
	{
		if (luaL_loadstring(m_state, code.GetConstBuffer()) != 0)
		{
			m_lastError = lua_tostring(m_state, -1);
			lua_pop(m_state, 1);

			return false;
		}

		return true;
	}

	bool LuaState::LoadFromFile(const String& filePath)
	{
		File file(filePath);
		if (!file.Open(OpenMode_ReadOnly | OpenMode_Text))
		{
			NazaraError("Failed to open file");
			return false;
		}

		std::size_t length = static_cast<std::size_t>(file.GetSize());

		String source(length, '\0');

		if (file.Read(&source[0], length) != length)
		{
			NazaraError("Failed to read file");
			return false;
		}

		file.Close();

		return Load(source);
	}

	bool LuaState::LoadFromMemory(const void* data, std::size_t size)
	{
		MemoryView stream(data, size);
		return LoadFromStream(stream);
	}

	bool LuaState::LoadFromStream(Stream& stream)
	{
		StreamData data;
		data.stream = &stream;

		if (lua_load(m_state, StreamReader, &data, "C++", nullptr) != 0)
		{
			m_lastError = lua_tostring(m_state, -1);
			lua_pop(m_state, 1);

			return false;
		}

		return true;
	}

	long long LuaState::Length(int index) const
	{
		return luaL_len(m_state, index);
	}

	std::size_t LuaState::LengthRaw(int index) const
	{
		return lua_rawlen(m_state, index);
	}

	void LuaState::MoveTo(LuaState* instance, int n) const
	{
		lua_xmove(m_state, instance->m_state, n);
	}

	LuaCoroutine LuaState::NewCoroutine()
	{
		lua_State* thread = lua_newthread(m_state);
		int ref = luaL_ref(m_state, LUA_REGISTRYINDEX);

		return LuaCoroutine(thread, ref);
	}

	bool LuaState::NewMetatable(const char* str)
	{
		return luaL_newmetatable(m_state, str) != 0;
	}

	bool LuaState::NewMetatable(const String& str)
	{
		return luaL_newmetatable(m_state, str.GetConstBuffer()) != 0;
	}

	bool LuaState::Next(int index) const
	{
		return lua_next(m_state, index) != 0;
	}

	void LuaState::Pop(unsigned int n) const
	{
		lua_pop(m_state, static_cast<int>(n));
	}

	void LuaState::PushBoolean(bool value) const
	{
		lua_pushboolean(m_state, (value) ? 1 : 0);
	}

	void LuaState::PushCFunction(LuaCFunction func, unsigned int upvalueCount) const
	{
		lua_pushcclosure(m_state, func, upvalueCount);
	}

	void LuaState::PushFunction(LuaFunction func) const
	{
		LuaFunction* luaFunc = static_cast<LuaFunction*>(lua_newuserdata(m_state, sizeof(LuaFunction)));
		PlacementNew(luaFunc, std::move(func));

		lua_pushcclosure(m_state, ProxyFunc, 1);
	}

	void LuaState::PushInteger(long long value) const
	{
		lua_pushinteger(m_state, value);
	}

	void LuaState::PushLightUserdata(void* value) const
	{
		lua_pushlightuserdata(m_state, value);
	}

	void LuaState::PushMetatable(const char* str) const
	{
		luaL_getmetatable(m_state, str);
	}

	void LuaState::PushMetatable(const String& str) const
	{
		luaL_getmetatable(m_state, str.GetConstBuffer());
	}

	void LuaState::PushNil() const
	{
		lua_pushnil(m_state);
	}

	void LuaState::PushNumber(double value) const
	{
		lua_pushnumber(m_state, value);
	}

	void LuaState::PushReference(int ref) const
	{
		lua_rawgeti(m_state, LUA_REGISTRYINDEX, ref);
	}

	void LuaState::PushString(const char* str) const
	{
		lua_pushstring(m_state, str);
	}

	void LuaState::PushString(const char* str, std::size_t size) const
	{
		lua_pushlstring(m_state, str, size);
	}

	void LuaState::PushString(const String& str) const
	{
		lua_pushlstring(m_state, str.GetConstBuffer(), str.GetSize());
	}

	void LuaState::PushTable(std::size_t sequenceElementCount, std::size_t arrayElementCount) const
	{
		constexpr std::size_t maxInt = std::numeric_limits<int>::max();
		lua_createtable(m_state, static_cast<int>(std::min(sequenceElementCount, maxInt)), static_cast<int>(std::min(arrayElementCount, maxInt)));
	}

	void* LuaState::PushUserdata(std::size_t size) const
	{
		return lua_newuserdata(m_state, size);
	}

	void LuaState::PushValue(int index) const
	{
		lua_pushvalue(m_state, index);
	}

	void LuaState::Remove(int index) const
	{
		lua_remove(m_state, index);
	}

	void LuaState::Replace(int index) const
	{
		lua_replace(m_state, index);
	}

	void LuaState::SetField(const char* name, int tableIndex) const
	{
		lua_setfield(m_state, tableIndex, name);
	}

	void LuaState::SetField(const String& name, int tableIndex) const
	{
		lua_setfield(m_state, tableIndex, name.GetConstBuffer());
	}

	void LuaState::SetGlobal(const char* name)
	{
		lua_setglobal(m_state, name);
	}

	void LuaState::SetGlobal(const String& name)
	{
		lua_setglobal(m_state, name.GetConstBuffer());
	}

	void LuaState::SetMetatable(const char* tname) const
	{
		luaL_setmetatable(m_state, tname);
	}

	void LuaState::SetMetatable(const String& tname) const
	{
		luaL_setmetatable(m_state, tname.GetConstBuffer());
	}

	void LuaState::SetMetatable(int index) const
	{
		lua_setmetatable(m_state, index);
	}

	void LuaState::SetTable(int index) const
	{
		lua_settable(m_state, index);
	}

	void LuaState::SetTableRaw(int index) const
	{
		lua_rawset(m_state, index);
	}

	bool LuaState::ToBoolean(int index) const
	{
		return lua_toboolean(m_state, index) != 0;
	}

	long long LuaState::ToInteger(int index, bool* succeeded) const
	{
		int success;
		long long result = lua_tointegerx(m_state, index, &success);

		if (succeeded)
			*succeeded = (success != 0);

		return result;
	}

	double LuaState::ToNumber(int index, bool* succeeded) const
	{
		int success;
		double result = lua_tonumberx(m_state, index, &success);

		if (succeeded)
			*succeeded = (success != 0);

		return result;
	}

	const void* LuaState::ToPointer(int index) const
	{
		return lua_topointer(m_state, index);
	}

	const char* LuaState::ToString(int index, std::size_t* length) const
	{
		return lua_tolstring(m_state, index, length);
	}

	void* LuaState::ToUserdata(int index) const
	{
		return lua_touserdata(m_state, index);
	}

	void* LuaState::ToUserdata(int index, const char* tname) const
	{
		return luaL_testudata(m_state, index, tname);
	}

	void* LuaState::ToUserdata(int index, const String& tname) const
	{
		return luaL_testudata(m_state, index, tname.GetConstBuffer());
	}

	bool LuaState::Run(int argCount, int resultCount)
	{
		LuaInstance& instance = GetInstance(m_state);

		if (instance.m_level++ == 0)
			instance.m_clock.Restart();

		int status = lua_pcall(m_state, argCount, resultCount, 0);

		instance.m_level--;

		if (status != 0)
		{
			m_lastError = lua_tostring(m_state, -1);
			lua_pop(m_state, 1);

			return false;
		}

		return true;
	}

	int LuaState::GetIndexOfUpValue(int upValue)
	{
		return lua_upvalueindex(upValue);
	}

	LuaInstance& LuaState::GetInstance(lua_State* internalState)
	{
		LuaInstance* instance;
		lua_getallocf(internalState, reinterpret_cast<void**>(&instance));

		return *instance;
	}

	int LuaState::ProxyFunc(lua_State* internalState)
	{
		LuaFunction& func = *static_cast<LuaFunction*>(lua_touserdata(internalState, lua_upvalueindex(1)));
		LuaState state = GetState(internalState);

		return func(state);
	}

}
