// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Notre utilisation du placement new n'est pas (encore ?) compatible avec les définitions du MLT
#define NAZARA_DEBUG_MEMORYLEAKTRACKER_DISABLE_REDEFINITION

#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Core/Error.hpp>
#include <cstring>
#include <limits>
#include <new>
#include <Nazara/Core/Debug.hpp>

NzParameterList::NzParameterList(const NzParameterList& list)
{
	operator=(list);
}

NzParameterList::NzParameterList(NzParameterList&& list) :
m_parameters(std::move(list.m_parameters))
{
}

NzParameterList::~NzParameterList()
{
	Clear();
}

void NzParameterList::Clear()
{
	for (auto it = m_parameters.begin(); it != m_parameters.end(); ++it)
		DestroyValue(it->second);

	m_parameters.clear();
}

bool NzParameterList::GetBooleanParameter(const NzString& name, bool* value) const
{
	auto it = m_parameters.find(name);
	if (it == m_parameters.end())
	{
		NazaraError("Parameter \"" + name + "\" is not present");
		return false;
	}

	switch (it->second.type)
	{
		case nzParameterType_Boolean:
			*value = it->second.value.boolVal;
			return true;

		case nzParameterType_Integer:
			*value = (it->second.value.intVal != 0);
			return true;

		case nzParameterType_String:
		{
			bool converted;
			if (it->second.value.stringVal.ToBool(&converted, NzString::CaseInsensitive))
			{
				*value = converted;
				return true;
			}

			break;
		}

		case nzParameterType_Float:
		case nzParameterType_None:
		case nzParameterType_Pointer:
		case nzParameterType_Userdata:
			break;
	}

	NazaraError("Parameter value is not representable as a boolean");
	return false;
}

bool NzParameterList::GetFloatParameter(const NzString& name, float* value) const
{
	auto it = m_parameters.find(name);
	if (it == m_parameters.end())
	{
		NazaraError("Parameter \"" + name + "\" is not present");
		return false;
	}

	switch (it->second.type)
	{
		case nzParameterType_Float:
			*value = it->second.value.floatVal;
			return true;

		case nzParameterType_Integer:
			*value = it->second.value.intVal;
			return true;

		case nzParameterType_String:
		{
			double converted;
			if (it->second.value.stringVal.ToDouble(&converted))
			{
				*value = converted;
				return true;
			}

			break;
		}

		case nzParameterType_Boolean:
		case nzParameterType_None:
		case nzParameterType_Pointer:
		case nzParameterType_Userdata:
			break;
	}

	NazaraError("Parameter value is not representable as a float");
	return false;
}

bool NzParameterList::GetIntegerParameter(const NzString& name, int* value) const
{
	auto it = m_parameters.find(name);
	if (it == m_parameters.end())
	{
		NazaraError("Parameter \"" + name + "\" is not present");
		return false;
	}

	switch (it->second.type)
	{
		case nzParameterType_Boolean:
			*value = (it->second.value.boolVal) ? 1 : 0;
			return true;

		case nzParameterType_Float:
			*value = it->second.value.floatVal;
			return true;

		case nzParameterType_Integer:
			*value = it->second.value.intVal;
			return false;

		case nzParameterType_String:
		{
			long long converted;
			if (it->second.value.stringVal.ToInteger(&converted))
			{
				if (converted <= std::numeric_limits<int>::max() && converted >= std::numeric_limits<int>::min())
				{
					*value = converted;
					return true;
				}
			}
			break;
		}

		case nzParameterType_None:
		case nzParameterType_Pointer:
		case nzParameterType_Userdata:
			break;
	}

	NazaraError("Parameter value is not representable as a integer");
	return false;
}

bool NzParameterList::GetParameterType(const NzString& name, nzParameterType* type) const
{
	auto it = m_parameters.find(name);
	if (it == m_parameters.end())
		return false;

	*type = it->second.type;

	return true;
}

bool NzParameterList::GetPointerParameter(const NzString& name, void** value) const
{
	auto it = m_parameters.find(name);
	if (it == m_parameters.end())
	{
		NazaraError("Parameter \"" + name + "\" is not present");
		return false;
	}

	switch (it->second.type)
	{
		case nzParameterType_Pointer:
			*value = it->second.value.ptrVal;
			return true;

		case nzParameterType_Userdata:
			*value = it->second.value.userdataVal->ptr;
			return true;

		case nzParameterType_Boolean:
		case nzParameterType_Float:
		case nzParameterType_Integer:
		case nzParameterType_None:
		case nzParameterType_String:
			break;
	}

	NazaraError("Parameter value is not a pointer");
	return false;
}

bool NzParameterList::GetStringParameter(const NzString& name, NzString* value) const
{
	auto it = m_parameters.find(name);
	if (it == m_parameters.end())
	{
		NazaraError("Parameter \"" + name + "\" is not present");
		return false;
	}

	switch (it->second.type)
	{
		case nzParameterType_Boolean:
			*value = NzString::Boolean(it->second.value.boolVal);
			return true;

		case nzParameterType_Float:
			*value = NzString::Number(it->second.value.floatVal);
			return true;

		case nzParameterType_Integer:
			*value = NzString::Number(it->second.value.intVal);
			return true;

		case nzParameterType_String:
			*value = it->second.value.stringVal;
			return true;

		case nzParameterType_Pointer:
			*value = NzString::Pointer(it->second.value.ptrVal);
			return true;

		case nzParameterType_Userdata:
			*value = NzString::Pointer(it->second.value.userdataVal->ptr);
			return true;

		case nzParameterType_None:
			*value = NzString();
			return true;
	}

	NazaraInternalError("Parameter value is not valid");
	return false;
}

