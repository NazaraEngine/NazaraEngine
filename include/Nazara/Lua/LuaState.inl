// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Flags.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <limits>
#include <string>
#include <vector>
#include <type_traits>

namespace Nz
{
	inline LuaState::LuaState(lua_State* internalState) :
	m_state(internalState)
	{
	}

	inline lua_State* LuaState::GetInternalState() const
	{
		return m_state;
	}

	inline String LuaState::GetLastError() const
	{
		return m_lastError;
	}

	// Functions args
	inline unsigned int LuaImplQueryArg(const LuaState& instance, int index, bool* arg, TypeTag<bool>)
	{
		*arg = instance.CheckBoolean(index);
		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& instance, int index, bool* arg, bool defValue, TypeTag<bool>)
	{
		*arg = instance.CheckBoolean(index, defValue);
		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& instance, int index, std::string* arg, TypeTag<std::string>)
	{
		std::size_t strLength = 0;
		const char* str = instance.CheckString(index, &strLength);

		arg->assign(str, strLength);

		return 1;
	}

	inline unsigned int LuaImplQueryArg(const LuaState& instance, int index, String* arg, TypeTag<String>)
	{
		std::size_t strLength = 0;
		const char* str = instance.CheckString(index, &strLength);

		arg->Set(str, strLength);

		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_enum<T>::value && !IsEnumFlag<T>::value, unsigned int> LuaImplQueryArg(const LuaState& instance, int index, T* arg, TypeTag<T>)
	{
		using UnderlyingT = std::underlying_type_t<T>;
		return LuaImplQueryArg(instance, index, reinterpret_cast<UnderlyingT*>(arg), TypeTag<UnderlyingT>());
	}

	template<typename T>
	std::enable_if_t<std::is_enum<T>::value && !IsEnumFlag<T>::value, unsigned int> LuaImplQueryArg(const LuaState& instance, int index, T* arg, T defValue, TypeTag<T>)
	{
		using UnderlyingT = std::underlying_type_t<T>;
		return LuaImplQueryArg(instance, index, reinterpret_cast<UnderlyingT*>(arg), static_cast<UnderlyingT>(defValue), TypeTag<UnderlyingT>());
	}

	template<typename T>
	std::enable_if_t<std::is_enum<T>::value && IsEnumFlag<T>::value, unsigned int> LuaImplQueryArg(const LuaState& instance, int index, T* arg, TypeTag<T>)
	{
		using UnderlyingT = std::underlying_type_t<T>;

		UnderlyingT pot2Val;
		unsigned int ret = LuaImplQueryArg(instance, index, &pot2Val, TypeTag<UnderlyingT>());

		*arg = static_cast<T>(IntegralLog2Pot(pot2Val));
		return ret;
	}

	template<typename T>
	std::enable_if_t<std::is_enum<T>::value && IsEnumFlag<T>::value, unsigned int> LuaImplQueryArg(const LuaState& instance, int index, T* arg, T defValue, TypeTag<T>)
	{
		using UnderlyingT = std::underlying_type_t<T>;

		UnderlyingT pot2Val;
		unsigned int ret = LuaImplQueryArg(instance, index, &pot2Val, 1U << static_cast<UnderlyingT>(defValue), TypeTag<UnderlyingT>());

		*arg = static_cast<T>(IntegralLog2Pot(pot2Val));
		return ret;
	}

	template<typename E>
	unsigned int LuaImplQueryArg(const LuaState& instance, int index, Flags<E>* arg, TypeTag<Flags<E>>)
	{
		*arg = Flags<E>(instance.CheckBoundInteger<UInt32>(index));
		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_floating_point<T>::value, unsigned int> LuaImplQueryArg(const LuaState& instance, int index, T* arg, TypeTag<T>)
	{
		*arg = static_cast<T>(instance.CheckNumber(index));
		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_floating_point<T>::value, unsigned int> LuaImplQueryArg(const LuaState& instance, int index, T* arg, T defValue, TypeTag<T>)
	{
		*arg = static_cast<T>(instance.CheckNumber(index, static_cast<double>(defValue)));
		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_integral<T>::value, unsigned int> LuaImplQueryArg(const LuaState& instance, int index, T* arg, TypeTag<T>)
	{
		*arg = instance.CheckBoundInteger<T>(index);
		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_integral<T>::value, unsigned int> LuaImplQueryArg(const LuaState& instance, int index, T* arg, T defValue, TypeTag<T>)
	{
		*arg = instance.CheckBoundInteger<T>(index, defValue);
		return 1;
	}

	template<typename T>
	std::enable_if_t<!std::is_integral<T>::value && !std::is_enum<T>::value && !std::is_floating_point<T>::value, unsigned int> LuaImplQueryArg(const LuaState& instance, int index, T* arg, const T& defValue, TypeTag<T> tag)
	{
		if (instance.IsValid(index))
			return LuaImplQueryArg(instance, index, arg, tag);
		else
		{
			*arg = defValue;
			return 1;
		}
	}

	template<typename T>
	unsigned int LuaImplQueryArg(const LuaState& instance, int index, T* arg, TypeTag<const T&>)
	{
		return LuaImplQueryArg(instance, index, arg, TypeTag<T>());
	}

	template<typename T>
	unsigned int LuaImplQueryArg(const LuaState& instance, int index, T* arg, const T& defValue, TypeTag<const T&>)
	{
		return LuaImplQueryArg(instance, index, arg, defValue, TypeTag<T>());
	}

	// Function returns
	inline int LuaImplReplyVal(const LuaState& instance, bool val, TypeTag<bool>)
	{
		instance.PushBoolean(val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& instance, double val, TypeTag<double>)
	{
		instance.PushNumber(val);
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& instance, float val, TypeTag<float>)
	{
		instance.PushNumber(val);
		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_enum<T>::value && !IsEnumFlag<T>::value, int> LuaImplReplyVal(const LuaState& instance, T val, TypeTag<T>)
	{
		using EnumT = typename std::underlying_type<T>::type;
		return LuaImplReplyVal(instance, static_cast<EnumT>(val), TypeTag<EnumT>());
	}

	template<typename T>
	std::enable_if_t<std::is_enum<T>::value && IsEnumFlag<T>::value, int> LuaImplReplyVal(const LuaState& instance, T val, TypeTag<T>)
	{
		Flags<T> flags(val);
		return LuaImplReplyVal(instance, flags, TypeTag<decltype(flags)>());
	}

	template<typename E>
	int LuaImplReplyVal(const LuaState& instance, Flags<E> val, TypeTag<Flags<E>>)
	{
		instance.PushInteger(typename Flags<E>::BitField(val));
		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_integral<T>::value, int> LuaImplReplyVal(const LuaState& instance, T val, TypeTag<T>)
	{
		instance.PushInteger(val);
		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value || std::is_enum<T>::value, int> LuaImplReplyVal(const LuaState& instance, T val, TypeTag<T&>)
	{
		return LuaImplReplyVal(instance, val, TypeTag<T>());
	}

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value || std::is_enum<T>::value, int> LuaImplReplyVal(const LuaState& instance, T val, TypeTag<const T&>)
	{
		return LuaImplReplyVal(instance, val, TypeTag<T>());
	}

	template<typename T>
	std::enable_if_t<!std::is_arithmetic<T>::value && !std::is_enum<T>::value, int> LuaImplReplyVal(const LuaState& instance, T val, TypeTag<T&>)
	{
		return LuaImplReplyVal(instance, std::move(val), TypeTag<T>());
	}

	template<typename T>
	std::enable_if_t<!std::is_arithmetic<T>::value && !std::is_enum<T>::value, int> LuaImplReplyVal(const LuaState& instance, T val, TypeTag<const T&>)
	{
		return LuaImplReplyVal(instance, std::move(val), TypeTag<T>());
	}

	template<typename T>
	int LuaImplReplyVal(const LuaState& instance, T&& val, TypeTag<T&&>)
	{
		return LuaImplReplyVal(instance, std::forward<T>(val), TypeTag<T>());
	}

	inline int LuaImplReplyVal(const LuaState& instance, std::string&& val, TypeTag<std::string>)
	{
		instance.PushString(val.c_str(), val.size());
		return 1;
	}

	template<typename T>
	inline int LuaImplReplyVal(const LuaState& instance, std::vector<T>&& valContainer, TypeTag<std::vector<T>>)
	{
		std::size_t index = 1;
		instance.PushTable(valContainer.size());
		for (T& val : valContainer)
		{
			instance.PushInteger(index++);
			if (LuaImplReplyVal(instance, std::move(val), TypeTag<T>()) != 1)
			{
				instance.Error("Couldn't create table: type need more than one place to store");
				return 0;
			}
			instance.SetTable();
		}

		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& instance, ByteArray&& val, TypeTag<ByteArray>)
	{
		instance.PushString(reinterpret_cast<const char*>(val.GetConstBuffer()), val.GetSize());
		return 1;
	}

	inline int LuaImplReplyVal(const LuaState& instance, String&& val, TypeTag<String>)
	{
		instance.PushString(std::move(val));
		return 1;
	}

	template<typename T1, typename T2>
	int LuaImplReplyVal(const LuaState& instance, std::pair<T1, T2>&& val, TypeTag<std::pair<T1, T2>>)
	{
		int retVal = 0;

		retVal += LuaImplReplyVal(instance, std::move(val.first), TypeTag<T1>());
		retVal += LuaImplReplyVal(instance, std::move(val.second), TypeTag<T2>());

		return retVal;
	}

	template<bool HasDefault>
	struct LuaImplArgProcesser;

	template<>
	struct LuaImplArgProcesser<true>
	{
		template<std::size_t N, std::size_t FirstDefArg, typename ArgType, typename ArgContainer, typename DefArgContainer>
		static unsigned int Process(const LuaState& instance, unsigned int argIndex, ArgContainer& args, DefArgContainer& defArgs)
		{
			return LuaImplQueryArg(instance, argIndex, &std::get<N>(args), std::get<FirstDefArg + std::tuple_size<DefArgContainer>() - N - 1>(defArgs), TypeTag<ArgType>());
		}
	};

	template<>
	struct LuaImplArgProcesser<false>
	{
		template<std::size_t N, std::size_t FirstDefArg, typename ArgType, typename ArgContainer, typename DefArgContainer>
		static unsigned int Process(const LuaState& instance, unsigned int argIndex, ArgContainer& args, DefArgContainer& defArgs)
		{
			NazaraUnused(defArgs);

			return LuaImplQueryArg(instance, argIndex, &std::get<N>(args), TypeTag<ArgType>());
		}
	};

	template<typename... Args>
	class LuaImplFunctionProxy
	{
		public:
			template<typename... DefArgs>
			class Impl
			{
				static constexpr std::size_t ArgCount = sizeof...(Args);
				static constexpr std::size_t DefArgCount = sizeof...(DefArgs);

				static_assert(ArgCount >= DefArgCount, "There cannot be more default arguments than argument");

				static constexpr std::size_t FirstDefArg = ArgCount - DefArgCount;

				public:
					Impl(DefArgs... defArgs) :
					m_defaultArgs(std::forward<DefArgs>(defArgs)...)
					{
					}

					void ProcessArguments(const LuaState& instance) const
					{
						m_index = 1;
						ProcessArgs<0, Args...>(instance);
					}

					int Invoke(const LuaState& instance, void(*func)(Args...)) const
					{
						NazaraUnused(instance);

						Apply(func, m_args);
						return 0;
					}

					template<typename Ret>
					int Invoke(const LuaState& instance, Ret(*func)(Args...)) const
					{
						return LuaImplReplyVal(instance, std::move(Apply(func, m_args)), TypeTag<decltype(Apply(func, m_args))>());
					}

				private:
					using ArgContainer = std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...>;
					using DefArgContainer = std::tuple<std::remove_cv_t<std::remove_reference_t<DefArgs>>...>;

					template<std::size_t N>
					void ProcessArgs(const LuaState& instance) const
					{
						NazaraUnused(instance);

						// No argument to process
					}

					template<std::size_t N, typename ArgType>
					void ProcessArgs(const LuaState& instance) const
					{
						LuaImplArgProcesser<(N >= FirstDefArg)>::template Process<N, FirstDefArg, ArgType>(instance, m_index, m_args, m_defaultArgs);
					}

					template<std::size_t N, typename ArgType1, typename ArgType2, typename... Rest>
					void ProcessArgs(const LuaState& instance) const
					{
						ProcessArgs<N, ArgType1>(instance);
						ProcessArgs<N + 1, ArgType2, Rest...>(instance);
					}

					mutable ArgContainer m_args;
					DefArgContainer m_defaultArgs;
					mutable unsigned int m_index;
			};
	};

	template<typename... Args>
	class LuaImplMethodProxy
	{
		public:
			template<typename... DefArgs>
			class Impl
			{
				static constexpr std::size_t ArgCount = sizeof...(Args);
				static constexpr std::size_t DefArgCount = sizeof...(DefArgs);

				static_assert(ArgCount >= DefArgCount, "There cannot be more default arguments than argument");

				static constexpr std::size_t FirstDefArg = ArgCount - DefArgCount;

				public:
					Impl(DefArgs... defArgs) :
					m_defaultArgs(std::forward<DefArgs>(defArgs)...)
					{
					}

					void ProcessArguments(const LuaState& instance) const
					{
						m_index = 2; //< 1 being the instance
						ProcessArgs<0, Args...>(instance);
					}

					template<typename T, typename P>
					std::enable_if_t<std::is_base_of<P, T>::value, int> Invoke(const LuaState& instance, T& object, void(P::*func)(Args...)) const
					{
						NazaraUnused(instance);

						Apply(object, func, m_args);
						return 0;
					}

					template<typename T, typename P, typename Ret>
					std::enable_if_t<std::is_base_of<P, T>::value, int> Invoke(const LuaState& instance, T& object, Ret(P::*func)(Args...)) const
					{
						return LuaImplReplyVal(instance, std::move(Apply(object, func, m_args)), TypeTag<decltype(Apply(object, func, m_args))>());
					}

					template<typename T, typename P>
					std::enable_if_t<std::is_base_of<P, T>::value, int> Invoke(const LuaState& instance, T& object, T&(P::*func)(Args...)) const
					{
						T& r = Apply(object, func, m_args);
						if (&r == &object)
						{
							instance.PushValue(1); //< Userdata
							return 1;
						}
						else
							return LuaImplReplyVal(instance, r, TypeTag<T&>());
					}

					template<typename T, typename P>
					std::enable_if_t<std::is_base_of<P, T>::value, int> Invoke(const LuaState& instance, const T& object, void(P::*func)(Args...) const) const
					{
						NazaraUnused(instance);

						Apply(object, func, m_args);
						return 0;
					}

					template<typename T, typename P, typename Ret>
					std::enable_if_t<std::is_base_of<P, T>::value, int> Invoke(const LuaState& instance, const T& object, Ret(P::*func)(Args...) const) const
					{
						return LuaImplReplyVal(instance, std::move(Apply(object, func, m_args)), TypeTag<decltype(Apply(object, func, m_args))>());
					}

					template<typename T, typename P>
					std::enable_if_t<std::is_base_of<P, T>::value, int> Invoke(const LuaState& instance, const T& object, const T&(P::*func)(Args...) const) const
					{
						const T& r = Apply(object, func, m_args);
						if (&r == &object)
						{
							instance.PushValue(1); //< Userdata
							return 1;
						}
						else
							return LuaImplReplyVal(instance, r, TypeTag<T&>());
					}

					template<typename T, typename P>
					std::enable_if_t<std::is_base_of<P, typename PointedType<T>::type>::value, int> Invoke(const LuaState& instance, T& object, void(P::*func)(Args...)) const
					{
						if (!object)
						{
							instance.Error("Invalid object");
							return 0;
						}

						Apply(*object, func, m_args);
						return 0;
					}

					template<typename T, typename P, typename Ret>
					std::enable_if_t<std::is_base_of<P, typename PointedType<T>::type>::value, int> Invoke(const LuaState& instance, T& object, Ret(P::*func)(Args...)) const
					{
						if (!object)
						{
							instance.Error("Invalid object");
							return 0;
						}

						return LuaImplReplyVal(instance, std::move(Apply(*object, func, m_args)), TypeTag<decltype(Apply(*object, func, m_args))>());
					}

					template<typename T, typename P>
					std::enable_if_t<std::is_base_of<P, typename PointedType<T>::type>::value, int> Invoke(const LuaState& instance, T& object, typename PointedType<T>::type&(P::*func)(Args...) const) const
					{
						if (!object)
						{
							instance.Error("Invalid object");
							return 0;
						}

						const typename PointedType<T>::type& r = Apply(*object, func, m_args);
						if (&r == &*object)
						{
							instance.PushValue(1); //< Userdata
							return 1;
						}
						else
							return LuaImplReplyVal(instance, r, TypeTag<T&>());
					}

					template<typename T, typename P>
					std::enable_if_t<std::is_base_of<P, typename PointedType<T>::type>::value, int> Invoke(const LuaState& instance, const T& object, void(P::*func)(Args...) const) const
					{
						if (!object)
						{
							instance.Error("Invalid object");
							return 0;
						}

						Apply(*object, func, m_args);
						return 0;
					}

					template<typename T, typename P, typename Ret>
					std::enable_if_t<std::is_base_of<P, typename PointedType<T>::type>::value, int> Invoke(const LuaState& instance, const T& object, Ret(P::*func)(Args...) const) const
					{
						if (!object)
						{
							instance.Error("Invalid object");
							return 0;
						}

						return LuaImplReplyVal(instance, std::move(Apply(*object, func, m_args)), TypeTag<decltype(Apply(*object, func, m_args))>());
					}

					template<typename T, typename P>
					std::enable_if_t<std::is_base_of<P, typename PointedType<T>::type>::value, int> Invoke(const LuaState& instance, const T& object, const typename PointedType<T>::type&(P::*func)(Args...) const) const
					{
						if (!object)
						{
							instance.Error("Invalid object");
							return 0;
						}

						const typename PointedType<T>::type& r = Apply(*object, func, m_args);
						if (&r == &*object)
						{
							instance.PushValue(1); //< Userdata
							return 1;
						}
						else
							return LuaImplReplyVal(instance, r, TypeTag<T&>());
					}

				private:
					using ArgContainer = std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...>;
					using DefArgContainer = std::tuple<std::remove_cv_t<std::remove_reference_t<DefArgs>>...>;

					template<std::size_t N>
					void ProcessArgs(const LuaState& instance) const
					{
						NazaraUnused(instance);

						// No argument to process
					}

					template<std::size_t N, typename ArgType>
					void ProcessArgs(const LuaState& instance) const
					{
						m_index += LuaImplArgProcesser<(N >= FirstDefArg)>::template Process<N, FirstDefArg, ArgType>(instance, m_index, m_args, m_defaultArgs);
					}

					template<std::size_t N, typename ArgType1, typename ArgType2, typename... Rest>
					void ProcessArgs(const LuaState& instance) const
					{
						ProcessArgs<N, ArgType1>(instance);
						ProcessArgs<N + 1, ArgType2, Rest...>(instance);
					}

					mutable ArgContainer m_args;
					DefArgContainer m_defaultArgs;
					mutable unsigned int m_index;
			};
	};

	template<typename T>
	T LuaState::Check(int* index) const
	{
		NazaraAssert(index, "Invalid index pointer");

		T object;
		*index += LuaImplQueryArg(*this, *index, &object, TypeTag<T>());

		return object;
	}

	template<typename T>
	T LuaState::Check(int* index, T defValue) const
	{
		NazaraAssert(index, "Invalid index pointer");

		T object;
		*index += LuaImplQueryArg(*this, *index, &object, defValue, TypeTag<T>());

		return object;
	}

	template<typename T>
	inline T LuaState::CheckBoundInteger(int index) const
	{
		return CheckBounds<T>(index, CheckInteger(index));
	}

	template<typename T>
	inline T LuaState::CheckBoundInteger(int index, T defValue) const
	{
		return CheckBounds<T>(index, CheckInteger(index, defValue));
	}

	template<typename T>
	T LuaState::CheckField(const char* fieldName, int tableIndex) const
	{
		T object;

		GetField(fieldName, tableIndex);
			tableIndex += LuaImplQueryArg(*this, -1, &object, TypeTag<T>());
		Pop();

		return object;
	}

	template<typename T>
	T LuaState::CheckField(const String& fieldName, int tableIndex) const
	{
		return CheckField<T>(fieldName.GetConstBuffer(), tableIndex);
	}

	template<typename T>
	T LuaState::CheckField(const char* fieldName, T defValue, int tableIndex) const
	{
		T object;

		GetField(fieldName, tableIndex);
			tableIndex += LuaImplQueryArg(*this, -1, &object, defValue, TypeTag<T>());
		Pop();

		return object;
	}

	template<typename T>
	T LuaState::CheckField(const String& fieldName, T defValue, int tableIndex) const
	{
		return CheckField<T>(fieldName.GetConstBuffer(), defValue, tableIndex);
	}

	template<typename T>
	T LuaState::CheckGlobal(const char* fieldName) const
	{
		T object;

		GetGlobal(fieldName);
			LuaImplQueryArg(*this, -1, &object, TypeTag<T>());
		Pop();

		return object;
	}

	template<typename T>
	T LuaState::CheckGlobal(const String& fieldName) const
	{
		return CheckGlobal<T>(fieldName.GetConstBuffer());
	}

	template<typename T>
	T LuaState::CheckGlobal(const char* fieldName, T defValue) const
	{
		T object;

		GetGlobal(fieldName);
			LuaImplQueryArg(*this, -1, &object, defValue, TypeTag<T>());
		Pop();

		return object;
	}

	template<typename T>
	T LuaState::CheckGlobal(const String& fieldName, T defValue) const
	{
		return CheckGlobal<T>(fieldName.GetConstBuffer(), defValue);
	}

	template<typename T>
	int LuaState::Push(T arg) const
	{
		return LuaImplReplyVal(*this, std::move(arg), TypeTag<T>());
	}

	template<typename T, typename T2, typename... Args>
	int LuaState::Push(T firstArg, T2 secondArg, Args... args) const
	{
		int valCount = 0;
		valCount += Push(std::move(firstArg));
		valCount += Push(secondArg, std::forward<Args>(args)...);

		return valCount;
	}

	template<typename T>
	void LuaState::PushField(const char* name, T&& arg, int tableIndex) const
	{
		Push(std::forward<T>(arg));
		SetField(name, tableIndex);
	}

	template<typename T>
	void LuaState::PushField(const String& name, T&& arg, int tableIndex) const
	{
		PushField(name.GetConstBuffer(), std::forward<T>(arg), tableIndex);
	}

	template<typename R, typename... Args, typename... DefArgs>
	void LuaState::PushFunction(R(*func)(Args...), DefArgs&&... defArgs) const
	{
		typename LuaImplFunctionProxy<Args...>::template Impl<DefArgs...> handler(std::forward<DefArgs>(defArgs)...);

		PushFunction([func, handler] (LuaState& lua) -> int
		{
			handler.ProcessArguments(lua);

			return handler.Invoke(lua, func);
		});
	}

	template<typename T>
	void LuaState::PushGlobal(const char* name, T&& arg)
	{
		Push(std::forward<T>(arg));
		SetGlobal(name);
	}

	template<typename T>
	void LuaState::PushGlobal(const String& name, T&& arg)
	{
		PushGlobal(name.GetConstBuffer(), std::forward<T>(arg));
	}

	template<typename T>
	void LuaState::PushInstance(const char* tname, T&& instance) const
	{
		T* userdata = static_cast<T*>(PushUserdata(sizeof(T)));
		PlacementNew(userdata, std::move(instance));

		SetMetatable(tname);
	}

	template<typename T, typename... Args>
	void LuaState::PushInstance(const char* tname, Args&&... args) const
	{
		T* userdata = static_cast<T*>(PushUserdata(sizeof(T)));
		PlacementNew(userdata, std::forward<Args>(args)...);

		SetMetatable(tname);
	}

	template<typename T>
	std::enable_if_t<std::is_signed<T>::value, T> LuaState::CheckBounds(int index, long long value) const
	{
		constexpr long long minBounds = std::numeric_limits<T>::min();
		constexpr long long maxBounds = std::numeric_limits<T>::max();
		if (value < minBounds || value > maxBounds)
		{
			Nz::StringStream stream;
			stream << "Argument #" << index << " is outside value range [" << minBounds << ", " << maxBounds << "] (" << value << ')';
			Error(stream);
		}

		return static_cast<T>(value);
	}

	template<typename T>
	std::enable_if_t<std::is_unsigned<T>::value, T> LuaState::CheckBounds(int index, long long value) const
	{
		unsigned long long uValue = static_cast<unsigned long long>(value);
		constexpr unsigned long long minBounds = 0;
		constexpr unsigned long long maxBounds = std::numeric_limits<T>::max();
		if (uValue < minBounds || uValue > maxBounds)
		{
			Nz::StringStream stream;
			stream << "Argument #" << index << " is outside value range [" << minBounds << ", " << maxBounds << "] (" << value << ')';
			Error(stream);
		}

		return static_cast<T>(uValue);
	}

	inline LuaState LuaState::GetState(lua_State* internalState)
	{
		return LuaState(internalState);
	}
}
