// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LUASTATE_HPP
#define NAZARA_LUASTATE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Lua/Enums.hpp>
#include <cstddef>
#include <functional>

struct lua_Debug;
struct lua_State;

class NzLuaInstance;

using NzLuaCFunction = int (*)(lua_State* state);
using NzLuaFunction = std::function<int(NzLuaInstance& instance)>;

class NAZARA_API NzLuaInstance : NzNonCopyable
{
	public:
		NzLuaInstance();
		~NzLuaInstance();

		void ArgCheck(bool condition, unsigned int argNum, const char* error);
		void ArgCheck(bool condition, unsigned int argNum, const NzString& error);
		int ArgError(unsigned int argNum, const char* error);
		int ArgError(unsigned int argNum, const NzString& error);

		void CheckAny(int index) const;
		bool CheckBoolean(int index) const;
		bool CheckBoolean(int index, bool defValue) const;
		int CheckInteger(int index) const;
		int CheckInteger(int index, int defValue) const;
		double CheckNumber(int index) const;
		double CheckNumber(int index, double defValue) const;
		void CheckStack(int space, const char* error = nullptr) const;
		void CheckStack(int space, const NzString& error) const;
		const char* CheckString(int index, std::size_t* length = nullptr) const;
		const char* CheckString(int index, const char* defValue, std::size_t* length = nullptr) const;
		void CheckType(int index, nzLuaType type) const;
		unsigned int CheckUnsigned(int index) const;
		unsigned int CheckUnsigned(int index, unsigned int defValue) const;
		void* CheckUserdata(int index, const char* tname) const;
		void* CheckUserdata(int index, const NzString& tname) const;

		bool Compare(int index1, int index2, nzLuaComparison comparison) const;
		void Compute(nzLuaOperation operation);

		void Concatenate(int count);

		int CreateReference();
		void DestroyReference(int ref);

		NzString DumpStack() const;

		void Error(const char* message);
		void Error(const NzString& message);

		bool Execute(const NzString& code);
		bool ExecuteFromFile(const NzString& filePath);
		bool ExecuteFromMemory(const void* data, unsigned int size);
		bool ExecuteFromStream(NzInputStream& stream);

		int GetAbsIndex(int index) const;
		void GetField(const char* fieldName, int index = -1) const;
		void GetField(const NzString& fieldName, int index = -1) const;
		void GetGlobal(const char* name) const;
		void GetGlobal(const NzString& name) const;
		lua_State* GetInternalState() const;
		NzString GetLastError() const;
		nzUInt32 GetMemoryLimit() const;
		nzUInt32 GetMemoryUsage() const;
		void GetMetatable(const char* tname) const;
		void GetMetatable(const NzString& tname) const;
		bool GetMetatable(int index) const;
		unsigned int GetStackTop() const;
		void GetTable(int index = -2) const;
		nzUInt32 GetTimeLimit() const;
		nzLuaType GetType(int index) const;
		const char* GetTypeName(nzLuaType type) const;

		void Insert(int index);

		bool IsOfType(int index, nzLuaType type) const;
		bool IsOfType(int index, const char* tname) const;
		bool IsOfType(int index, const NzString& tname) const;
		bool IsValid(int index) const;

		unsigned int Length(int index) const;

		void MoveTo(NzLuaInstance* instance, int n);

		bool NewMetatable(const char* str);
		bool NewMetatable(const NzString& str);
		bool Next(int index = -2);

		void Pop(unsigned int n = 1U);

		void PushBoolean(bool value);
		void PushCFunction(NzLuaCFunction func, int upvalueCount = 0);
		void PushFunction(NzLuaFunction func);
		void PushInteger(int value);
		void PushLightUserdata(void* value);
		void PushMetatable(const char* str);
		void PushMetatable(const NzString& str);
		void PushNil();
		void PushNumber(double value);
		void PushReference(int ref);
		void PushString(const char* str);
		void PushString(const NzString& str);
		void PushTable(unsigned int sequenceElementCount = 0, unsigned int arrayElementCount = 0);
		void PushUnsigned(unsigned int value);
		void* PushUserdata(unsigned int size);
		void PushValue(int index);

		void Remove(int index);
		void Replace(int index);

		void SetField(const char* name, int index = -2);
		void SetField(const NzString& name, int index = -2);
		void SetGlobal(const char* name);
		void SetGlobal(const NzString& name);
		void SetMetatable(const char* tname);
		void SetMetatable(const NzString& tname);
		void SetMetatable(int index);
		void SetMemoryLimit(nzUInt32 memoryLimit);
		void SetTable(int index = -3);
		void SetTimeLimit(nzUInt32 timeLimit);

		bool ToBoolean(int index) const;
		int ToInteger(int index, bool* succeeded = nullptr) const;
		double ToNumber(int index, bool* succeeded = nullptr) const;
		const void* ToPointer(int index) const;
		const char* ToString(int index, std::size_t* length = nullptr) const;
		unsigned int ToUnsigned(int index, bool* succeeded = nullptr) const;
		void* ToUserdata(int index) const;
		void* ToUserdata(int index, const char* tname) const;
		void* ToUserdata(int index, const NzString& tname) const;

		static int GetIndexOfUpValue(int upValue);
		static NzLuaInstance* GetInstance(lua_State* state);

	private:
		bool Run();

		static void* MemoryAllocator(void *ud, void *ptr, std::size_t osize, std::size_t nsize);
		static int ProxyFunc(lua_State* state);
		static void TimeLimiter(lua_State* state, lua_Debug* debug);

		nzUInt32 m_memoryLimit;
		nzUInt32 m_memoryUsage;
		nzUInt32 m_timeLimit;
		NzClock m_clock;
		NzString m_lastError;
		lua_State* m_state;
		unsigned int m_level;
};

#endif // NAZARA_LUASTATE_HPP
