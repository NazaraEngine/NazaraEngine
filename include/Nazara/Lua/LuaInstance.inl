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

	int LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<int>)
	{
		return static_cast<int>(instance.CheckInteger(index));
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

	template<typename T>
	std::enable_if_t<std::is_unsigned<T>::value, T> LuaImplQueryArg(LuaInstance& instance, unsigned int index, TypeTag<T>)
	{
		return static_cast<T>(LuaImplQueryArg(instance, index, TypeTag<typename std::make_signed<T>::type>()));
	}

	// Function returns
	int LuaImplReplyVal(LuaInstance& instance, bool&& val, TypeTag<bool>)
	{
		instance.PushBoolean(val);
		return 1;
	}

	int LuaImplReplyVal(LuaInstance& instance, double&& val, TypeTag<double>)
	{
		instance.PushNumber(val);
		return 1;
	}

	int LuaImplReplyVal(LuaInstance& instance, float&& val, TypeTag<float>)
	{
		instance.PushNumber(val);
		return 1;
	}

	int LuaImplReplyVal(LuaInstance& instance, int&& val, TypeTag<int>)
	{
		instance.PushInteger(val);
		return 1;
	}

	int LuaImplReplyVal(LuaInstance& instance, std::string&& val, TypeTag<std::string>)
	{
		instance.PushString(val.c_str(), val.size());
		return 1;
	}

	int LuaImplReplyVal(LuaInstance& instance, String&& val, TypeTag<String>)
	{
		instance.PushString(std::move(val));
		return 1;
	}

	template<typename T1, typename T2>
	int LuaImplReplyVal(LuaInstance& instance, std::pair<T1, T2>&& val, TypeTag<std::pair<T1, T2>>)
	{
		int retVal = 0;

		retVal += LuaImplReplyVal(instance, std::move(val.first), TypeTag<T1>());
		retVal += LuaImplReplyVal(instance, std::move(val.second), TypeTag<T2>());

		return retVal;
	}

	template<typename T>
	std::enable_if_t<std::is_unsigned<T>::value, int> LuaImplReplyVal(LuaInstance& instance, T&& val, TypeTag<T>)
	{
		using SignedT = typename std::make_signed<T>::type;

		return LuaImplReplyVal(instance, val, TypeTag<SignedT>());
	}

	template<typename... Args>
	class LuaImplFunctionProxy
	{
		public:
			LuaImplFunctionProxy(LuaInstance& instance) :
			m_instance(instance)
			{
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
			LuaInstance& m_instance;
			std::tuple<Args...> m_args;
	};

	template<typename R, typename... Args>
	void LuaInstance::PushFunction(R(*func)(Args...))
	{
		PushFunction([func](LuaInstance& instance) -> int
		{
			LuaImplFunctionProxy<Args...> handler(instance);
			handler.ProcessArgs();

			return handler.Invoke(func);
		});
	}
}
