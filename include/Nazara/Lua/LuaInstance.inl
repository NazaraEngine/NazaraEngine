// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <string>
#include <type_traits>

namespace Nz
{
	// Functions args
	inline unsigned int LuaImplQueryArg(LuaInstance& instance, int index, bool* arg, TypeTag<bool>)
	{
		*arg = instance.CheckBoolean(index);
		return 1;
	}

	inline unsigned int LuaImplQueryArg(LuaInstance& instance, int index, bool* arg, bool defValue, TypeTag<bool>)
	{
		*arg = instance.CheckBoolean(index, defValue);
		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_enum<T>::value, unsigned int> LuaImplQueryArg(LuaInstance& instance, int index, T* arg, TypeTag<T>)
	{
		using UnderlyingT = std::underlying_type_t<T>;
		*arg = static_cast<T>(LuaImplQueryArg(instance, index, reinterpret_cast<UnderlyingT*>(arg), TypeTag<UnderlyingT>()));

		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_enum<T>::value, unsigned int> LuaImplQueryArg(LuaInstance& instance, int index, T* arg, T defValue, TypeTag<T>)
	{
		using UnderlyingT = std::underlying_type_t<T>;
		*arg = static_cast<T>(LuaImplQueryArg(instance, index, reinterpret_cast<UnderlyingT*>(arg), static_cast<UnderlyingT>(defValue), TypeTag<UnderlyingT>()));

		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_floating_point<T>::value, unsigned int> LuaImplQueryArg(LuaInstance& instance, int index, T* arg, TypeTag<T>)
	{
		*arg = static_cast<T>(instance.CheckNumber(index));
		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_floating_point<T>::value, unsigned int> LuaImplQueryArg(LuaInstance& instance, int index, T* arg, T defValue, TypeTag<T>)
	{
		*arg = static_cast<T>(instance.CheckNumber(index, static_cast<double>(defValue)));
		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_integral<T>::value && !std::is_unsigned<T>::value, unsigned int> LuaImplQueryArg(LuaInstance& instance, int index, T* arg, TypeTag<T>)
	{
		*arg = static_cast<T>(instance.CheckInteger(index));
		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_integral<T>::value && !std::is_unsigned<T>::value, unsigned int> LuaImplQueryArg(LuaInstance& instance, int index, T* arg, T defValue, TypeTag<T>)
	{
		*arg = static_cast<T>(instance.CheckInteger(index, defValue));
		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_unsigned<T>::value, unsigned int> LuaImplQueryArg(LuaInstance& instance, int index, T* arg, TypeTag<T>)
	{
		using SignedT = std::make_signed_t<T>;
		return LuaImplQueryArg(instance, index, reinterpret_cast<SignedT*>(arg), TypeTag<SignedT>());
	}

	template<typename T>
	std::enable_if_t<std::is_unsigned<T>::value, unsigned int> LuaImplQueryArg(LuaInstance& instance, int index, T* arg, T defValue, TypeTag<T>)
	{
		using SignedT = std::make_signed_t<T>;
		
		return LuaImplQueryArg(instance, index, reinterpret_cast<SignedT*>(arg), static_cast<SignedT>(defValue), TypeTag<SignedT>());
	}

	inline unsigned int LuaImplQueryArg(LuaInstance& instance, int index, std::string* arg, TypeTag<std::string>)
	{
		std::size_t strLength = 0;
		const char* str = instance.CheckString(index, &strLength);

		arg->assign(str, strLength);

		return 1;
	}

	inline unsigned int LuaImplQueryArg(LuaInstance& instance, int index, String* arg, TypeTag<String>)
	{
		std::size_t strLength = 0;
		const char* str = instance.CheckString(index, &strLength);

		arg->Set(str, strLength);

		return 1;
	}

	template<typename T>
	unsigned int LuaImplQueryArg(LuaInstance& instance, int index, T* arg, const T& defValue, TypeTag<T> tag)
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
	unsigned int LuaImplQueryArg(LuaInstance& instance, int index, T* arg, TypeTag<const T&>)
	{
		return LuaImplQueryArg(instance, index, arg, TypeTag<T>());
	}

	template<typename T>
	unsigned int LuaImplQueryArg(LuaInstance& instance, int index, T* arg, const T& defValue, TypeTag<const T&>)
	{
		return LuaImplQueryArg(instance, index, arg, defValue, TypeTag<T>());
	}

	// Function returns
	inline int LuaImplReplyVal(LuaInstance& instance, bool val, TypeTag<bool>)
	{
		instance.PushBoolean(val);
		return 1;
	}

	inline int LuaImplReplyVal(LuaInstance& instance, double val, TypeTag<double>)
	{
		instance.PushNumber(val);
		return 1;
	}

	inline int LuaImplReplyVal(LuaInstance& instance, float val, TypeTag<float>)
	{
		instance.PushNumber(val);
		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_enum<T>::value, int> LuaImplReplyVal(LuaInstance& instance, T val, TypeTag<T>)
	{
		using EnumT = typename std::underlying_type<T>::type;

		return LuaImplReplyVal(instance, static_cast<EnumT>(val), TypeTag<EnumT>());
	}

	template<typename T>
	std::enable_if_t<std::is_integral<T>::value && !std::is_unsigned<T>::value, int> LuaImplReplyVal(LuaInstance& instance, T val, TypeTag<T>)
	{
		instance.PushInteger(val);
		return 1;
	}

	template<typename T>
	std::enable_if_t<std::is_unsigned<T>::value, int> LuaImplReplyVal(LuaInstance& instance, T val, TypeTag<T>)
	{
		using SignedT = typename std::make_signed<T>::type;

		return LuaImplReplyVal(instance, static_cast<SignedT>(val), TypeTag<SignedT>());
	}

	inline int LuaImplReplyVal(LuaInstance& instance, std::string val, TypeTag<std::string>)
	{
		instance.PushString(val.c_str(), val.size());
		return 1;
	}

	inline int LuaImplReplyVal(LuaInstance& instance, String val, TypeTag<String>)
	{
		instance.PushString(std::move(val));
		return 1;
	}

	template<typename T1, typename T2>
	int LuaImplReplyVal(LuaInstance& instance, std::pair<T1, T2> val, TypeTag<std::pair<T1, T2>>)
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
		static unsigned int Process(LuaInstance& instance, unsigned int argIndex, ArgContainer& args, DefArgContainer& defArgs)
		{
			return LuaImplQueryArg(instance, argIndex, &std::get<N>(args), std::get<std::tuple_size<DefArgContainer>() - N + FirstDefArg - 1>(defArgs), TypeTag<ArgType>());
		}
	};

	template<>
	struct LuaImplArgProcesser<false>
	{
		template<std::size_t N, std::size_t FirstDefArg, typename ArgType, typename ArgContainer, typename DefArgContainer>
		static unsigned int Process(LuaInstance& instance, unsigned int argIndex, ArgContainer& args, DefArgContainer& defArgs)
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
					Impl(LuaInstance& instance, DefArgs... defArgs) :
					m_defaultArgs(std::forward<DefArgs>(defArgs)...)
					{
					}

					void ProcessArgs(LuaInstance& instance) const
					{
						m_index = 1;
						ProcessArgs<0, Args...>(instance);
					}

					int Invoke(LuaInstance& instance, void (*func)(Args...)) const
					{
						NazaraUnused(instance);

						Apply(func, m_args);
						return 0;
					}

					template<typename Ret>
					int Invoke(LuaInstance& instance, Ret (*func)(Args...)) const
					{
						return LuaImplReplyVal(instance, std::move(Apply(func, m_args)), TypeTag<decltype(Apply(func, m_args))>());
					}

				private:
					using ArgContainer = std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...>;
					using DefArgContainer = std::tuple<std::remove_cv_t<std::remove_reference_t<DefArgs>>...>;

					template<std::size_t N>
					void ProcessArgs(LuaInstance& instance) const
					{
						NazaraUnused(instance);

						// No argument to process
					}

					template<std::size_t N, typename ArgType>
					void ProcessArgs(LuaInstance& instance) const
					{
						LuaImplArgProcesser<(N >= FirstDefArg)>::template Process<N, FirstDefArg, ArgType>(instance, &m_index, m_args, m_defaultArgs);
					}

					template<std::size_t N, typename ArgType1, typename ArgType2, typename... Rest>
					void ProcessArgs(LuaInstance& instance) const
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

					void ProcessArgs(LuaInstance& instance) const
					{
						m_index = 1;
						ProcessArgs<0, Args...>(instance);
					}

					template<typename T, typename P>
					std::enable_if_t<std::is_base_of<P, T>::value, int> Invoke(LuaInstance& instance, T& object, void(P::*func)(Args...)) const
					{
						NazaraUnused(instance);

						Apply(object, func, m_args);
						return 0;
					}

					template<typename T, typename P, typename Ret>
					std::enable_if_t<std::is_base_of<P, T>::value, int> Invoke(LuaInstance& instance, T& object, Ret(P::*func)(Args...)) const
					{
						return LuaImplReplyVal(instance, std::move(Apply(object, func, m_args)), TypeTag<decltype(Apply(object, func, m_args))>());
					}

					template<typename T, typename P>
					std::enable_if_t<std::is_base_of<P, T>::value, int> Invoke(LuaInstance& instance, const T& object, void(P::*func)(Args...) const) const
					{
						NazaraUnused(instance);

						Apply(object, func, m_args);
						return 0;
					}

					template<typename T, typename P, typename Ret>
					std::enable_if_t<std::is_base_of<P, T>::value, int> Invoke(LuaInstance& instance, const T& object, Ret(P::*func)(Args...) const) const
					{
						return LuaImplReplyVal(instance, std::move(Apply(object, func, m_args)), TypeTag<decltype(Apply(object, func, m_args))>());
					}

					template<typename T, typename P>
					std::enable_if_t<std::is_base_of<P, typename PointedType<T>::type>::value, int> Invoke(LuaInstance& instance, T& object, void(P::*func)(Args...)) const
					{
						NazaraUnused(instance);

						Apply(*object, func, m_args);
						return 0;
					}

					template<typename T, typename P, typename Ret>
					std::enable_if_t<std::is_base_of<P, typename PointedType<T>::type>::value, int> Invoke(LuaInstance& instance, T& object, Ret(P::*func)(Args...)) const
					{
						return LuaImplReplyVal(instance, std::move(Apply(*object, func, m_args)), TypeTag<decltype(Apply(*object, func, m_args))>());
					}

					template<typename T, typename P>
					std::enable_if_t<std::is_base_of<P, typename PointedType<T>::type>::value, int> Invoke(LuaInstance& instance, const T& object, void(P::*func)(Args...) const) const
					{
						NazaraUnused(instance);

						Apply(*object, func, m_args);
						return 0;
					}

					template<typename T, typename P, typename Ret>
					std::enable_if_t<std::is_base_of<P, typename PointedType<T>::type>::value, int> Invoke(LuaInstance& instance, const T& object, Ret(P::*func)(Args...) const) const
					{
						return LuaImplReplyVal(instance, std::move(Apply(*object, func, m_args)), TypeTag<decltype(Apply(*object, func, m_args))>());
					}

				private:
					using ArgContainer = std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...>;
					using DefArgContainer = std::tuple<std::remove_cv_t<std::remove_reference_t<DefArgs>>...>;

					template<std::size_t N>
					void ProcessArgs(LuaInstance& instance) const
					{
						NazaraUnused(instance);

						// No argument to process
					}

					template<std::size_t N, typename ArgType>
					void ProcessArgs(LuaInstance& instance) const
					{
						m_index += LuaImplArgProcesser<(N >= FirstDefArg)>::template Process<N, FirstDefArg, ArgType>(instance, m_index, m_args, m_defaultArgs);
					}

					template<std::size_t N, typename ArgType1, typename ArgType2, typename... Rest>
					void ProcessArgs(LuaInstance& instance) const
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
	T LuaInstance::Check(unsigned int* index)
	{
		NazaraAssert(index, "Invalid index pointer");

		T object;
		*index += LuaImplQueryArg(*this, index, &object, TypeTag<T>());

		return object;
	}

	template<typename T>
	T LuaInstance::Check(unsigned int* index, T defValue)
	{
		NazaraAssert(index, "Invalid index pointer");

		T object;
		*index += LuaImplQueryArg(*this, index, &object, defValue, TypeTag<T>());

		return object;
	}
	
	template<typename T>
	int LuaInstance::Push(T arg)
	{
		return LuaImplReplyVal(*this, std::move(arg), TypeTag<T>());
	}

	template<typename R, typename... Args, typename... DefArgs>
	void LuaInstance::PushFunction(R (*func)(Args...), DefArgs&&... defArgs)
	{
		typename LuaImplFunctionProxy<Args...>::template Impl<DefArgs...> handler(std::forward<DefArgs>(defArgs)...);

		PushFunction([func, handler](LuaInstance& lua) -> int
		{
			handler.ProcessArgs(lua);

			return handler.Invoke(lua, func);
		});
	}

	template<typename T>
	void LuaInstance::PushInstance(const char* tname, T* instance)
	{
		T** userdata = static_cast<T**>(PushUserdata(sizeof(T*)));
		*userdata = instance;
		SetMetatable(tname);
	}

	template<typename T, typename... Args>
	void LuaInstance::PushInstance(const char* tname, Args&&... args)
	{
		PushInstance(tname, new T(std::forward<Args>(args)...));
	}
}
