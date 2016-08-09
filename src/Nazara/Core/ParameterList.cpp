// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
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
	* \brief Clears all the parameters
	*/
	void ParameterList::Clear()
	{
		for (auto it = m_parameters.begin(); it != m_parameters.end(); ++it)
			DestroyValue(it->second);

		m_parameters.clear();
	}

	/*!
	* \brief Gets a parameter as a boolean
	* \return true if the parameter could be represented as a boolean
	*
	* \param name Name of the parameter
	* \param value Pointer to a boolean to hold the retrieved value
	*
	* \remark value must be a valid pointer
	* \remark In case of failure, the variable pointed by value keep its value
	* \remark If the parameter is not a boolean, a conversion will be performed, compatibles types are:
	          Integer: 0 is interpreted as false, any other value is interpreted as true
	          String:  Conversion obeys the rule as described by String::ToBool
	*/
	bool ParameterList::GetBooleanParameter(const String& name, bool* value) const
	{
		NazaraAssert(value, "Invalid pointer");

		ErrorFlags flags(ErrorFlag_Silent | ErrorFlag_ThrowExceptionDisabled);

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

			case ParameterType_Color:
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
	* \brief Gets a parameter as a color
	* \return true if the parameter could be represented as a color
	*
	* \param name Name of the parameter
	* \param value Pointer to a color to hold the retrieved value
	*
	* \remark value must be a valid pointer
	* \remark In case of failure, the variable pointed by value keep its value
	* \remark If the parameter is not a color, the function fails
	*/
	bool ParameterList::GetColorParameter(const String& name, Color* value) const
	{
		NazaraAssert(value, "Invalid pointer");

		ErrorFlags flags(ErrorFlag_Silent | ErrorFlag_ThrowExceptionDisabled);

		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
		{
			NazaraError("Parameter \"" + name + "\" is not present");
			return false;
		}

		switch (it->second.type)
		{
			case ParameterType_Color:
				*value = it->second.value.colorVal;
				return true;

			case ParameterType_Boolean:
			case ParameterType_Integer:
			case ParameterType_String:
			case ParameterType_Float:
			case ParameterType_None:
			case ParameterType_Pointer:
			case ParameterType_Userdata:
				break;
		}

		NazaraError("Parameter value is not representable as a color");
		return false;
	}

	/*!
	* \brief Gets a parameter as a float
	* \return true if the parameter could be represented as a float
	*
	* \param name Name of the parameter
	* \param value Pointer to a float to hold the retrieved value
	*
	* \remark value must be a valid pointer
	* \remark In case of failure, the variable pointed by value keep its value
	* \remark If the parameter is not a float, a conversion will be performed, compatibles types are:
	          Integer: The integer value is converted to its float representation
	          String:  Conversion obeys the rule as described by String::ToDouble
	*/
	bool ParameterList::GetFloatParameter(const String& name, float* value) const
	{
		NazaraAssert(value, "Invalid pointer");

		ErrorFlags flags(ErrorFlag_Silent | ErrorFlag_ThrowExceptionDisabled);

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
			case ParameterType_Color:
			case ParameterType_None:
			case ParameterType_Pointer:
			case ParameterType_Userdata:
				break;
		}

		NazaraError("Parameter value is not representable as a float");
		return false;
	}

	/*!
	* \brief Gets a parameter as an integer
	* \return true if the parameter could be represented as an integer
	*
	* \param name Name of the parameter
	* \param value Pointer to an integer to hold the retrieved value
	*
	* \remark value must be a valid pointer
	* \remark In case of failure, the variable pointed by value keep its value
	* \remark If the parameter is not a float, a conversion will be performed, compatibles types are:
	          Boolean: The boolean is represented as 1 if true and 0 if false
	          Float:   The floating-point value is truncated and converted to a integer
	          String:  Conversion obeys the rule as described by String::ToInteger but fails if the value could not be represented as a int
	*/
	bool ParameterList::GetIntegerParameter(const String& name, int* value) const
	{
		NazaraAssert(value, "Invalid pointer");

		ErrorFlags flags(ErrorFlag_Silent | ErrorFlag_ThrowExceptionDisabled);

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
				return true;

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

			case ParameterType_Color:
			case ParameterType_None:
			case ParameterType_Pointer:
			case ParameterType_Userdata:
				break;
		}

		NazaraError("Parameter value is not representable as a integer");
		return false;
	}

	/*!
	* \brief Gets a parameter type
	* \return true if the parameter is present, its type being written to type
	*
	* \param name Name of the variable
	* \param type Pointer to a variable to hold the result
	*
	* \remark type must be a valid pointer to a ParameterType variable
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
	* \brief Gets a parameter as a pointer
	* \return true if the parameter could be represented as a pointer
	*
	* \param name Name of the parameter
	* \param value Pointer to a pointer to hold the retrieved value
	*
	* \remark value must be a valid pointer
	* \remark In case of failure, the variable pointed by value keep its value
	* \remark If the parameter is not a pointer, a conversion will be performed, compatibles types are:
	          Userdata: The pointer part of the userdata is returned
	*/
	bool ParameterList::GetPointerParameter(const String& name, void** value) const
	{
		NazaraAssert(value, "Invalid pointer");

		ErrorFlags flags(ErrorFlag_Silent | ErrorFlag_ThrowExceptionDisabled);

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
			case ParameterType_Color:
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
	* \brief Gets a parameter as a string
	* \return true if the parameter could be represented as a string
	*
	* \param name Name of the parameter
	* \param value Pointer to a pointer to hold the retrieved value
	*
	* \remark value must be a valid pointer
	* \remark In case of failure, the variable pointed by value keep its value
	* \remark If the parameter is not a string, a conversion will be performed, all types are compatibles:
	          Boolean:  Conversion obeys the rules of String::Boolean
	          Color:    Conversion obeys the rules of Color::ToString
	          Float:    Conversion obeys the rules of String::Number
	          Integer:  Conversion obeys the rules of String::Number
	          None:     An empty string is returned
	          Pointer:  Conversion obeys the rules of String::Pointer
	          Userdata: Conversion obeys the rules of String::Pointer
	*/
	bool ParameterList::GetStringParameter(const String& name, String* value) const
	{
		NazaraAssert(value, "Invalid pointer");

		ErrorFlags flags(ErrorFlag_Silent | ErrorFlag_ThrowExceptionDisabled);

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

			case ParameterType_Color:
				*value = it->second.value.colorVal.ToString();
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
	* \brief Gets a parameter as an userdata
	* \return true if the parameter could be represented as a userdata
	*
	* \param name Name of the parameter
	* \param value Pointer to a pointer to hold the retrieved value
	*
	* \remark value must be a valid pointer
	* \remark In case of failure, the variable pointed by value keep its value
	* \remark If the parameter is not an userdata, the function fails
	*
	* \see GetPointerParameter
	*/
	bool ParameterList::GetUserdataParameter(const String& name, void** value) const
	{
		NazaraAssert(value, "Invalid pointer");

		ErrorFlags flags(ErrorFlag_Silent | ErrorFlag_ThrowExceptionDisabled);

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
	* \brief Checks whether the parameter list contains a parameter named `name`
	* \return true if found
	*
	* \param name Name of the parameter
	*/
	bool ParameterList::HasParameter(const String& name) const
	{
		return m_parameters.find(name) != m_parameters.end();
	}

	/*!
	* \brief Removes the parameter named `name`
	*
	* Search for a parameter named `name` and remove it from the parameter list, freeing up its memory
	* Nothing is done if the parameter is not present in the parameter list
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
	* \brief Sets a null parameter named `name`
	*
	* If a parameter already exists with that name, it is destroyed and replaced by this call
	*
	* \param name Name of the parameter
	*/
	void ParameterList::SetParameter(const String& name)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType_None;
	}

	/*!
	* \brief Sets a color parameter named `name`
	*
	* If a parameter already exists with that name, it is destroyed and replaced by this call
	*
	* \param name Name of the parameter
	* \param value The color value
	*/
	void ParameterList::SetParameter(const String& name, const Color& value)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType_Color;

		PlacementNew(&parameter.value.colorVal, value);
	}

	/*!
	* \brief Sets a string parameter named `name`
	*
	* If a parameter already exists with that name, it is destroyed and replaced by this call
	*
	* \param name Name of the parameter
	* \param value The string value
	*/
	void ParameterList::SetParameter(const String& name, const String& value)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType_String;

		PlacementNew(&parameter.value.stringVal, value);
	}

	/*!
	* \brief Sets a string parameter named `name`
	*
	* If a parameter already exists with that name, it is destroyed and replaced by this call
	*
	* \param name Name of the parameter
	* \param value The string value
	*/
	void ParameterList::SetParameter(const String& name, const char* value)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType_String;

		PlacementNew(&parameter.value.stringVal, value);
	}

	/*!
	* \brief Sets a boolean parameter named `name`
	*
	* If a parameter already exists with that name, it is destroyed and replaced by this call
	*
	* \param name Name of the parameter
	* \param value The boolean value
	*/
	void ParameterList::SetParameter(const String& name, bool value)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType_Boolean;
		parameter.value.boolVal = value;
	}

	/*!
	* \brief Sets a float parameter named `name`
	*
	* If a parameter already exists with that name, it is destroyed and replaced by this call
	*
	* \param name Name of the parameter
	* \param value The float value
	*/
	void ParameterList::SetParameter(const String& name, float value)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType_Float;
		parameter.value.floatVal = value;
	}

	/*!
	* \brief Sets an integer parameter named `name`
	*
	* If a parameter already exists with that name, it is destroyed and replaced by this call
	*
	* \param name Name of the parameter
	* \param value The integer value
	*/
	void ParameterList::SetParameter(const String& name, int value)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType_Integer;
		parameter.value.intVal = value;
	}

	/*!
	* \brief Sets a pointer parameter named `name`
	*
	* If a parameter already exists with that name, it is destroyed and replaced by this call
	*
	* \param name Name of the parameter
	* \param value The pointer value
	*
	* \remark This sets a raw pointer, this class takes no responsibility toward it,
	          if you wish to destroy the pointed variable along with the parameter list, you should set a userdata
	*/
	void ParameterList::SetParameter(const String& name, void* value)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType_Pointer;
		parameter.value.ptrVal = value;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "ParameterList(Name: Type(value), ...)"
	*/
	String ParameterList::ToString() const
	{
		StringStream ss;

		ss << "ParameterList(";
		for (auto it = m_parameters.cbegin(); it != m_parameters.cend();)
		{
			ss << it->first << ": ";
			switch (it->second.type)
			{
				case ParameterType_Boolean:
					ss << "Boolean(" << String::Boolean(it->second.value.boolVal) << ")";
					break;
				case ParameterType_Color:
					ss << "Color(" << it->second.value.colorVal.ToString() << ")";
					break;
				case ParameterType_Float:
					ss << "Float(" << it->second.value.floatVal << ")";
					break;
				case ParameterType_Integer:
					ss << "Integer(" << it->second.value.intVal << ")";
					break;
				case ParameterType_String:
					ss << "String(" << it->second.value.stringVal << ")";
					break;
				case ParameterType_Pointer:
					ss << "Pointer(" << String::Pointer(it->second.value.ptrVal) << ")";
					break;
				case ParameterType_Userdata:
					ss << "Userdata(" << String::Pointer(it->second.value.userdataVal->ptr) << ")";
					break;
				case ParameterType_None:
					ss << "None";
					break;
			}

			if (++it != m_parameters.cend())
				ss << ", ";
		}
		ss << ")";

		return ss;
	}

	/*!
	* \brief Sets a userdata parameter named `name`
	*
	* If a parameter already exists with that name, it is destroyed and replaced by this call
	*
	* \param name Name of the parameter
	* \param value The pointer value
	* \param destructor The destructor function to be called upon parameter suppression
	*
	* \remark The destructor is called once when all copies of the userdata are destroyed, which means
	          you can safely copy the parameter list around.
	*/
	void ParameterList::SetParameter(const String& name, void* value, Destructor destructor)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType_Userdata;
		parameter.value.userdataVal = new Parameter::UserdataValue(destructor, value);
	}

	/*!
	* \brief Copies the content of the other parameter list to this
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
				case ParameterType_Color:
				case ParameterType_Float:
				case ParameterType_Integer:
				case ParameterType_Pointer:
					std::memcpy(&parameter, &it->second, sizeof(Parameter));
					break;

				case ParameterType_String:
					parameter.type = ParameterType_String;

					PlacementNew(&parameter.value.stringVal, it->second.value.stringVal);
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
	* \brief Create an uninitialized value of a set name
	*
	* \param name Name of the parameter
	* \param value Value of the parameter
	*
	* \remark The previous value if any gets destroyed
	*/
	ParameterList::Parameter& ParameterList::CreateValue(const String& name)
	{
		std::pair<ParameterMap::iterator, bool> pair = m_parameters.insert(std::make_pair(name, Parameter()));
		Parameter& parameter = pair.first->second;

		if (!pair.second)
			DestroyValue(parameter);

		return parameter;
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
			case ParameterType_Color:
			case ParameterType_Float:
			case ParameterType_Integer:
			case ParameterType_None:
			case ParameterType_Pointer:
				break;
		}
	}
}

/*!
* \brief Output operator
* \return The stream
*
* \param out The stream
* \param parameterList The ParameterList to output
*/

std::ostream& operator<<(std::ostream& out, const Nz::ParameterList& parameterList)
{
	out << parameterList.ToString();
	return out;
}
