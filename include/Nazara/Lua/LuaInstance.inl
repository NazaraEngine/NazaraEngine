// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <string>
#include <type_traits>

namespace Nz
{
	// Functions args
	inline bool LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<bool>)
	{
		return instance.CheckBoolean(index);
	}

	inline bool LuaImplQueryArg(LuaInstance& instance, unsigned int index, bool defValue, TypeTag<bool>)
	{
		return instance.CheckBoolean(index, defValue);
	}

	inline double LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<double>)
	{
		return instance.CheckNumber(index);
	}

	inline double LuaImplQueryArg(LuaInstance& instance, unsigned int index, double defValue, TypeTag<double>)
	{
		return instance.CheckNumber(index, defValue);
	}

	inline float LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<float>)
	{
		return static_cast<float>(instance.CheckNumber(index));
	}

	inline float LuaImplQueryArg(LuaInstance& instance, unsigned int index, float defValue, TypeTag<float>)
	{
		return static_cast<float>(instance.CheckNumber(index, defValue));
	}

	template<typename T>
	std::enable_if_t<std::is_enum<T>::value, T> LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<T>)
	{
		return static_cast<T>(LuaImplQueryArg(instance, index, TypeTag<typename std::underlying_type<T>::type>()));
	}

	template<typename T>
	std::enable_if_t<std::is_enum<T>::value, T> LuaImplQueryArg(LuaInstance& instance, unsigned int index, T defValue, TypeTag<T>)
	{
		using UnderlyingT = std::underlying_type_t<T>;
		return static_cast<T>(LuaImplQueryArg(instance, index, static_cast<UnderlyingT>(defValue), TypeTag<UnderlyingT>()));
	}

	template<typename T>
	std::enable_if_t<std::is_integral<T>::value && !std::is_unsigned<T>::value, T> LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<T>)
	{
		return static_cast<T>(instance.CheckInteger(index));
	}

	template<typename T>
	std::enable_if_t<std::is_integral<T>::value && !std::is_unsigned<T>::value, T> LuaImplQueryArg(LuaInstance& instance, unsigned int index, T defValue, TypeTag<T>)
	{
		return static_cast<T>(instance.CheckInteger(index, defValue));
	}

	template<typename T>
	std::enable_if_t<std::is_unsigned<T>::value, T> LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<T>)
	{
		using SignedT = std::make_signed_t<T>;
		return static_cast<T>(LuaImplQueryArg(instance, index, TypeTag<SignedT>()));
	}

	template<typename T>
	std::enable_if_t<std::is_unsigned<T>::value, T> LuaImplQueryArg(LuaInstance& instance, unsigned int index, T defValue, TypeTag<T>)
	{
		using SignedT = std::make_signed_t<T>;
		return static_cast<T>(LuaImplQueryArg(instance, index, static_cast<SignedT>(defValue), TypeTag<SignedT>()));
	}

	inline std::string LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<std::string>)
	{
		std::size_t strLength = 0;
		const char* str = instance.CheckString(index, &strLength);

		return std::string(str, strLength);
	}

	inline std::string LuaImplQueryArg(LuaInstance& instance, unsigned int index, const std::string& defValue, TypeTag<std::string>)
	{
		std::size_t strLength = 0;
		const char* str = instance.CheckString(index, defValue.c_str(), &strLength);

		return std::string(str, strLength);
	}

	inline String LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<String>)
	{
		std::size_t strLength = 0;
		const char* str = instance.CheckString(index, &strLength);

		return String(str, strLength);
	}

	inline String LuaImplQueryArg(LuaInstance& instance, unsigned int index, const String& defValue, TypeTag<String>)
	{
		std::size_t strLength = 0;
		const char* str = instance.CheckString(index, defValue.GetConstBuffer(), &strLength);

		return String(str, strLength);
	}

	template<typename T>
	T LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<const T&>)
	{
		return LuaImplQueryArg(instance, index, TypeTag<T>());
	}

	template<typename T>
	T LuaImplQueryArg(LuaInstance& instance, unsigned int index, const T& defValue, TypeTag<T> tag)
	{
		if (instance.IsValid(index))
			return LuaImplQueryArg(instance, index, tag);
		else
			return defValue;
	}

	template<typename T>
	T LuaImplQueryArg(LuaInstance& instance, unsigned int index, const T& defValue, TypeTag<const T&>)
	{
		return LuaImplQueryArg(instance, index, defValue, TypeTag<T>());
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
		static void Process(LuaInstance& instance, ArgContainer& args, DefArgContainer& defArgs)
		{
			std::get<N>(args) = std::move(LuaImplQueryArg(instance, N + 1, std::get<std::tuple_size<DefArgContainer>() - N + FirstDefArg - 1>(defArgs), TypeTag<ArgType>()));
		}
	};

	template<>
	struct LuaImplArgProcesser<false>
	{
		template<std::size_t N, std::size_t FirstDefArg, typename ArgType, typename ArgContainer, typename DefArgContainer>
		static void Process(LuaInstance& instance, ArgContainer& args, DefArgContainer& defArgs)
		{
			NazaraUnused(defArgs);

			std::get<N>(args) = std::move(LuaImplQueryArg(instance, N + 1, TypeTag<ArgType>()));
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
					m_defaultArgs(std::forward<DefArgs>(defArgs)...),
					m_instance(instance)
					{
					}

					void ProcessArgs()
					{
						ProcessArgs<0, Args...>();
					}

					int Invoke(void (*func)(Args...))
					{
						Apply(func, m_args);
						return 0;
					}

					template<typename Ret>
					int Invoke(Ret (*func)(Args...))
					{
						return LuaImplReplyVal(m_instance, std::move(Apply(func, m_args)), TypeTag<decltype(Apply(func, m_args))>());
					}

				private:
					using ArgContainer = std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...>;
					using DefArgContainer = std::tuple<std::remove_cv_t<std::remove_reference_t<DefArgs>>...>;

					template<std::size_t N>
					void ProcessArgs()
					{
						// No argument to process
					}

					template<std::size_t N, typename ArgType>
					void ProcessArgs()
					{
						LuaImplArgProcesser<(N >= FirstDefArg)>::template Process<N, FirstDefArg, ArgType>(m_instance, m_args, m_defaultArgs);
					}

					template<std::size_t N, typename ArgType1, typename ArgType2, typename... Rest>
					void ProcessArgs()
					{
						ProcessArgs<N, ArgType1>();
						ProcessArgs<N + 1, ArgType2, Rest...>();
					}

					ArgContainer m_args;
					DefArgContainer m_defaultArgs;
					LuaInstance& m_instance;
			};
	};

	template<typename T, typename... Args>
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
					Impl(LuaInstance& instance, T& object, DefArgs... defArgs) :
					m_defaultArgs(std::forward<DefArgs>(defArgs)...),
					m_instance(instance),
					m_object(object)
					{
					}

					void ProcessArgs()
					{
						ProcessArgs<0, Args...>();
					}

					template<typename P>
					std::enable_if_t<std::is_base_of<P, T>::value, int> Invoke(void(P::*func)(Args...))
					{
						Apply(m_object, func, m_args);
						return 0;
					}

					template<typename P, typename Ret>
					std::enable_if_t<std::is_base_of<P, T>::value, int> Invoke(Ret(P::*func)(Args...))
					{
						return LuaImplReplyVal(m_instance, std::move(Apply(m_object, func, m_args)), TypeTag<decltype(Apply(m_object, func, m_args))>());
					}

					template<typename P>
					std::enable_if_t<std::is_base_of<P, T>::value, int> Invoke(void(P::*func)(Args...) const)
					{
						Apply(m_object, func, m_args);
						return 0;
					}

					template<typename P, typename Ret>
					std::enable_if_t<std::is_base_of<P, T>::value, int> Invoke(Ret(P::*func)(Args...) const)
					{
						return LuaImplReplyVal(m_instance, std::move(Apply(m_object, func, m_args)), TypeTag<decltype(Apply(m_object, func, m_args))>());
					}

				private:
					using ArgContainer = std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...>;
					using DefArgContainer = std::tuple<std::remove_cv_t<std::remove_reference_t<DefArgs>>...>;

					template<std::size_t N>
					void ProcessArgs()
					{
						// No argument to process
					}

					template<std::size_t N, typename ArgType>
					void ProcessArgs()
					{
						LuaImplArgProcesser<(N >= FirstDefArg)>::template Process<N, FirstDefArg, ArgType>(m_instance, m_args, m_defaultArgs);
					}

					template<std::size_t N, typename ArgType1, typename ArgType2, typename... Rest>
					void ProcessArgs()
					{
						ProcessArgs<N, ArgType1>();
						ProcessArgs<N + 1, ArgType2, Rest...>();
					}

					ArgContainer m_args;
					DefArgContainer m_defaultArgs;
					LuaInstance& m_instance;
					T& m_object;
			};
	};

	template<typename T>
	T LuaInstance::Check(int index)
	{
		return LuaImplQueryArg(*this, index, TypeTag<T>());
	}

	template<typename T>
	T LuaInstance::Check(int index, T defValue)
	{
		return LuaImplQueryArg(*this, index, defValue, TypeTag<T>());
	}

	template<typename T>
	int LuaInstance::Push(T arg)
	{
		return LuaImplReplyVal(*this, std::move(arg), TypeTag<T>());
	}

	template<typename R, typename... Args, typename... DefArgs>
	void LuaInstance::PushFunction(R (*func)(Args...), DefArgs... defArgs)
	{
		PushFunction([func, defArgs...](LuaInstance& instance) -> int
		{
			typename LuaImplFunctionProxy<Args...>::template Impl<DefArgs...> handler(instance, defArgs...);
			handler.ProcessArgs();

			return handler.Invoke(func);
		});
	}
}
