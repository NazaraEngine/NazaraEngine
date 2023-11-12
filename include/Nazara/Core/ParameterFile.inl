// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Log.hpp>
#include <array>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline ParameterFile::ParameterFile(Stream& stream) :
	m_stream(stream)
	{
	}

	template<typename... Args>
	void ParameterFile::Block(Args&&... args)
	{
		using Types = TypeListTransform<TypeListTransform<TypeList<Args...>, std::remove_const>, std::decay>;

		constexpr bool IsOptionalBlock = TypeListHas<Types, OptionalBlock_t>;
		if constexpr (IsOptionalBlock)
		{
			std::string nextKeyword = ReadKeyword(true);
			if (nextKeyword != "{")
				return;
		}

		std::string beginToken = ReadKeyword();
		if (beginToken != "{")
			throw std::runtime_error(Format("expected \"{{\" token, got {}", beginToken));

		HandleInner("}", std::forward<Args>(args)...);

		std::string endToken = ReadKeyword();
		if (endToken != "}")
			throw std::runtime_error(Format("expected \"}}\" token, got {}", endToken));
	}

	template<typename... Args>
	void ParameterFile::Handle(Args&&... args)
	{
		HandleInner({}, std::forward<Args>(args)...);
	}

	template<std::size_t N, typename K, typename V, typename... Rest>
	void ParameterFile::BuildKeyValues(ParameterFile& file, FixedVector<KeyValue, N>& keyValues, K&& key, V&& value, Rest&&... rest)
	{
		if constexpr (ShouldIgnore<K>)
			return BuildKeyValues(file, keyValues, std::forward<V>(value), std::forward<Rest>(rest)...);
		else
		{
			auto& keyValue = keyValues.emplace_back();
			keyValue.key = BuildBlockKey(std::forward<K>(key));
			keyValue.handler = BuildBlockHandler(file, std::forward<V>(value));

			if constexpr (sizeof...(Rest) > 0)
				BuildKeyValues(file, keyValues, std::forward<Rest>(rest)...);
		}
	}

	template<typename V, typename ...Rest>
	auto ParameterFile::GetSingleHandler(ParameterFile& file, V&& value, Rest&&... rest) -> ValueHandler
	{
		if constexpr (ShouldIgnore<V>)
		{
			static_assert(sizeof...(Rest) > 0, "expected a handler");
			return GetSingleHandler(file, std::forward<Rest>(rest)...);
		}
		else
		{
			static_assert(sizeof...(Rest) == 0, "expected a single handler");
			return BuildBlockHandler(file, std::forward<V>(value));
		}
	}

	template<typename T>
	std::string_view ParameterFile::BuildBlockKey(T&& key)
	{
		static_assert(std::is_constructible_v<std::string_view, T>, "parameter key must be convertible to a std::string_view");
		return std::string_view(std::forward<T>(key));
	}

	template<typename... Args>
	auto ParameterFile::BuildBlockHandler(ParameterFile& /*file*/, ValueHandler handler) -> ValueHandler
	{
		return handler;
	}

	template<typename T>
	auto ParameterFile::BuildBlockHandler(ParameterFile& /*file*/, T* value) -> ValueHandler
	{
		return [value](ParameterFile& file)
		{
			*value = ReadValue<T>(file);
		};
	}

	template<typename T>
	auto ParameterFile::BuildBlockHandler(ParameterFile& file, T&& handler, std::enable_if_t<IsFunctor_v<T>>*) -> ValueHandler
	{
		using FunctionType = typename FunctionTraits<T>::FuncType;
		return BuildBlockHandler(file, FunctionRef<FunctionType>(handler));
	}

	template<typename... Args>
	auto ParameterFile::BuildBlockHandler(ParameterFile& /*file*/, FunctionRef<void(Args...)> handler) -> ValueHandler
	{
		return [handler](ParameterFile& file)
		{
			std::tuple<Args...> args{ ReadValue<Args>(file)... };
			std::apply(handler, std::move(args));
		};
	}

	template<typename... Args>
	void ParameterFile::HandleInner(std::string_view listEnd, Args&&... args)
	{
		using Types = TypeListTransform<TypeListTransform<TypeList<Args...>, std::remove_const>, std::decay>;
		constexpr bool IsArray = TypeListHas<Types, Array_t>;

		if constexpr (IsArray)
		{
			ValueHandler handler = GetSingleHandler(*this, std::forward<Args>(args)...);

			for (;;)
			{
				std::string nextKeyword = ReadKeyword(true);
				if (nextKeyword == listEnd)
					break;

				handler(*this);
			}
		}
		else
		{
			FixedVector<KeyValue, sizeof...(Args) / 2> keys;
			BuildKeyValues(*this, keys, std::forward<Args>(args)...);

			for (;;)
			{
				std::string nextKeyword = ReadKeyword(true);
				if (nextKeyword == listEnd)
					break;

				auto it = std::find_if(keys.begin(), keys.end(), [nextKeyword = ReadKeyword()](const KeyValue& keyValue) { return keyValue.key == nextKeyword; });
				if (it == keys.end())
					throw std::runtime_error(Format("unexpected keyword \"{}\"", nextKeyword));

				const ValueHandler& handler = it->handler;
				handler(*this);
			}
		}
	}

	template<typename T>
	T ParameterFile::ReadValue(ParameterFile& file)
	{
		if constexpr (std::is_same_v<T, std::string>)
			return file.ReadString();
		else if constexpr (std::is_same_v<T, Keyword>)
			return Keyword{ file.ReadKeyword() };
		else
			static_assert(AlwaysFalse<T>(), "unsupported type");
	}
}

#include <Nazara/Core/DebugOff.hpp>
