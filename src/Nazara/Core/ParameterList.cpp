// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <cstring>
#include <limits>
#include <new>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::ParameterList
	* \brief Core class that represents a list of parameters
	*/

	/*!
	* \brief Constructs a ParameterList object by copy
	*/

	ParameterList::ParameterList(const ParameterList& list)
	{
		operator=(list);
	}

	/*!
	* \brief Destructs the object and clears
	*/

	ParameterList::~ParameterList()
	{
		Clear();
	}

	/*!
	* \brief Clears the list of parameters
	*/

	void ParameterList::Clear()
	{
		for (auto it = m_parameters.begin(); it != m_parameters.end(); ++it)
			DestroyValue(it->second);

		m_parameters.clear();
	}

	/*!
	* \brief Gets the boolean parameter by name
	* \return true if success
	*
	* \param name Name of the variable
	* \param value Value to set
	*
	* \remark Produces a NazaraAssert if pointer is invalid
	* \remark Produces a NazaraError if name is not a variable
	* \remark Produces a NazaraError if value could not be convertible
	*/

	bool ParameterList::GetBooleanParameter(const String& name, bool* value) const
	{
		NazaraAssert(value, "Invalid pointer");

		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
		{
			NazaraError("Parameter \"" + name + "\" is not present");
			return false;
		}

		switch (it->second.type)
		{
			case ParameterType_Boolean:
				*value = it->second.value.boolVal;
				return true;

			case ParameterType_Integer:
				*value = (it->second.value.intVal != 0);
				return true;

			case ParameterType_String:
				{
					bool converted;
					if (it->second.value.stringVal.ToBool(&converted, String::CaseInsensitive))
					{
						*value = converted;
						return true;
					}

					break;
				}

			case ParameterType_Float:
			case ParameterType_None:
			case ParameterType_Pointer:
			case ParameterType_Userdata:
				break;
		}

		NazaraError("Parameter value is not representable as a boolean");
		return false;
	}

	/*!
	* \brief Gets the float parameter by name
	* \return true if success
	*
	* \param name Name of the variable
	* \param value Value to set
	*
	* \remark Produces a NazaraAssert if pointer is invalid
	* \remark Produces a NazaraError if name is not a variable
	* \remark Produces a NazaraError if value could not be convertible
	*/

	bool ParameterList::GetFloatParameter(const String& name, float* value) const
	{
		NazaraAssert(value, "Invalid pointer");

		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
		{
			NazaraError("Parameter \"" + name + "\" is not present");
			return false;
		}

		switch (it->second.type)
		{
			case ParameterType_Float:
				*value = it->second.value.floatVal;
				return true;

			case ParameterType_Integer:
				*value = static_cast<float>(it->second.value.intVal);
				return true;

			case ParameterType_String:
				{
					double converted;
					if (it->second.value.stringVal.ToDouble(&converted))
					{
						*value = static_cast<float>(converted);
						return true;
					}

					break;
				}

			case ParameterType_Boolean:
			case ParameterType_None:
			case ParameterType_Pointer:
			case ParameterType_Userdata:
				break;
		}

		NazaraError("Parameter value is not representable as a float");
		return false;
	}

	/*!
	* \brief Gets the integer parameter by name
	* \return true if success
	*
	* \param name Name of the variable
	* \param value Value to set
	*
	* \remark Produces a NazaraAssert if pointer is invalid
	* \remark Produces a NazaraError if name is not a variable
	* \remark Produces a NazaraError if value could not be convertible
	*/

	bool ParameterList::GetIntegerParameter(const String& name, int* value) const
	{
		NazaraAssert(value, "Invalid pointer");

		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
		{
			NazaraError("Parameter \"" + name + "\" is not present");
			return false;
		}

		switch (it->second.type)
		{
			case ParameterType_Boolean:
				*value = (it->second.value.boolVal) ? 1 : 0;
				return true;

			case ParameterType_Float:
				*value = static_cast<int>(it->second.value.floatVal);
				return true;

			case ParameterType_Integer:
				*value = it->second.value.intVal;
				return false;

			case ParameterType_String:
				{
					long long converted;
					if (it->second.value.stringVal.ToInteger(&converted))
					{
						if (converted <= std::numeric_limits<int>::max() && converted >= std::numeric_limits<int>::min())
						{
							*value = static_cast<int>(converted);
							return true;
						}
					}
					break;
				}

			case ParameterType_None:
			case ParameterType_Pointer:
			case ParameterType_Userdata:
				break;
		}

		NazaraError("Parameter value is not representable as a integer");
		return false;
	}

	/*!
	* \brief Gets the parameter by name
	* \return true if success
	*
	* \param name Name of the variable
	* \param type Type to set
	*
	* \remark Produces a NazaraAssert if pointer is invalid
	* \remark Produces a NazaraError if name is not a variable
	* \remark Produces a NazaraError if value could not be convertible
	*/

	bool ParameterList::GetParameterType(const String& name, ParameterType* type) const
	{
		NazaraAssert(type, "Invalid pointer");

		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
			return false;

		*type = it->second.type;

		return true;
	}

	/*!
	* \brief Gets the pointer parameter by name
	* \return true if success
	*
	* \param name Name of the variable
	* \param value Value to set
	*
	* \remark Produces a NazaraAssert if pointer is invalid
	* \remark Produces a NazaraError if name is not a variable
	* \remark Produces a NazaraError if value could not be convertible
	*/

	bool ParameterList::GetPointerParameter(const String& name, void** value) const
	{
		NazaraAssert(value, "Invalid pointer");

		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
		{
			NazaraError("Parameter \"" + name + "\" is not present");
			return false;
		}

		switch (it->second.type)
		{
			case ParameterType_Pointer:
				*value = it->second.value.ptrVal;
				return true;

			case ParameterType_Userdata:
				*value = it->second.value.userdataVal->ptr;
				return true;

			case ParameterType_Boolean:
			case ParameterType_Float:
			case ParameterType_Integer:
			case ParameterType_None:
			case ParameterType_String:
				break;
		}

		NazaraError("Parameter value is not a pointer");
		return false;
	}

	/*!
	* \brief Gets the string parameter by name
	* \return true if success
	*
	* \param name Name of the variable
	* \param value Value to set
	*
	* \remark Produces a NazaraAssert if pointer is invalid
	* \remark Produces a NazaraError if name is not a variable
	* \remark Produces a NazaraError if value could not be convertible
	*/

	bool ParameterList::GetStringParameter(const String& name, String* value) const
	{
		NazaraAssert(value, "Invalid pointer");

		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
		{
			NazaraError("Parameter \"" + name + "\" is not present");
			return false;
		}

		switch (it->second.type)
		{
			case ParameterType_Boolean:
				*value = String::Boolean(it->second.value.boolVal);
				return true;

			case ParameterType_Float:
				*value = String::Number(it->second.value.floatVal);
				return true;

			case ParameterType_Integer:
				*value = String::Number(it->second.value.intVal);
				return true;

			case ParameterType_String:
				*value = it->second.value.stringVal;
				return true;

			case ParameterType_Pointer:
				*value = String::Pointer(it->second.value.ptrVal);
				return true;

			case ParameterType_Userdata:
				*value = String::Pointer(it->second.value.userdataVal->ptr);
				return true;

			case ParameterType_None:
				*value = String();
				return true;
		}

		NazaraInternalError("Parameter value is not valid");
		return false;
	}

	/*!
	* \brief Gets the user parameter by name
	* \return true if success
	*
	* \param name Name of the variable
	* \param value Value to set
	*
	* \remark Produces a NazaraAssert if pointer is invalid
	* \remark Produces a NazaraError if name is not a variable
	* \remark Produces a NazaraError if value could not be convertible
	*/

	bool ParameterList::GetUserdataParameter(const String& name, void** value) const
	{
		NazaraAssert(value, "Invalid pointer");

		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
		{
			NazaraError("Parameter \"" + name + "\" is not present");
			return false;
		}

		if (it->second.type == ParameterType_Userdata)
		{
			*value = it->second.value.userdataVal->ptr;
			return true;
		}
		else
		{
			NazaraError("Parameter value is not a userdata");
			return false;
		}
	}

	/*!
	* \brief Checks whether the parameter list has a parameter with that name
	* \return true if found
	*
	* \param name Name of the parameter
	*/

	bool ParameterList::HasParameter(const String& name) const
	{
		return m_parameters.find(name) != m_parameters.end();
	}

	/*!
	* \brief Removes the parameter with that name
	*
	* Removes the parameter with that name, if not found, nothing is done
	*
	* \param name Name of the parameter
	*/

	void ParameterList::RemoveParameter(const String& name)
	{
		auto it = m_parameters.find(name);
		if (it != m_parameters.end())
		{
			DestroyValue(it->second);
			m_parameters.erase(it);
		}
	}

	/*!
	* \brief Sets the parameter with the name to ParameterType_None
	*
	* \param name Name of the parameter
	*/

	void ParameterList::SetParameter(const String& name)
	{
		std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
		Parameter& parameter = pair.first->second;

		if (!pair.second)
			DestroyValue(parameter);

		parameter.type = ParameterType_None;
	}

	/*!
	* \brief Sets the parameter with the name to the value
	*
	* \param name Name of the parameter
	* \param value Value of the parameter
	*/

	void ParameterList::SetParameter(const String& name, const String& value)
	{
		std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
		Parameter& parameter = pair.first->second;

		if (!pair.second)
			DestroyValue(parameter);

		parameter.type = ParameterType_String;

		PlacementNew<String>(&parameter.value.stringVal, value);
	}

	/*!
	* \brief Sets the parameter with the name to the value
	*
	* \param name Name of the parameter
	* \param value Value of the parameter
	*/

	void ParameterList::SetParameter(const String& name, const char* value)
	{
		std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
		Parameter& parameter = pair.first->second;

		if (!pair.second)
			DestroyValue(parameter);

		parameter.type = ParameterType_String;

		PlacementNew<String>(&parameter.value.stringVal, value);
	}

	/*!
	* \brief Sets the parameter with the name to the value
	*
	* \param name Name of the parameter
	* \param value Value of the parameter
	*/

	void ParameterList::SetParameter(const String& name, void* value)
	{
		std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
		Parameter& parameter = pair.first->second;

		if (!pair.second)
			DestroyValue(parameter);

		parameter.type = ParameterType_Pointer;
		parameter.value.ptrVal = value;
	}

	/*!
	* \brief Sets the parameter with the name to the value
	*
	* \param name Name of the parameter
	* \param value Value of the parameter
	* \param destructor Destructor for dynamic variable
	*/

	void ParameterList::SetParameter(const String& name, void* value, Destructor destructor)
	{
		std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
		Parameter& parameter = pair.first->second;

		if (!pair.second)
			DestroyValue(parameter);

		parameter.type = ParameterType_Userdata;
		parameter.value.userdataVal = new Parameter::UserdataValue(destructor, value);
	}

	/*!
	* \brief Sets the parameter with the name to the value
	*
	* \param name Name of the parameter
	* \param value Value of the parameter
	*/

	void ParameterList::SetParameter(const String& name, bool value)
	{
		std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
		Parameter& parameter = pair.first->second;

		if (!pair.second)
			DestroyValue(parameter);

		parameter.type = ParameterType_Boolean;
		parameter.value.boolVal = value;
	}

	/*!
	* \brief Sets the parameter with the name to the value
	*
	* \param name Name of the parameter
	* \param value Value of the parameter
	*/

	void ParameterList::SetParameter(const String& name, float value)
	{
		std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
		Parameter& parameter = pair.first->second;

		if (!pair.second)
			DestroyValue(parameter);

		parameter.type = ParameterType_Float;
		parameter.value.floatVal = value;
	}

	/*!
	* \brief Sets the parameter with the name to the value
	*
	* \param name Name of the parameter
	* \param value Value of the parameter
	*/

	void ParameterList::SetParameter(const String& name, int value)
	{
		std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
		Parameter& parameter = pair.first->second;

		if (!pair.second)
			DestroyValue(parameter);

		parameter.type = ParameterType_Integer;
		parameter.value.intVal = value;
	}

	/*!
	* \brief Assigns the content of the other parameter list to this
	* \return A reference to this
	*
	* \param list List to assign
	*/

	ParameterList& ParameterList::operator=(const ParameterList& list)
	{
		Clear();

		for (auto it = list.m_parameters.begin(); it != list.m_parameters.end(); ++it)
		{
			Parameter& parameter = m_parameters[it->first];

			switch (it->second.type)
			{
				case ParameterType_Boolean:
				case ParameterType_Float:
				case ParameterType_Integer:
				case ParameterType_Pointer:
					std::memcpy(&parameter, &it->second, sizeof(Parameter));
					break;

				case ParameterType_String:
					parameter.type = ParameterType_String;

					PlacementNew<String>(&parameter.value.stringVal, it->second.value.stringVal);
					break;

				case ParameterType_Userdata:
					parameter.type = ParameterType_Userdata;
					parameter.value.userdataVal = it->second.value.userdataVal;
					++(parameter.value.userdataVal->counter);
					break;

				case ParameterType_None:
					parameter.type = ParameterType_None;
					break;
			}
		}

		return *this;
	}

	/*!
	* \brief Destroys the value for the parameter
	*
	* \param parameter Parameter to destroy
	*/

	void ParameterList::DestroyValue(Parameter& parameter)
	{
		switch (parameter.type)
		{
			case ParameterType_String:
				parameter.value.stringVal.~String();
				break;

			case ParameterType_Userdata:
				{
					Parameter::UserdataValue* userdata = parameter.value.userdataVal;
					if (--userdata->counter == 0)
					{
						userdata->destructor(userdata->ptr);
						delete userdata;
					}
					break;
				}

			case ParameterType_Boolean:
			case ParameterType_Float:
			case ParameterType_Integer:
			case ParameterType_None:
			case ParameterType_Pointer:
				break;
		}
	}
}
