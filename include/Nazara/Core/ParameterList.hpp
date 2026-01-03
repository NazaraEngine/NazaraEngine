// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_PARAMETERLIST_HPP
#define NAZARA_CORE_PARAMETERLIST_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <NazaraUtils/Result.hpp>
#include <NazaraUtils/StringHash.hpp>
#include <atomic>
#include <string>
#include <unordered_map>
#include <variant>

namespace Nz
{
	class NAZARA_CORE_API ParameterList
	{
		public:
			using Destructor = void (*)(void* value);
			enum class Error;

			ParameterList() = default;
			ParameterList(const ParameterList& list);
			ParameterList(ParameterList&&) noexcept = default;
			~ParameterList();

			void Clear();

			inline void ForEach(const std::function<bool(const ParameterList& list, const std::string& name)>& callback);
			inline void ForEach(const std::function<void(const ParameterList& list, const std::string& name)>& callback) const;

			Result<bool, Error> GetBooleanParameter(std::string_view name, bool strict = true) const;
			Result<Color, Error> GetColorParameter(std::string_view name, bool strict = true) const;
			Result<double, Error> GetDoubleParameter(std::string_view name, bool strict = true) const;
			Result<long long, Error> GetIntegerParameter(std::string_view name, bool strict = true) const;
			Result<void*, Error> GetPointerParameter(std::string_view name, bool strict = true) const;
			Result<std::string, Error> GetStringParameter(std::string_view name, bool strict = true) const;
			Result<std::string_view, Error> GetStringViewParameter(std::string_view name, bool strict = true) const;
			Result<void*, Error> GetUserdataParameter(std::string_view name, bool strict = true) const;

			bool HasParameter(std::string_view name) const;

			void RemoveParameter(std::string_view name);

			void SetParameter(std::string name);
			void SetParameter(std::string name, const Color& value);
			void SetParameter(std::string name, std::string value);
			void SetParameter(std::string name, const char* value);
			void SetParameter(std::string name, bool value);
			void SetParameter(std::string name, double value);
			void SetParameter(std::string name, long long value);
			void SetParameter(std::string name, void* value);
			void SetParameter(std::string name, void* value, Destructor destructor);

			std::string ToString() const;

			ParameterList& operator=(const ParameterList& list);
			ParameterList& operator=(ParameterList&& list) noexcept = default;

			enum class Error
			{
				ConversionFailed,
				MissingValue,
				WouldRequireConversion,
				WrongType
			};

		private:
			struct UserdataValue
			{
				UserdataValue(Destructor func, void* ud) :
				counter(1),
				destructor(func),
				ptr(ud)
				{
				}

				std::atomic_uint counter;
				Destructor destructor;
				MovablePtr<void> ptr;
			};

			template<typename T>
			struct Primitive
			{
				T value;
			};

			using Parameter = std::variant<std::monostate, Primitive<bool>, Primitive<double>, Primitive<long long>, Primitive<void*>, Color, std::string, UserdataValue*>;

			Parameter& CreateValue(std::string&& name);
			void DestroyValue(Parameter& parameter);

			using ParameterMap = std::unordered_map<std::string, Parameter, StringHash<>, std::equal_to<>>;
			ParameterMap m_parameters;
	};

	NAZARA_CORE_API std::ostream& operator<<(std::ostream& out, const ParameterList& parameterList);
}

#include <Nazara/Core/ParameterList.inl>

#endif // NAZARA_CORE_PARAMETERLIST_HPP
