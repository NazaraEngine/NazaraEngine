// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Utils/MemoryHelper.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <cstring>
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
		for (auto& parameter : m_parameters)
			DestroyValue(parameter.second);

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
	          std::string:  Conversion obeys the rule as described by std::string::ToBool
	*/
	bool ParameterList::GetBooleanParameter(const std::string& name, bool* value) const
	{
		NazaraAssert(value, "Invalid pointer");

		ErrorFlags flags(ErrorMode::Silent | ErrorMode::ThrowExceptionDisabled);

		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
		{
			NazaraError("Parameter \"" + name + "\" is not present");
			return false;
		}

		switch (it->second.type)
		{
			case ParameterType::Boolean:
				*value = it->second.value.boolVal;
				return true;

			case ParameterType::Integer:
				*value = (it->second.value.intVal != 0);
				return true;

			case ParameterType::String:
			{
				if (it->second.value.stringVal == "1" || it->second.value.stringVal == "yes" || it->second.value.stringVal == "true")
				{
					*value = true;
					return true;
				}
				else if (it->second.value.stringVal == "0" || it->second.value.stringVal == "no" || it->second.value.stringVal == "false")
				{
					*value = false;
					return true;
				}

				break;
			}

			case ParameterType::Color:
			case ParameterType::Double:
			case ParameterType::None:
			case ParameterType::Pointer:
			case ParameterType::Userdata:
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
	bool ParameterList::GetColorParameter(const std::string& name, Color* value) const
	{
		NazaraAssert(value, "Invalid pointer");

		ErrorFlags flags(ErrorMode::Silent | ErrorMode::ThrowExceptionDisabled);

		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
		{
			NazaraError("Parameter \"" + name + "\" is not present");
			return false;
		}

		switch (it->second.type)
		{
			case ParameterType::Color:
				*value = it->second.value.colorVal;
				return true;

			case ParameterType::Boolean:
			case ParameterType::Double:
			case ParameterType::Integer:
			case ParameterType::String:
			case ParameterType::None:
			case ParameterType::Pointer:
			case ParameterType::Userdata:
				break;
		}

		NazaraError("Parameter value is not representable as a color");
		return false;
	}

	/*!
	* \brief Gets a parameter as a double
	* \return true if the parameter could be represented as a double
	*
	* \param name Name of the parameter
	* \param value Pointer to a double to hold the retrieved value
	*
	* \remark value must be a valid pointer
	* \remark In case of failure, the variable pointed by value keep its value
	* \remark If the parameter is not a double, a conversion will be performed, compatibles types are:
	          Integer: The integer value is converted to its double representation
	          std::string:  Conversion obeys the rule as described by std::string::ToDouble
	*/
	bool ParameterList::GetDoubleParameter(const std::string& name, double* value) const
	{
		NazaraAssert(value, "Invalid pointer");

		ErrorFlags flags(ErrorMode::Silent | ErrorMode::ThrowExceptionDisabled);

		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
		{
			NazaraError("Parameter \"" + name + "\" is not present");
			return false;
		}

		switch (it->second.type)
		{
			case ParameterType::Double:
				*value = it->second.value.doubleVal;
				return true;

			case ParameterType::Integer:
				*value = static_cast<double>(it->second.value.intVal);
				return true;

			case ParameterType::String:
			{
				const std::string& str = it->second.value.stringVal;

				int& err = errno;
				err = 0;

				char* endStr;
				double ret = std::strtod(str.data(), &endStr);

				if (str.data() == endStr || err == ERANGE)
					break;

				*value = ret;
				return true;
			}

			case ParameterType::Boolean:
			case ParameterType::Color:
			case ParameterType::None:
			case ParameterType::Pointer:
			case ParameterType::Userdata:
				break;
		}

		NazaraError("Parameter value is not representable as a double");
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
	* \remark If the parameter is not an integer, a conversion will be performed, compatibles types are:
	          Boolean: The boolean is represented as 1 if true and 0 if false
	          Double:  The floating-point value is truncated and converted to a integer
	          std::string:  Conversion obeys the rule as described by std::string::ToInteger
	*/
	bool ParameterList::GetIntegerParameter(const std::string& name, long long* value) const
	{
		NazaraAssert(value, "Invalid pointer");

		ErrorFlags flags(ErrorMode::Silent | ErrorMode::ThrowExceptionDisabled);

		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
		{
			NazaraError("Parameter \"" + name + "\" is not present");
			return false;
		}

		switch (it->second.type)
		{
			case ParameterType::Boolean:
				*value = (it->second.value.boolVal) ? 1 : 0;
				return true;

			case ParameterType::Double:
				*value = static_cast<long long>(it->second.value.doubleVal);
				return true;

			case ParameterType::Integer:
				*value = it->second.value.intVal;
				return true;

			case ParameterType::String:
			{
				const std::string& str = it->second.value.stringVal;

				int& err = errno;
				err = 0;

				char* endStr;
				long long ret = std::strtoll(str.data(), &endStr, 0);

				if (str.data() == endStr || err == ERANGE)
					break;

				*value = ret;
				return true;
			}

			case ParameterType::Color:
			case ParameterType::None:
			case ParameterType::Pointer:
			case ParameterType::Userdata:
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
	bool ParameterList::GetParameterType(const std::string& name, ParameterType* type) const
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
	bool ParameterList::GetPointerParameter(const std::string& name, void** value) const
	{
		NazaraAssert(value, "Invalid pointer");

		ErrorFlags flags(ErrorMode::Silent | ErrorMode::ThrowExceptionDisabled);

		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
		{
			NazaraError("Parameter \"" + name + "\" is not present");
			return false;
		}

		switch (it->second.type)
		{
			case ParameterType::Pointer:
				*value = it->second.value.ptrVal;
				return true;

			case ParameterType::Userdata:
				*value = it->second.value.userdataVal->ptr;
				return true;

			case ParameterType::Boolean:
			case ParameterType::Color:
			case ParameterType::Double:
			case ParameterType::Integer:
			case ParameterType::None:
			case ParameterType::String:
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
	          Boolean:  Conversion obeys the rules of std::string::Boolean
	          Color:    Conversion obeys the rules of Color::ToString
	          Double:   Conversion obeys the rules of std::string::Number
	          Integer:  Conversion obeys the rules of std::string::Number
	          None:     An empty string is returned
	          Pointer:  Conversion obeys the rules of PointerToString
	          Userdata: Conversion obeys the rules of PointerToString
	*/
	bool ParameterList::GetStringParameter(const std::string& name, std::string* value) const
	{
		NazaraAssert(value, "Invalid pointer");

		ErrorFlags flags(ErrorMode::Silent | ErrorMode::ThrowExceptionDisabled);

		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
		{
			NazaraError("Parameter \"" + name + "\" is not present");
			return false;
		}

		switch (it->second.type)
		{
			case ParameterType::Boolean:
				*value = (it->second.value.boolVal) ? "true" : "false";
				return true;

			case ParameterType::Color:
				*value = it->second.value.colorVal.ToString();
				return true;

			case ParameterType::Double:
				*value = std::to_string(it->second.value.doubleVal);
				return true;

			case ParameterType::Integer:
				*value = std::to_string(it->second.value.intVal);
				return true;

			case ParameterType::String:
				*value = it->second.value.stringVal;
				return true;

			case ParameterType::Pointer:
				*value = PointerToString(it->second.value.ptrVal);
				return true;

			case ParameterType::Userdata:
				*value = PointerToString(it->second.value.userdataVal->ptr);
				return true;

			case ParameterType::None:
				*value = std::string();
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
	bool ParameterList::GetUserdataParameter(const std::string& name, void** value) const
	{
		NazaraAssert(value, "Invalid pointer");

		ErrorFlags flags(ErrorMode::Silent | ErrorMode::ThrowExceptionDisabled);

		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
		{
			NazaraError("Parameter \"" + name + "\" is not present");
			return false;
		}

		const auto& parameter = it->second;

		if (parameter.type == ParameterType::Userdata)
		{
			*value = parameter.value.userdataVal->ptr;
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
	bool ParameterList::HasParameter(const std::string& name) const
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
	void ParameterList::RemoveParameter(const std::string& name)
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
	void ParameterList::SetParameter(const std::string& name)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType::None;
	}

	/*!
	* \brief Sets a color parameter named `name`
	*
	* If a parameter already exists with that name, it is destroyed and replaced by this call
	*
	* \param name Name of the parameter
	* \param value The color value
	*/
	void ParameterList::SetParameter(const std::string& name, const Color& value)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType::Color;

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
	void ParameterList::SetParameter(const std::string& name, const std::string& value)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType::String;

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
	void ParameterList::SetParameter(const std::string& name, const char* value)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType::String;

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
	void ParameterList::SetParameter(const std::string& name, bool value)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType::Boolean;
		parameter.value.boolVal = value;
	}

	/*!
	* \brief Sets a double parameter named `name`
	*
	* If a parameter already exists with that name, it is destroyed and replaced by this call
	*
	* \param name Name of the parameter
	* \param value The double value
	*/
	void ParameterList::SetParameter(const std::string& name, double value)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType::Double;
		parameter.value.doubleVal = value;
	}

	/*!
	* \brief Sets an integer parameter named `name`
	*
	* If a parameter already exists with that name, it is destroyed and replaced by this call
	*
	* \param name Name of the parameter
	* \param value The integer value
	*/
	void ParameterList::SetParameter(const std::string& name, long long value)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType::Integer;
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
	void ParameterList::SetParameter(const std::string& name, void* value)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType::Pointer;
		parameter.value.ptrVal = value;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "ParameterList(Name: Type(value), ...)"
	*/
	std::string ParameterList::ToString() const
	{
		std::ostringstream ss;
		ss << std::boolalpha;

		ss << "ParameterList(";
		for (auto it = m_parameters.cbegin(); it != m_parameters.cend();)
		{
			const auto& parameter = it->second;

			ss << it->first << ": ";
			switch (it->second.type)
			{
				case ParameterType::Boolean:
					ss << "Boolean(" << parameter.value.boolVal << ")";
					break;
				case ParameterType::Color:
					ss << "Color(" << parameter.value.colorVal.ToString() << ")";
					break;
				case ParameterType::Double:
					ss << "Double(" << parameter.value.doubleVal << ")";
					break;
				case ParameterType::Integer:
					ss << "Integer(" << parameter.value.intVal << ")";
					break;
				case ParameterType::String:
					ss << "std::string(" << parameter.value.stringVal << ")";
					break;
				case ParameterType::Pointer:
					ss << "Pointer(" << parameter.value.ptrVal << ")";
					break;
				case ParameterType::Userdata:
					ss << "Userdata(" << parameter.value.userdataVal->ptr << ")";
					break;
				case ParameterType::None:
					ss << "None";
					break;
			}

			if (++it != m_parameters.cend())
				ss << ", ";
		}
		ss << ")";

		return ss.str();
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
	void ParameterList::SetParameter(const std::string& name, void* value, Destructor destructor)
	{
		Parameter& parameter = CreateValue(name);
		parameter.type = ParameterType::Userdata;
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
				case ParameterType::Boolean:
				case ParameterType::Color:
				case ParameterType::Double:
				case ParameterType::Integer:
				case ParameterType::Pointer:
					std::memcpy(&parameter, &it->second, sizeof(Parameter));
					break;

				case ParameterType::String:
					parameter.type = ParameterType::String;

					PlacementNew(&parameter.value.stringVal, it->second.value.stringVal);
					break;

				case ParameterType::Userdata:
					parameter.type = ParameterType::Userdata;
					parameter.value.userdataVal = it->second.value.userdataVal;
					++(parameter.value.userdataVal->counter);
					break;

				case ParameterType::None:
					parameter.type = ParameterType::None;
					break;
			}
		}

		return *this;
	}

	/*!
	* \brief Create an uninitialized value of a set name
	*
	* \param name Name of the parameter
	*
	* \remark The previous value if any gets destroyed
	*/
	ParameterList::Parameter& ParameterList::CreateValue(const std::string& name)
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
			case ParameterType::String:
				PlacementDestroy(&parameter.value.stringVal);
				break;

			case ParameterType::Userdata:
				{
					Parameter::UserdataValue* userdata = parameter.value.userdataVal;
					if (--userdata->counter == 0)
					{
						userdata->destructor(userdata->ptr);
						delete userdata;
					}
					break;
				}

			case ParameterType::Boolean:
			case ParameterType::Color:
			case ParameterType::Double:
			case ParameterType::Integer:
			case ParameterType::None:
			case ParameterType::Pointer:
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
