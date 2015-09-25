// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LUASTATE_HPP
#define NAZARA_LUASTATE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Lua/Config.hpp>
#include <Nazara/Lua/Enums.hpp>
#include <cstddef>
#include <functional>

struct lua_Debug;
struct lua_State;

namespace Nz
{
	class LuaInstance;

	using LuaCFunction = int (*)(lua_State* state);
	using LuaFunction = std::function<int(LuaInstance& instance)>;

	class NAZARA_LUA_API LuaInstance
	{
		public:
			LuaInstance();
			LuaInstance(const LuaInstance&) = delete;
			LuaInstance(LuaInstance&&) = delete; ///TODO
			~LuaInstance();

			void ArgCheck(bool condition, unsigned int argNum, const char* error);
			void ArgCheck(bool condition, unsigned int argNum, const String& error);
			int ArgError(unsigned int argNum, const char* error);
			int ArgError(unsigned int argNum, const String& error);

			bool Call(unsigned int argCount);
			bool Call(unsigned int argCount, unsigned int resultCount);

			void CheckAny(int index) const;
			bool CheckBoolean(int index) const;
			bool CheckBoolean(int index, bool defValue) const;
			long long CheckInteger(int index) const;
			long long CheckInteger(int index, long long defValue) const;
			double CheckNumber(int index) const;
			double CheckNumber(int index, double defValue) const;
			void CheckStack(int space, const char* error = nullptr) const;
			void CheckStack(int space, const String& error) const;
			const char* CheckString(int index, std::size_t* length = nullptr) const;
			const char* CheckString(int index, const char* defValue, std::size_t* length = nullptr) const;
			void CheckType(int index, nzLuaType type) const;
			void* CheckUserdata(int index, const char* tname) const;
			void* CheckUserdata(int index, const String& tname) const;

			bool Compare(int index1, int index2, nzLuaComparison comparison) const;
			void Compute(nzLuaOperation operation);

			void Concatenate(int count);

			int CreateReference();
			void DestroyReference(int ref);

			String DumpStack() const;

			void Error(const char* message);
			void Error(const String& message);

			bool Execute(const String& code);
			bool ExecuteFromFile(const String& filePath);
			bool ExecuteFromMemory(const void* data, unsigned int size);
			bool ExecuteFromStream(InputStream& stream);

			int GetAbsIndex(int index) const;
			nzLuaType GetField(const char* fieldName, int index = -1) const;
			nzLuaType GetField(const String& fieldName, int index = -1) const;
			nzLuaType GetGlobal(const char* name) const;
			nzLuaType GetGlobal(const String& name) const;
			lua_State* GetInternalState() const;
			String GetLastError() const;
			UInt32 GetMemoryLimit() const;
			UInt32 GetMemoryUsage() const;
			nzLuaType GetMetatable(const char* tname) const;
			nzLuaType GetMetatable(const String& tname) const;
			bool GetMetatable(int index) const;
			unsigned int GetStackTop() const;
			nzLuaType GetTable(int index = -2) const;
			UInt32 GetTimeLimit() const;
			nzLuaType GetType(int index) const;
			const char* GetTypeName(nzLuaType type) const;

			void Insert(int index);

			bool IsOfType(int index, nzLuaType type) const;
			bool IsOfType(int index, const char* tname) const;
			bool IsOfType(int index, const String& tname) const;
			bool IsValid(int index) const;

			long long Length(int index) const;

			void MoveTo(LuaInstance* instance, int n);

			bool NewMetatable(const char* str);
			bool NewMetatable(const String& str);
			bool Next(int index = -2);

			void Pop(unsigned int n = 1U);

			void PushBoolean(bool value);
			void PushCFunction(LuaCFunction func, unsigned int upvalueCount = 0);
			void PushFunction(LuaFunction func);
			template<typename R, typename... Args> void PushFunction(R(*func)(Args...));
			void PushInteger(long long value);
			void PushLightUserdata(void* value);
			void PushMetatable(const char* str);
			void PushMetatable(const String& str);
			void PushNil();
			void PushNumber(double value);
			void PushReference(int ref);
			void PushString(const char* str);
			void PushString(const char* str, unsigned int size);
			void PushString(const String& str);
			void PushTable(unsigned int sequenceElementCount = 0, unsigned int arrayElementCount = 0);
			void* PushUserdata(unsigned int size);
			void PushValue(int index);

			void Remove(int index);
			void Replace(int index);

			void SetField(const char* name, int index = -2);
			void SetField(const String& name, int index = -2);
			void SetGlobal(const char* name);
			void SetGlobal(const String& name);
			void SetMetatable(const char* tname);
			void SetMetatable(const String& tname);
			void SetMetatable(int index);
			void SetMemoryLimit(UInt32 memoryLimit);
			void SetTable(int index = -3);
			void SetTimeLimit(UInt32 timeLimit);

			bool ToBoolean(int index) const;
			long long ToInteger(int index, bool* succeeded = nullptr) const;
			double ToNumber(int index, bool* succeeded = nullptr) const;
			const void* ToPointer(int index) const;
			const char* ToString(int index, std::size_t* length = nullptr) const;
			void* ToUserdata(int index) const;
			void* ToUserdata(int index, const char* tname) const;
			void* ToUserdata(int index, const String& tname) const;

			LuaInstance& operator=(const LuaInstance&) = delete;
			LuaInstance& operator=(LuaInstance&&) = delete; ///TODO

			static int GetIndexOfUpValue(int upValue);
			static LuaInstance* GetInstance(lua_State* state);

		private:
			bool Run(int argCount, int resultCount);

			static void* MemoryAllocator(void *ud, void *ptr, std::size_t osize, std::size_t nsize);
			static int ProxyFunc(lua_State* state);
			static void TimeLimiter(lua_State* state, lua_Debug* debug);

			UInt32 m_memoryLimit;
			UInt32 m_memoryUsage;
			UInt32 m_timeLimit;
			Clock m_clock;
			String m_lastError;
			lua_State* m_state;
			unsigned int m_level;
	};
}

#include <Nazara/Lua/LuaInstance.inl>

#endif // NAZARA_LUASTATE_HPP
