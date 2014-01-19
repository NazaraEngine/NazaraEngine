// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

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

bool NzParameterList::GetBooleanParameter(const NzString& name, bool* succeeded) const
{
	auto it = m_parameters.find(name);
	if (it == m_parameters.end())
	{
		if (succeeded)
			*succeeded = false;

		NazaraError("Parameter \"" + name + "\" is not present");
		return false;
	}

	switch (it->second.type)
	{
		case nzParameterType_Boolean:
			if (succeeded)
				*succeeded = true;

			return it->second.value.boolVal;

		case nzParameterType_Integer:
			if (succeeded)
				*succeeded = true;

			return (it->second.value.intVal != 0);

		case nzParameterType_String:
		{
			bool value;
			if (it->second.value.stringVal.ToBool(&value, NzString::CaseInsensitive))
			{
				if (succeeded)
					*succeeded = true;

				return value;
			}

			break;
		}

		case nzParameterType_Float:
		case nzParameterType_None:
		case nzParameterType_Pointer:
		case nzParameterType_Userdata:
			break;
	}

	if (succeeded)
		*succeeded = false;

	NazaraError("Parameter value is not representable as a boolean");
	return false;
}

float NzParameterList::GetFloatParameter(const NzString& name, bool* succeeded) const
{
	auto it = m_parameters.find(name);
	if (it == m_parameters.end())
	{
		if (succeeded)
			*succeeded = false;

		NazaraError("Parameter \"" + name + "\" is not present");
		return std::numeric_limits<float>::quiet_NaN();
	}

	switch (it->second.type)
	{
		case nzParameterType_Float:
			if (succeeded)
				*succeeded = true;

			return it->second.value.floatVal;

		case nzParameterType_Integer:
			if (succeeded)
				*succeeded = true;

			return it->second.value.intVal;

		case nzParameterType_String:
		{
			double value;
			if (it->second.value.stringVal.ToDouble(&value))
			{
				if (succeeded)
					*succeeded = true;

				return value;
			}

			break;
		}

		case nzParameterType_Boolean:
		case nzParameterType_None:
		case nzParameterType_Pointer:
		case nzParameterType_Userdata:
			break;
	}

	if (succeeded)
		*succeeded = false;

	NazaraError("Parameter value is not representable as a float");
	return std::numeric_limits<float>::quiet_NaN();
}

int NzParameterList::GetIntegerParameter(const NzString& name, bool* succeeded) const
{
	auto it = m_parameters.find(name);
	if (it == m_parameters.end())
	{
		if (succeeded)
			*succeeded = false;

		NazaraError("Parameter \"" + name + "\" is not present");
		return 0;
	}

	switch (it->second.type)
	{
		case nzParameterType_Boolean:
			return (it->second.value.boolVal) ? 1 : 0;

		case nzParameterType_Float:
			if (succeeded)
				*succeeded = true;

			return it->second.value.floatVal;

		case nzParameterType_Integer:
			if (succeeded)
				*succeeded = true;

			return it->second.value.intVal;

		case nzParameterType_String:
		{
			long long value;
			if (it->second.value.stringVal.ToInteger(&value))
			{
				if (value <= std::numeric_limits<int>::max() && value >= std::numeric_limits<int>::min())
				{
					if (succeeded)
						*succeeded = true;

					return value;
				}
			}

			break;
		}

		case nzParameterType_None:
		case nzParameterType_Pointer:
		case nzParameterType_Userdata:
			break;
	}

	if (succeeded)
		*succeeded = false;

	NazaraError("Parameter value is not representable as a integer");
	return 0;
}

nzParameterType NzParameterList::GetParameterType(const NzString& name, bool* existing) const
{
	auto it = m_parameters.find(name);
	if (it == m_parameters.end())
	{
		if (existing)
			*existing = false;

		return nzParameterType_None;
	}

	if (existing)
		*existing = true;

	return it->second.type;
}

void* NzParameterList::GetPointerParameter(const NzString& name, bool* succeeded) const
{
	auto it = m_parameters.find(name);
	if (it == m_parameters.end())
	{
		if (succeeded)
			*succeeded = false;

		NazaraError("Parameter \"" + name + "\" is not present");
		return nullptr;
	}

	switch (it->second.type)
	{
		case nzParameterType_Pointer:
			if (succeeded)
				*succeeded = true;

			return it->second.value.ptrVal;
		case nzParameterType_Userdata:
			if (succeeded)
				*succeeded = true;

			return it->second.value.userdataVal->ptr;

		case nzParameterType_Boolean:
		case nzParameterType_Float:
		case nzParameterType_Integer:
		case nzParameterType_None:
		case nzParameterType_String:
			break;
	}

	if (succeeded)
		*succeeded = false;

	NazaraError("Parameter value is not a pointer");
	return nullptr;
}

NzString NzParameterList::GetStringParameter(const NzString& name, bool* succeeded) const
{
	auto it = m_parameters.find(name);
	if (it == m_parameters.end())
	{
		if (succeeded)
			*succeeded = false;

		NazaraError("Parameter \"" + name + "\" is not present");
		return NzString();
	}

	switch (it->second.type)
	{
		case nzParameterType_Boolean:
			if (succeeded)
				*succeeded = true;

			return NzString::Boolean(it->second.value.boolVal);

		case nzParameterType_Float:
			if (succeeded)
				*succeeded = true;

			return NzString::Number(it->second.value.floatVal);

		case nzParameterType_Integer:
			if (succeeded)
				*succeeded = true;

			return NzString::Number(it->second.value.intVal);

		case nzParameterType_String:
			if (succeeded)
				*succeeded = true;

			return it->second.value.stringVal;

		case nzParameterType_Pointer:
			if (succeeded)
				*succeeded = true;

			return NzString::Pointer(it->second.value.ptrVal);

		case nzParameterType_Userdata:
			if (succeeded)
				*succeeded = true;

			return NzString::Pointer(it->second.value.userdataVal->ptr);

		case nzParameterType_None:
			if (succeeded)
				*succeeded = true;

			return NzString();
	}

	if (succeeded)
		*succeeded = false;

	NazaraInternalError("Parameter value is not valid");
	return NzString();
}

void* NzParameterList::GetUserdataParameter(const NzString& name, bool* succeeded) const
{
	auto it = m_parameters.find(name);
	if (it == m_parameters.end())
	{
		if (succeeded)
			*succeeded = false;

		NazaraError("Parameter \"" + name + "\" is not present");
		return nullptr;
	}

	switch (it->second.type)
	{
		case nzParameterType_Userdata:
			if (succeeded)
				*succeeded = true;

			return it->second.value.userdataVal->ptr;

		case nzParameterType_Boolean:
		case nzParameterType_Float:
		case nzParameterType_Integer:
		case nzParameterType_None:
		case nzParameterType_Pointer:
		case nzParameterType_String:
			break;
	}

	if (succeeded)
		*succeeded = false;

	NazaraError("Parameter value is not an userdata");
	return nullptr;
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
