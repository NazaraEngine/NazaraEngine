// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <string>

// Functions args
bool NzLuaImplQueryArg(NzLuaInstance& instance, unsigned int index, NzTypeTag<bool>)
{
	return instance.CheckBoolean(index);
}

double NzLuaImplQueryArg(NzLuaInstance& instance, unsigned int index, NzTypeTag<double>)
{
	return instance.CheckNumber(index);
}

float NzLuaImplQueryArg(NzLuaInstance& instance, unsigned int index, NzTypeTag<float>)
{
	return static_cast<float>(instance.CheckNumber(index));
}

int NzLuaImplQueryArg(NzLuaInstance& instance, unsigned int index, NzTypeTag<int>)
{
	return static_cast<int>(instance.CheckInteger(index));
}

std::string NzLuaImplQueryArg(NzLuaInstance& instance, unsigned int index, NzTypeTag<std::string>)
{
	std::size_t strLength = 0;
	const char* str = instance.CheckString(index, &strLength);

	return std::string(str, strLength);
}

NzString NzLuaImplQueryArg(NzLuaInstance& instance, unsigned int index, NzTypeTag<NzString>)
{
	std::size_t strLength = 0;
	const char* str = instance.CheckString(index, &strLength);

	return NzString(str, strLength);
}

template<typename T>
T NzLuaImplQueryArg(NzLuaInstance& instance, unsigned int index, NzTypeTag<const T&>)
{
	return NzLuaImplQueryArg(instance, index, NzTypeTag<T>());
}

template<typename T>
std::enable_if_t<std::is_unsigned<T>::value, T> NzLuaImplQueryArg(NzLuaInstance& instance, unsigned int index, NzTypeTag<T>)
{
	return static_cast<T>(NzLuaImplQueryArg(instance, index, NzTypeTag<typename std::make_signed<T>::type>()));
}

// Function returns
int NzLuaImplReplyVal(NzLuaInstance& instance, bool&& val, NzTypeTag<bool>)
{
	instance.PushBoolean(val);
	return 1;
}

int NzLuaImplReplyVal(NzLuaInstance& instance, double&& val, NzTypeTag<double>)
{
	instance.PushNumber(val);
	return 1;
}

int NzLuaImplReplyVal(NzLuaInstance& instance, float&& val, NzTypeTag<float>)
{
	instance.PushNumber(val);
	return 1;
}

int NzLuaImplReplyVal(NzLuaInstance& instance, int&& val, NzTypeTag<int>)
{
	instance.PushInteger(val);
	return 1;
}

int NzLuaImplReplyVal(NzLuaInstance& instance, std::string&& val, NzTypeTag<std::string>)
{
	instance.PushString(val.c_str(), val.size());
	return 1;
}

int NzLuaImplReplyVal(NzLuaInstance& instance, NzString&& val, NzTypeTag<NzString>)
{
	instance.PushString(std::move(val));
	return 1;
}

template<typename T1, typename T2>
int NzLuaImplReplyVal(NzLuaInstance& instance, std::pair<T1, T2>&& val, NzTypeTag<std::pair<T1, T2>>)
{
	int retVal = 0;

	retVal += NzLuaImplReplyVal(instance, std::move(val.first), NzTypeTag<T1>());
	retVal += NzLuaImplReplyVal(instance, std::move(val.second), NzTypeTag<T2>());

	return retVal;
}

template<typename T>
std::enable_if_t<std::is_unsigned<T>::value, int> NzLuaImplReplyVal(NzLuaInstance& instance, T&& val, NzTypeTag<T>)
{
	using SignedT = typename std::make_signed<T>::type;

	return NzLuaImplReplyVal(instance, val, NzTypeTag<SignedT>());
}

template<typename... Args>
class NzLuaImplFunctionProxy
{
	public:
		NzLuaImplFunctionProxy(NzLuaInstance& instance) :
		m_instance(instance)
		{
		}

		template<unsigned int N, typename ArgType>
		void ProcessArgs()
		{
			std::get<N>(m_args) = std::move(NzLuaImplQueryArg(m_instance, N+1, NzTypeTag<ArgType>()));
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
			NzApply(func, m_args);
			return 0;
		}

		template<typename Ret>
		int Invoke(Ret (*func)(Args...))
		{
			return NzLuaImplReplyVal(m_instance, std::move(NzApply(func, m_args)), NzTypeTag<decltype(NzApply(func, m_args))>());
		}

	private:
		NzLuaInstance& m_instance;
		std::tuple<Args...> m_args;
};

template<typename R, typename... Args>
void NzLuaInstance::PushFunction(R(*func)(Args...))
{
	PushFunction([func](NzLuaInstance& instance) -> int
	{
		NzLuaImplFunctionProxy<Args...> handler(instance);
		handler.ProcessArgs();

		return handler.Invoke(func);
	});
}
