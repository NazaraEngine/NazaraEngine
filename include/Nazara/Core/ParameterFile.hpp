// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_PARAMETERFILE_HPP
#define NAZARA_CORE_PARAMETERFILE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <NazaraUtils/FunctionTraits.hpp>
#include <NazaraUtils/TypeList.hpp>
#include <functional>
#include <string>
#include <type_traits>
#include <variant>

namespace Nz
{
	class ParameterFileSection;
	class Stream;

	class NAZARA_CORE_API ParameterFile
	{
		friend ParameterFileSection;

		public:
			inline ParameterFile(Stream& stream);
			ParameterFile(const ParameterFile&) = delete;
			ParameterFile(ParameterFile&&) = delete;
			~ParameterFile() = default;

			template<typename... Args> void Parse(Args&&... args);

			ParameterFile& operator=(const ParameterFile&) = delete;
			ParameterFile& operator=(ParameterFile&&) = delete;

			struct Identifier
			{
				std::string value;
			};

			struct List_t {};
			struct OptionalBlock_t {};

			static constexpr List_t List{};
			static constexpr OptionalBlock_t OptionalBlock{};

		private:
			using ValueHandler = std::function<void(ParameterFile& file)>;

			struct KeyValue
			{
				std::string_view key;
				ValueHandler handler;
			};

			struct ClosingCurlyBracket {};
			struct EndOfStream {};
			struct OpenCurlyBracket {};

			struct String
			{
				std::string value; //< has to be a string because of text parsing
			};

			using Token = std::variant<std::monostate, ClosingCurlyBracket, EndOfStream, Identifier, OpenCurlyBracket, String>;

			Token Advance();
			void ConsumeChar(std::size_t count = 1);
			template<typename EndToken, typename... Args> void HandleInner(Args&&... args);
			Token& PeekToken();
			char PeekCharacter(std::size_t advance = 1);
			template<typename T> T& Peek();
			template<typename T> T Read();
			template<typename T> T ReadValue();

			// Handlers
			template<std::size_t N, typename T, typename... Rest> ValueHandler BuildBlockHandler(FixedVector<KeyValue, N>* keyValues, T* value, Rest&&... rest);
			template<std::size_t N, Functor T, typename... Rest> ValueHandler BuildBlockHandler(FixedVector<KeyValue, N>* keyValues, T&& handler, Rest&&... rest);
			template<std::size_t N, typename O, typename... Args, typename... Rest> ValueHandler BuildBlockHandler(FixedVector<KeyValue, N>* keyValues, void(O::* method)(Args...), O* object, Rest&&... rest);
			template<std::size_t N, typename... Args, typename... Rest> ValueHandler BuildBlockHandler(FixedVector<KeyValue, N>* keyValues, FunctionRef<void(Args...)> handler, Rest&&... rest);
			template<std::size_t N, typename K, typename... Rest> void BuildKeyValues(FixedVector<KeyValue, N>* keyValues, K&& key, Rest&&... rest);
			template<typename V, typename... Rest> ValueHandler GetSingleHandler(V&& value, Rest&&... rest);

			template<typename T> static std::string_view BuildBlockKey(T&& key);

			template<typename T> static constexpr bool ShouldIgnoreImpl = std::is_same_v<T, List_t> || std::is_same_v<T, OptionalBlock_t>;
			template<typename T> static constexpr bool ShouldIgnore = ShouldIgnoreImpl<std::decay_t<T>>;

			std::size_t m_bufferOffset;
			std::string m_buffer;
			Stream& m_stream;
			Token m_nextToken;
			unsigned int m_currentLine;
	};

	class ParameterFileSection
	{
		friend ParameterFile;

		public:
			ParameterFileSection(const ParameterFileSection&) = default;

			template<typename... Args> void Block(Args&&... args);

		private:
			inline ParameterFileSection(ParameterFile& file);

			ParameterFile& m_file;
	};
}

#include <Nazara/Core/ParameterFile.inl>

#endif // NAZARA_CORE_PARAMETERFILE_HPP