bool NzParameterList::GetUserdataParameter(const NzString& name, void** value) const
{
	auto it = m_parameters.find(name);
	if (it == m_parameters.end())
	{
		NazaraError("Parameter \"" + name + "\" is not present");
		return false;
	}

	if (it->second.type == nzParameterType_Userdata)
	{
		*value = it->second.value.userdataVal->ptr;
		return true;
	}
	else
	{
		NazaraError("Parameter value is not an userdata");
		return nullptr;
	}
}

bool NzParameterList::HasParameter(const NzString& name) const
{
	return m_parameters.find(name) != m_parameters.end();
}

void NzParameterList::RemoveParameter(const NzString& name)
{
	auto it = m_parameters.find(name);
	if (it != m_parameters.end())
	{
		DestroyValue(it->second);
		m_parameters.erase(it);
	}
}

void NzParameterList::SetParameter(const NzString& name)
{
	std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
	Parameter& parameter = pair.first->second;

	if (!pair.second)
		DestroyValue(parameter);

	parameter.type = nzParameterType_None;
}

void NzParameterList::SetParameter(const NzString& name, const NzString& value)
{
	std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
	Parameter& parameter = pair.first->second;

	if (!pair.second)
		DestroyValue(parameter);

	parameter.type = nzParameterType_String;

	new (&parameter.value.stringVal) NzString(value);
}

void NzParameterList::SetParameter(const NzString& name, const char* value)
{
	std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
	Parameter& parameter = pair.first->second;

	if (!pair.second)
		DestroyValue(parameter);

	parameter.type = nzParameterType_String;

	new (&parameter.value.stringVal) NzString(value);
}

void NzParameterList::SetParameter(const NzString& name, void* value)
{
	std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
	Parameter& parameter = pair.first->second;

	if (!pair.second)
		DestroyValue(parameter);

	parameter.type = nzParameterType_Pointer;
	parameter.value.ptrVal = value;
}

void NzParameterList::SetParameter(const NzString& name, void* value, Destructor destructor)
{
	std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
	Parameter& parameter = pair.first->second;

	if (!pair.second)
		DestroyValue(parameter);

	parameter.type = nzParameterType_Userdata;
	parameter.value.userdataVal = new Parameter::UserdataValue(destructor, value);
}

void NzParameterList::SetParameter(const NzString& name, bool value)
{
	std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
	Parameter& parameter = pair.first->second;

	if (!pair.second)
		DestroyValue(parameter);

	parameter.type = nzParameterType_Boolean;
	parameter.value.boolVal = value;
}

void NzParameterList::SetParameter(const NzString& name, float value)
{
	std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
	Parameter& parameter = pair.first->second;

	if (!pair.second)
		DestroyValue(parameter);

	parameter.type = nzParameterType_Float;
	parameter.value.floatVal = value;
}

void NzParameterList::SetParameter(const NzString& name, int value)
{
	std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
	Parameter& parameter = pair.first->second;

	if (!pair.second)
		DestroyValue(parameter);

	parameter.type = nzParameterType_Integer;
	parameter.value.intVal = value;
}

NzParameterList& NzParameterList::operator=(const NzParameterList& list)
{
	Clear();

	for (auto it = list.m_parameters.begin(); it != list.m_parameters.end(); ++it)
	{
		Parameter& parameter = m_parameters[it->first];

		switch (it->second.type)
		{
			case nzParameterType_Boolean:
			case nzParameterType_Float:
			case nzParameterType_Integer:
			case nzParameterType_Pointer:
				std::memcpy(&parameter, &it->second, sizeof(Parameter));
				break;

			case nzParameterType_String:
				parameter.type = nzParameterType_String;

				new (&parameter.value.stringVal) NzString(it->second.value.stringVal);
				break;

			case nzParameterType_Userdata:
				parameter.type = nzParameterType_Userdata;
				parameter.value.userdataVal = it->second.value.userdataVal;
				++(parameter.value.userdataVal->counter);
				break;

			case nzParameterType_None:
				parameter.type = nzParameterType_None;
				break;
		}
	}

	return *this;
}

NzParameterList& NzParameterList::operator=(NzParameterList&& list)
{
	m_parameters = std::move(list.m_parameters);
	return *this;
}

void NzParameterList::DestroyValue(Parameter& parameter)
{
	switch (parameter.type)
	{
		case nzParameterType_String:
			parameter.value.stringVal.~NzString();
			break;

		case nzParameterType_Userdata:
		{
			Parameter::UserdataValue* userdata = parameter.value.userdataVal;
			if (--userdata->counter == 0)
			{
				userdata->destructor(userdata->ptr);
				delete userdata;
			}
			break;
		}

		case nzParameterType_Boolean:
		case nzParameterType_Float:
		case nzParameterType_Integer:
		case nzParameterType_None:
		case nzParameterType_Pointer:
			break;
	}
}
