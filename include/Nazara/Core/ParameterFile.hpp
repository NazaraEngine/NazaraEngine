// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_PARAMETERFILE_HPP
#define NAZARA_CORE_PARAMETERFILE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <NazaraUtils/FunctionTraits.hpp>
#include <NazaraUtils/TypeList.hpp>
#include <functional>
#include <string>
#include <type_traits>

namespace Nz
{
	class Stream;

	class NAZARA_CORE_API ParameterFile
	{
		public:
			inline ParameterFile(Stream& stream);
			ParameterFile(const ParameterFile&) = delete;
			ParameterFile(ParameterFile&&) = delete;
			~ParameterFile() = default;

			template<typename... Args> void Block(Args&&... args);
			template<typename... Args> void Handle(Args&&... args);

			ParameterFile& operator=(const ParameterFile&) = delete;
			ParameterFile& operator=(ParameterFile&&) = delete;

			struct Keyword
			{
				std::string str;
			};

			struct Array_t {};
			struct OptionalBlock_t {};

			static constexpr Array_t Array{};
			static constexpr OptionalBlock_t OptionalBlock{};

		private:
			using ValueHandler = std::function<void(ParameterFile& file)>;

			struct KeyValue
			{
				std::string_view key;
				ValueHandler handler;
			};

			template<typename T> static std::string_view BuildBlockKey(T&& key);
			template<typename... Args> static ValueHandler BuildBlockHandler(ParameterFile& file, ValueHandler handler);
			template<typename T> static ValueHandler BuildBlockHandler(ParameterFile& file, T* value);
			template<typename T> static ValueHandler BuildBlockHandler(ParameterFile& file, T&& handler, std::enable_if_t<IsFunctor_v<T>>* = nullptr);
			template<typename... Args> static ValueHandler BuildBlockHandler(ParameterFile& file, FunctionRef<void(Args...)> handler);

			template<std::size_t N, typename K, typename V, typename... Rest> static void BuildKeyValues(ParameterFile& file, FixedVector<KeyValue, N>& keyValues, K&& key, V&& value, Rest&&... rest);
			template<typename V, typename... Rest> static ValueHandler GetSingleHandler(ParameterFile& file, V&& value, Rest&&... rest);

			template<typename... Args> void HandleInner(std::string_view listEnd, Args&&... args);

			template<typename T> static T ReadValue(ParameterFile& file);

			template<typename T> static constexpr bool ShouldIgnoreImpl = std::is_same_v<T, Array_t> || std::is_same_v<T, OptionalBlock_t>;
			template<typename T> static constexpr bool ShouldIgnore = ShouldIgnoreImpl<std::decay_t<std::remove_const_t<T>>>;

			bool EnsureLine(bool peek = false);
			std::string ReadKeyword(bool peek = false);
			std::string ReadString();

			std::string m_currentLine;
			Stream& m_stream;
	};
}

#include <Nazara/Core/ParameterFile.inl>

#endif // NAZARA_CORE_PARAMETERFILE_HPP
