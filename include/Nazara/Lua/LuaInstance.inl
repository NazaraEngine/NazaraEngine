// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <string>

namespace Nz
{
	// Functions args
	bool LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<bool>)
	{
		return instance.CheckBoolean(index);
	}

	double LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<double>)
	{
		return instance.CheckNumber(index);
	}

	float LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<float>)
	{
		return static_cast<float>(instance.CheckNumber(index));
	}

	template<typename T>
	std::enable_if_t<std::is_enum<T>::value, T> LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<T>)
	{
		return static_cast<T>(LuaImplQueryArg(instance, index, TypeTag<typename std::underlying_type<T>::type>()));
	}

	template<typename T>
	std::enable_if_t<std::is_integral<T>::value && !std::is_unsigned<T>::value, T> LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<T>)
	{
		return static_cast<T>(instance.CheckInteger(index));
	}

	template<typename T>
	std::enable_if_t<std::is_unsigned<T>::value, T> LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<T>)
	{
		return static_cast<T>(LuaImplQueryArg(instance, index, TypeTag<typename std::make_signed<T>::type>()));
	}

	std::string LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<std::string>)
	{
		std::size_t strLength = 0;
		const char* str = instance.CheckString(index, &strLength);

		return std::string(str, strLength);
	}

	String LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<String>)
	{
		std::size_t strLength = 0;
		const char* str = instance.CheckString(index, &strLength);

		return String(str, strLength);
	}

	template<typename T>
	T LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<const T&>)
	{
		return LuaImplQueryArg(instance, index, TypeTag<T>());
	}

	// Function returns
	int LuaImplReplyVal(LuaInstance& instance, bool val, TypeTag<bool>)
	{
		instance.PushBoolean(val);
		return 1;
	}

	int LuaImplReplyVal(LuaInstance& instance, double val, TypeTag<double>)
	{
		instance.PushNumber(val);
		return 1;
	}

	int LuaImplReplyVal(LuaInstance& instance, float val, TypeTag<float>)
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

	int LuaImplReplyVal(LuaInstance& instance, std::string val, TypeTag<std::string>)
	{
		instance.PushString(val.c_str(), val.size());
		return 1;
	}

	int LuaImplReplyVal(LuaInstance& instance, String val, TypeTag<String>)
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

	template<typename... Args>
	class LuaImplFunctionProxy
	{
		public:
			LuaImplFunctionProxy(LuaInstance& instance) :
			m_instance(instance)
			{
			}

			template<unsigned int N>
			void ProcessArgs()
			{
				// No argument to process
			}

			template<unsigned int N, typename ArgType>
			void ProcessArgs()
			{
				std::get<N>(m_args) = std::move(LuaImplQueryArg(m_instance, N+1, TypeTag<ArgType>()));
			}

			template<int N, typename ArgType1, typename ArgType2, typename... Rest>
			void ProcessArgs()
			{
				ProcessArgs<N, ArgType1>();
				ProcessArgs<N+1, ArgType2, Rest...>();
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
			std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...> m_args;
			LuaInstance& m_instance;
	};

	template<typename T, typename... Args>
	class LuaImplMethodProxy
	{
		public:
			LuaImplMethodProxy(LuaInstance& instance, T& object) :
			m_instance(instance),
			m_object(object)
			{
			}

			template<unsigned int N>
			void ProcessArgs()
			{
				// No argument to process
			}

			template<unsigned int N, typename ArgType>
			void ProcessArgs()
			{
				std::get<N>(m_args) = std::move(LuaImplQueryArg(m_instance, N + 1, TypeTag<ArgType>()));
			}

			template<unsigned int N, typename ArgType1, typename ArgType2, typename... Rest>
			void ProcessArgs()
			{
				ProcessArgs<N, ArgType1>();
				ProcessArgs<N + 1, ArgType2, Rest...>();
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
			std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...> m_args;
			LuaInstance& m_instance;
			T& m_object;
	};

	template<typename T>
	T LuaInstance::Check(int index)
	{
		return LuaImplQueryArg(*this, index, TypeTag<T>());
	}

	template<typename T>
	int LuaInstance::Push(T arg)
	{
		return LuaImplReplyVal(*this, std::move(arg), TypeTag<T>());
	}

	template<typename R, typename... Args>
	void LuaInstance::PushFunction(R (*func)(Args...))
	{
		PushFunction([func](LuaInstance& instance) -> int
		{
			LuaImplFunctionProxy<Args...> handler(instance);
			handler.ProcessArgs();

			return handler.Invoke(func);
		});
	}
}
