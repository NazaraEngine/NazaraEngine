// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARAMETERLIST_HPP
#define NAZARA_PARAMETERLIST_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/String.hpp>
#include <atomic>
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

			inline void ForEach(const std::function<bool(const ParameterList& list, const String& name)>& callback);
			inline void ForEach(const std::function<void(const ParameterList& list, const String& name)>& callback) const;

			bool GetBooleanParameter(const String& name, bool* value) const;
			bool GetColorParameter(const String& name, Color* value) const;
			bool GetFloatParameter(const String& name, float* value) const;
			bool GetIntegerParameter(const String& name, int* value) const;
			bool GetParameterType(const String& name, ParameterType* type) const;
			bool GetPointerParameter(const String& name, void** value) const;
			bool GetStringParameter(const String& name, String* value) const;
			bool GetUserdataParameter(const String& name, void** value) const;

			bool HasParameter(const String& name) const;

			void RemoveParameter(const String& name);

			void SetParameter(const String& name);
			void SetParameter(const String& name, const Color& value);
			void SetParameter(const String& name, const String& value);
			void SetParameter(const String& name, const char* value);
			void SetParameter(const String& name, bool value);
			void SetParameter(const String& name, float value);
			void SetParameter(const String& name, int value);
			void SetParameter(const String& name, void* value);
			void SetParameter(const String& name, void* value, Destructor destructor);

			String ToString() const;

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
					void* ptr;
				};

				ParameterType type;
				union Value
				{
					// We define an empty constructor/destructor, to be able to put classes in the union
					Value() {}
					Value(const Value&) {} // Placeholder
					~Value() {}

					bool boolVal;
					float floatVal;
					int intVal;
					void* ptrVal;
					Color colorVal;
					String stringVal;
					UserdataValue* userdataVal;
				};

				Value value;
			};

			Parameter& CreateValue(const String& name);
			void DestroyValue(Parameter& parameter);

			using ParameterMap = std::unordered_map<String, Parameter>;
			ParameterMap m_parameters;
	};
}

std::ostream& operator<<(std::ostream& out, const Nz::ParameterList& parameterList);

#include <Nazara/Core/ParameterList.inl>

#endif // NAZARA_PARAMETERLIST_HPP
