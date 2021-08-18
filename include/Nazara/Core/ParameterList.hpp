// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARAMETERLIST_HPP
#define NAZARA_PARAMETERLIST_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <atomic>
#include <string>
#include <unordered_map>

namespace Nz
{
	class NAZARA_CORE_API ParameterList
	{
		public:
			using Destructor = void (*)(void* value);

			ParameterList() = default;
			ParameterList(const ParameterList& list);
			ParameterList(ParameterList&&) = default;
			~ParameterList();

			void Clear();

			inline void ForEach(const std::function<bool(const ParameterList& list, const std::string& name)>& callback);
			inline void ForEach(const std::function<void(const ParameterList& list, const std::string& name)>& callback) const;

			bool GetBooleanParameter(const std::string& name, bool* value) const;
			bool GetColorParameter(const std::string& name, Color* value) const;
			bool GetDoubleParameter(const std::string& name, double* value) const;
			bool GetIntegerParameter(const std::string& name, long long* value) const;
			bool GetParameterType(const std::string& name, ParameterType* type) const;
			bool GetPointerParameter(const std::string& name, void** value) const;
			bool GetStringParameter(const std::string& name, std::string* value) const;
			bool GetUserdataParameter(const std::string& name, void** value) const;

			bool HasParameter(const std::string& name) const;

			void RemoveParameter(const std::string& name);

			void SetParameter(const std::string& name);
			void SetParameter(const std::string& name, const Color& value);
			void SetParameter(const std::string& name, const std::string& value);
			void SetParameter(const std::string& name, const char* value);
			void SetParameter(const std::string& name, bool value);
			void SetParameter(const std::string& name, double value);
			void SetParameter(const std::string& name, long long value);
			void SetParameter(const std::string& name, void* value);
			void SetParameter(const std::string& name, void* value, Destructor destructor);

			std::string ToString() const;

			ParameterList& operator=(const ParameterList& list);
			ParameterList& operator=(ParameterList&&) = default;

		private:
			struct Parameter
			{
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

				ParameterType type;
				union Value
				{
					// We define an empty constructor/destructor, to be able to put classes in the union
					Value() {}
					Value(const Value&) {} // Placeholder
					Value(Value&&) noexcept {} // Placeholder
					~Value() {}

					bool boolVal;
					double doubleVal;
					long long intVal;
					void* ptrVal;
					Color colorVal;
					std::string stringVal;
					UserdataValue* userdataVal;
				};

				Value value;
			};

			Parameter& CreateValue(const std::string& name);
			void DestroyValue(Parameter& parameter);

			using ParameterMap = std::unordered_map<std::string, Parameter>;
			ParameterMap m_parameters;
	};
}

std::ostream& operator<<(std::ostream& out, const Nz::ParameterList& parameterList);

#include <Nazara/Core/ParameterList.inl>

#endif // NAZARA_PARAMETERLIST_HPP
