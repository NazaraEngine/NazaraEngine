// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARAMETERLIST_HPP
#define NAZARA_PARAMETERLIST_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <atomic>
#include <unordered_map>

class NAZARA_API NzParameterList
{
	public:
		using Destructor = void (*)(void* value);

		NzParameterList() = default;
		NzParameterList(const NzParameterList& list);
		NzParameterList(NzParameterList&& list);
		virtual ~NzParameterList();

		void Clear();

		bool GetBooleanParameter(const NzString& name, bool* value) const;
		bool GetFloatParameter(const NzString& name, float* value) const;
		bool GetIntegerParameter(const NzString& name, int* value) const;
		bool GetParameterType(const NzString& name, nzParameterType* type) const;
		bool GetPointerParameter(const NzString& name, void** value) const;
		bool GetStringParameter(const NzString& name, NzString* value) const;
		bool GetUserdataParameter(const NzString& name, void** value) const;

		bool HasParameter(const NzString& name) const;

		void RemoveParameter(const NzString& name);

		void SetParameter(const NzString& name);
		void SetParameter(const NzString& name, const NzString& value);
		void SetParameter(const NzString& name, const char* value);
		void SetParameter(const NzString& name, void* value);
		void SetParameter(const NzString& name, void* value, Destructor destructor);
		void SetParameter(const NzString& name, bool value);
		void SetParameter(const NzString& name, float value);
		void SetParameter(const NzString& name, int value);

		NzParameterList& operator=(const NzParameterList& list);
		NzParameterList& operator=(NzParameterList&& list);

	private:
		struct Parameter
		{
			struct UserdataValue
			{
				UserdataValue(Destructor Destructor, void* value) :
				counter(1),
				destructor(Destructor),
				ptr(value)
				{
				}

				std::atomic_uint counter;
				Destructor destructor;
				void* ptr;
			};

			nzParameterType type;
			union Value
			{
				// On définit un constructeur/destructeur vide, permettant de mettre des classes dans l'union
				Value() {}
				Value(const Value&) {} // Placeholder
				~Value() {}

				bool boolVal;
				float floatVal;
				int intVal;
				void* ptrVal;
				NzString stringVal;
				UserdataValue* userdataVal;
			};

			Value value;
		};

		using ParameterMap = std::unordered_map<NzString, Parameter>;

		void DestroyValue(Parameter& parameter);

		ParameterMap m_parameters;
};

#endif // NAZARA_PARAMETERLIST_HPP
