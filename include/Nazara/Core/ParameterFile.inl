// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Log.hpp>
#include <NazaraUtils/TypeName.hpp>
#include <array>
#include <cassert>
#include <stdexcept>
#include <tuple>
#include <utility>

namespace Nz
{
	inline ParameterFile::ParameterFile(Stream& stream) :
	m_bufferOffset(0),
	m_stream(stream),
	m_currentLine(1)
	{
	}

	template<typename... Args>
	void ParameterFile::Parse(Args&&... args)
	{
		HandleInner<EndOfStream>(std::forward<Args>(args)...);
	}

	template<typename EndToken, typename... Args>
	void ParameterFile::HandleInner(Args&&... args)
	{
		using Types = TypeListTransform<TypeList<Args...>, std::decay>;
		constexpr bool IsArray = TypeListHas<Types, List_t>;

		if constexpr (IsArray)
		{
			ValueHandler handler = GetSingleHandler(std::forward<Args>(args)...);

			for (;;)
			{
				if (std::holds_alternative<EndToken>(PeekToken()))
					break;

				handler(*this);
			}
		}
		else
		{
			FixedVector<KeyValue, sizeof...(Args) / 2> keys;
			BuildKeyValues(&keys, std::forward<Args>(args)...);

			for (;;)
			{
				if (std::holds_alternative<EndToken>(PeekToken()))
					break;

				Identifier nextIdentifier = Read<Identifier>();

				auto it = std::find_if(keys.begin(), keys.end(), [&](const KeyValue& keyValue) { return keyValue.key == nextIdentifier.value; });
				if (it == keys.end())
					throw std::runtime_error(Format("unexpected keyword \"{}\"", nextIdentifier.value));

				const ValueHandler& handler = it->handler;
				handler(*this);
			}
		}
	}

	template<typename T>
	auto ParameterFile::Peek() -> T&
	{
		Token& token = PeekToken();
		if (!std::holds_alternative<T>(token))
			throw std::runtime_error(Format("expected {} on line {}", TypeName<T>(), m_currentLine));

		return std::get<T>(token);
	}

	template<typename T>
	auto ParameterFile::Read() -> T
	{
		Token token = std::move(m_nextToken);
		if (!std::holds_alternative<T>(token))
			throw std::runtime_error(Format("expected {} on line {}", TypeName<T>(), m_currentLine));

		Advance();
		return std::get<T>(token);
	}

	template<typename T>
	T ParameterFile::ReadValue()
	{
		if constexpr (std::is_same_v<T, std::string>)
			return Read<String>().value;
		else if constexpr (std::is_same_v<T, Identifier>)
			return Read<Identifier>();
		else if constexpr (std::is_same_v<T, ParameterFileSection>)
			return ParameterFileSection{ *this };
		else
			static_assert(AlwaysFalse<T>(), "unsupported type");
	}

	template<std::size_t N, typename K, typename... Rest>
	void ParameterFile::BuildKeyValues(FixedVector<KeyValue, N>* keyValues, K&& key, Rest&&... rest)
	{
		if constexpr (ShouldIgnore<K>)
			return BuildKeyValues(keyValues, std::forward<Rest>(rest)...);
		else
		{
			assert(keyValues);
			auto& keyValue = keyValues->emplace_back();
			keyValue.key = BuildBlockKey(std::forward<K>(key));
			keyValue.handler = BuildBlockHandler(keyValues, std::forward<Rest>(rest)...);
		}
	}

	template<typename V, typename... Rest>
	auto ParameterFile::GetSingleHandler(V&& value, Rest&&... rest) -> ValueHandler
	{
		if constexpr (ShouldIgnore<V>)
		{
			static_assert(sizeof...(Rest) > 0, "expected a handler");
			return GetSingleHandler(std::forward<Rest>(rest)...);
		}
		else
		{
			static_assert(sizeof...(Rest) == 0, "expected a single handler");
			return BuildBlockHandler(static_cast<FixedVector<KeyValue, 0>*>(nullptr), std::forward<V>(value));
		}
	}

	template<std::size_t N, typename T, typename... Rest>
	auto ParameterFile::BuildBlockHandler(FixedVector<KeyValue, N>* keyValues, T* value, Rest&&... rest) -> ValueHandler
	{
		ValueHandler valueHandler = [value](ParameterFile& file)
		{
			*value = file.ReadValue<T>();
		};

		if constexpr (sizeof...(Rest) > 0)
			BuildKeyValues(keyValues, std::forward<Rest>(rest)...);

		return valueHandler;
	}

	template<std::size_t N, Functor T, typename... Rest>
	auto ParameterFile::BuildBlockHandler(FixedVector<KeyValue, N>* keyValues, T&& handler, Rest&&... rest) -> ValueHandler
	{
		using FunctionType = typename FunctionTraits<T>::FuncType;
		return BuildBlockHandler(keyValues, FunctionRef<FunctionType>(handler), std::forward<Rest>(rest)...);
	}

	template<std::size_t N, typename O, typename... Args, typename... Rest>
	auto ParameterFile::BuildBlockHandler(FixedVector<KeyValue, N>* keyValues, void(O::* method)(Args...), O* object, Rest&&... rest) -> ValueHandler
	{
		ValueHandler valueHandler = [object, method](ParameterFile& file)
		{
			std::tuple<O*, Args...> args{ object, file.ReadValue<Args>()... };
			std::apply(method, std::move(args));
		};

		if constexpr (sizeof...(Rest) > 0)
			BuildKeyValues(keyValues, std::forward<Rest>(rest)...);

		return valueHandler;
	}

	template<std::size_t N, typename... Args, typename... Rest>
	auto ParameterFile::BuildBlockHandler(FixedVector<KeyValue, N>* keyValues, FunctionRef<void(Args...)> handler, Rest&&... rest) -> ValueHandler
	{
		ValueHandler valueHandler = [handler](ParameterFile& file)
		{
			std::tuple<Args...> args{ file.ReadValue<Args>()... };
			std::apply(handler, std::move(args));
		};

		if constexpr (sizeof...(Rest) > 0)
			BuildKeyValues(keyValues, std::forward<Rest>(rest)...);

		return valueHandler;
	}

	template<typename T>
	std::string_view ParameterFile::BuildBlockKey(T&& key)
	{
		static_assert(std::is_constructible_v<std::string_view, T>, "parameter key must be convertible to a std::string_view");
		return std::string_view(std::forward<T>(key));
	}


	inline ParameterFileSection::ParameterFileSection(ParameterFile& file) :
	m_file(file)
	{
	}

	template<typename ...Args>
	void ParameterFileSection::Block(Args&&... args)
	{
		using Types = TypeListTransform<TypeList<Args...>, std::decay>;

		constexpr bool IsOptionalBlock = TypeListHas<Types, ParameterFile::OptionalBlock_t>;
		if constexpr (IsOptionalBlock)
		{
			if (!std::holds_alternative<ParameterFile::OpenCurlyBracket>(m_file.PeekToken()))
				return;

			m_file.Advance();
		}
		else
		{
			if (!std::holds_alternative<ParameterFile::OpenCurlyBracket>(m_file.Advance()))
				throw std::runtime_error(Format("expected OpenCurlyBracket on line {}", m_file.m_currentLine));
		}

		m_file.HandleInner<ParameterFile::ClosingCurlyBracket>(std::forward<Args>(args)...);

		if (!std::holds_alternative<ParameterFile::ClosingCurlyBracket>(m_file.Advance()))
			throw std::runtime_error(Format("expected ClosingCurlyBracket on line {}", m_file.m_currentLine));
	}
}
