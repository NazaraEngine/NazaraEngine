// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LUASTATE_HPP
#define NAZARA_LUASTATE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Stream.hpp>
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
			LuaInstance(LuaInstance&& instance) noexcept;
			~LuaInstance();

			void ArgCheck(bool condition, unsigned int argNum, const char* error) const;
			void ArgCheck(bool condition, unsigned int argNum, const String& error) const;
			int ArgError(unsigned int argNum, const char* error) const;
			int ArgError(unsigned int argNum, const String& error) const;

			bool Call(unsigned int argCount);
			bool Call(unsigned int argCount, unsigned int resultCount);

			template<typename T> T Check(int* index) const;
			template<typename T> T Check(int* index, T defValue) const;
			void CheckAny(int index) const;
			bool CheckBoolean(int index) const;
			bool CheckBoolean(int index, bool defValue) const;
			template<typename T> T CheckBoundInteger(int index) const;
			template<typename T> T CheckBoundInteger(int index, T defValue) const;
			template<typename T> T CheckField(const char* fieldName, int tableIndex = -1) const;
			template<typename T> T CheckField(const String& fieldName, int tableIndex = -1) const;
			template<typename T> T CheckField(const char* fieldName, T defValue, int tableIndex = -1) const;
			template<typename T> T CheckField(const String& fieldName, T defValue, int tableIndex = -1) const;
			long long CheckInteger(int index) const;
			long long CheckInteger(int index, long long defValue) const;
			template<typename T> T CheckGlobal(const char* fieldName) const;
			template<typename T> T CheckGlobal(const String& fieldName) const;
			template<typename T> T CheckGlobal(const char* fieldName, T defValue) const;
			template<typename T> T CheckGlobal(const String& fieldName, T defValue) const;
			double CheckNumber(int index) const;
			double CheckNumber(int index, double defValue) const;
			void CheckStack(int space, const char* error = nullptr) const;
			void CheckStack(int space, const String& error) const;
			const char* CheckString(int index, std::size_t* length = nullptr) const;
			const char* CheckString(int index, const char* defValue, std::size_t* length = nullptr) const;
			void CheckType(int index, LuaType type) const;
			void* CheckUserdata(int index, const char* tname) const;
			void* CheckUserdata(int index, const String& tname) const;

			bool Compare(int index1, int index2, LuaComparison comparison) const;
			void Compute(LuaOperation operation) const;

			void Concatenate(int count) const;

			int CreateReference();
			void DestroyReference(int ref);

			String DumpStack() const;

			void Error(const char* message) const;
			void Error(const String& message) const;

			bool Execute(const String& code);
			bool ExecuteFromFile(const String& filePath);
			bool ExecuteFromMemory(const void* data, std::size_t size);
			bool ExecuteFromStream(Stream& stream);

			int GetAbsIndex(int index) const;
			LuaType GetField(const char* fieldName, int tableIndex = -1) const;
			LuaType GetField(const String& fieldName, int tableIndex = -1) const;
			LuaType GetGlobal(const char* name) const;
			LuaType GetGlobal(const String& name) const;
			inline lua_State* GetInternalState() const;
			inline String GetLastError() const;
			inline std::size_t GetMemoryLimit() const;
			inline std::size_t GetMemoryUsage() const;
			LuaType GetMetatable(const char* tname) const;
			LuaType GetMetatable(const String& tname) const;
			bool GetMetatable(int index) const;
			unsigned int GetStackTop() const;
			LuaType GetTable(int index = -2) const;
			inline UInt32 GetTimeLimit() const;
			LuaType GetType(int index) const;
			const char* GetTypeName(LuaType type) const;

			void Insert(int index) const;

			bool IsOfType(int index, LuaType type) const;
			bool IsOfType(int index, const char* tname) const;
			bool IsOfType(int index, const String& tname) const;
			bool IsValid(int index) const;

			long long Length(int index) const;

			void MoveTo(LuaInstance* instance, int n) const;

			bool NewMetatable(const char* str);
			bool NewMetatable(const String& str);
			bool Next(int index = -2) const;

			void Pop(unsigned int n = 1U) const;

			template<typename T> int Push(T arg) const;
			template<typename T, typename T2, typename... Args> int Push(T firstArg, T2 secondArg, Args... args) const;
			void PushBoolean(bool value) const;
			void PushCFunction(LuaCFunction func, unsigned int upvalueCount = 0) const;
			template<typename T> void PushField(const char* name, T&& arg, int tableIndex = -2) const;
			template<typename T> void PushField(const String& name, T&& arg, int tableIndex = -2) const;
			void PushFunction(LuaFunction func) const;
			template<typename R, typename... Args, typename... DefArgs> void PushFunction(R(*func)(Args...), DefArgs&&... defArgs) const;
			template<typename T> void PushGlobal(const char* name, T&& arg);
			template<typename T> void PushGlobal(const String& name, T&& arg);
			template<typename T> void PushInstance(const char* tname, const T& instance) const;
			template<typename T> void PushInstance(const char* tname, T&& instance) const;
			template<typename T, typename... Args> void PushInstance(const char* tname, Args&&... args) const;
			void PushInteger(long long value) const;
			void PushLightUserdata(void* value) const;
			void PushMetatable(const char* str) const;
			void PushMetatable(const String& str) const;
			void PushNil() const;
			void PushNumber(double value) const;
			void PushReference(int ref) const;
			void PushString(const char* str) const;
			void PushString(const char* str, std::size_t size) const;
			void PushString(const String& str) const;
			void PushTable(std::size_t sequenceElementCount = 0, std::size_t arrayElementCount = 0) const;
			void* PushUserdata(std::size_t size) const;
			void PushValue(int index) const;

			void Remove(int index) const;
			void Replace(int index) const;

			void SetField(const char* name, int tableIndex = -2) const;
			void SetField(const String& name, int tableIndex = -2) const;
			void SetGlobal(const char* name);
			void SetGlobal(const String& name);
			void SetMetatable(const char* tname) const;
			void SetMetatable(const String& tname) const;
			void SetMetatable(int index) const;
			void SetMemoryLimit(std::size_t memoryLimit);
			void SetTable(int index = -3) const;
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
			LuaInstance& operator=(LuaInstance&& instance) noexcept;

			static int GetIndexOfUpValue(int upValue);
			static LuaInstance* GetInstance(lua_State* state);

		private:
			template<typename T> T CheckBounds(int index, long long value) const;
			bool Run(int argCount, int resultCount);

			static void* MemoryAllocator(void *ud, void *ptr, std::size_t osize, std::size_t nsize);
			static int ProxyFunc(lua_State* state);
			static void TimeLimiter(lua_State* state, lua_Debug* debug);

			std::size_t m_memoryLimit;
			std::size_t m_memoryUsage;
			UInt32 m_timeLimit;
			Clock m_clock;
			String m_lastError;
			lua_State* m_state;
			unsigned int m_level;
	};
}

#include <Nazara/Lua/LuaInstance.inl>

#endif // NAZARA_LUASTATE_HPP
