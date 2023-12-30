// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <NazaraUtils/MemoryHelper.hpp>
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
	* \return result containing the value or an error
	*
	* \param name Name of the parameter
	* \param strict If true, prevent conversions from compatible types
	*
	* \remark If the parameter is not a boolean, a conversion may be performed if strict parameter is set to false, compatibles types are:
	          Integer: 0 is interpreted as false, any other value is interpreted as true
	          std::string:  Conversion obeys the rule as described by std::string::ToBool
	*/
	auto ParameterList::GetBooleanParameter(std::string_view name, bool strict) const -> Result<bool, Error>
	{
		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
			return Err(Error::MissingValue);

		switch (it->second.type)
		{
			case ParameterType::Boolean:
				return it->second.value.boolVal;

			case ParameterType::Integer:
				if (strict)
					return Err(Error::WouldRequireConversion);

				return (it->second.value.intVal != 0);

			case ParameterType::String:
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				if (it->second.value.stringVal == "1" || it->second.value.stringVal == "yes" || it->second.value.stringVal == "true")
					return true;
				else if (it->second.value.stringVal == "0" || it->second.value.stringVal == "no" || it->second.value.stringVal == "false")
					return false;

				return Err(Error::ConversionFailed);
			}

			case ParameterType::Color:
			case ParameterType::Double:
			case ParameterType::None:
			case ParameterType::Pointer:
			case ParameterType::Userdata:
				break;
		}

		return Err(Error::WrongType);
	}

	/*!
	* \brief Gets a parameter as a color
	* \return result containing the value or an error
	*
	* \param name Name of the parameter
	* \param strict If true, prevent conversions from compatible types
	*
	* \remark If the parameter is not a color, the function fails
	*/
	auto ParameterList::GetColorParameter(std::string_view name, bool /*strict*/) const -> Result<Color, Error>
	{
		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
			return Err(Error::MissingValue);

		switch (it->second.type)
		{
			case ParameterType::Color:
				return it->second.value.colorVal;

			case ParameterType::Boolean:
			case ParameterType::Double:
			case ParameterType::Integer:
			case ParameterType::String:
			case ParameterType::None:
			case ParameterType::Pointer:
			case ParameterType::Userdata:
				break;
		}

		return Err(Error::WrongType);
	}

	/*!
	* \brief Gets a parameter as a double
	* \return result containing the value or an error
	*
	* \param name Name of the parameter
	* \param strict If true, prevent conversions from compatible types
	*
	* \remark If the parameter is not a double, a conversion may be performed if strict parameter is set to false, compatibles types are:
	          Integer: The integer value is converted to its double representation
	          std::string:  Conversion obeys the rule as described by std::string::ToDouble
	*/
	auto ParameterList::GetDoubleParameter(std::string_view name, bool strict) const -> Result<double, Error>
	{
		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
			return Err(Error::MissingValue);

		switch (it->second.type)
		{
			case ParameterType::Double:
				return it->second.value.doubleVal;

			case ParameterType::Integer:
				if (strict)
					return Err(Error::WouldRequireConversion);

				return static_cast<double>(it->second.value.intVal);

			case ParameterType::String:
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				const std::string& str = it->second.value.stringVal;

				int& err = errno;
				err = 0;

				char* endStr;
				double ret = std::strtod(str.data(), &endStr);

				if (str.data() == endStr || err == ERANGE)
					break;

				return ret;
			}

			case ParameterType::Boolean:
			case ParameterType::Color:
			case ParameterType::None:
			case ParameterType::Pointer:
			case ParameterType::Userdata:
				break;
		}

		return Err(Error::WrongType);
	}

	/*!
	* \brief Gets a parameter as an integer
	* \return result containing the value or an error
	*
	* \param name Name of the parameter
	* \param strict If true, prevent conversions from compatible types
	*
	* \remark If the parameter is not an integer, a conversion may be performed if strict parameter is set to false, compatibles types are:
	          Boolean: The boolean is represented as 1 if true and 0 if false
	          Double:  The floating-point value is truncated and converted to a integer
	          std::string:  Conversion obeys the rule as described by std::string::ToInteger
	*/
	auto ParameterList::GetIntegerParameter(std::string_view name, bool strict) const -> Result<long long, Error>
	{
		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
			return Err(Error::MissingValue);

		switch (it->second.type)
		{
			case ParameterType::Boolean:
				if (strict)
					return Err(Error::WouldRequireConversion);

				return (it->second.value.boolVal) ? 1LL : 0LL;

			case ParameterType::Double:
				if (strict)
					return Err(Error::WouldRequireConversion);

				return static_cast<long long>(it->second.value.doubleVal);

			case ParameterType::Integer:
				return it->second.value.intVal;

			case ParameterType::String:
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				const std::string& str = it->second.value.stringVal;

				int& err = errno;
				err = 0;

				char* endStr;
				long long ret = std::strtoll(str.data(), &endStr, 0);

				if (str.data() == endStr || err == ERANGE)
					break;

				return ret;
			}

			case ParameterType::Color:
			case ParameterType::None:
			case ParameterType::Pointer:
			case ParameterType::Userdata:
				break;
		}

		return Err(Error::WrongType);
	}

	/*!
	* \brief Gets a parameter type
	* \return result containing the parameter type or an error
	*
	* \param name Name of the variable
	*
	* \remark type must be a valid pointer to a ParameterType variable
	*/
	auto ParameterList::GetParameterType(std::string_view name) const -> Result<ParameterType, Error>
	{
		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
			return Err(Error::MissingValue);

		return it->second.type;
	}

	/*!
	* \brief Gets a parameter as a pointer
	* \return result containing the value or an error
	*
	* \param name Name of the parameter
	* \param strict If true, prevent conversions from compatible types
	*
	* \remark If the parameter is not a pointer, a conversion may be performed if strict parameter is set to false, compatibles types are:
	          Userdata: The pointer part of the userdata is returned
	*/
	auto ParameterList::GetPointerParameter(std::string_view name, bool strict) const -> Result<void*, Error>
	{
		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
			return Err(Error::MissingValue);

		switch (it->second.type)
		{
			case ParameterType::Pointer:
				return it->second.value.ptrVal;

			case ParameterType::Userdata:
				if (strict)
					return Err(Error::WouldRequireConversion);

				return it->second.value.userdataVal->ptr.Get();

			case ParameterType::Boolean:
			case ParameterType::Color:
			case ParameterType::Double:
			case ParameterType::Integer:
			case ParameterType::None:
			case ParameterType::String:
				break;
		}

		return Err(Error::WrongType);
	}

	/*!
	* \brief Gets a parameter as a string
	* \return result containing the value or an error
	*
	* \param name Name of the parameter
	* \param strict If true, prevent conversions from compatible types
	*
	* \remark If the parameter is not a string, a conversion may be performed if strict parameter is set to false, all types are compatibles:
	          Boolean:  Returns "true" or "false" as a string
	          Color:    Conversion obeys the rules of Color::ToString
	          Double:   Conversion obeys the rules of std::to_string
	          Integer:  Conversion obeys the rules of std::to_string
	          None:     An empty string is returned
	          Pointer:  Conversion obeys the rules of PointerToString
	          Userdata: Conversion obeys the rules of PointerToString
	*/
	auto ParameterList::GetStringParameter(std::string_view name, bool strict) const -> Result<std::string, Error>
	{
		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
			return Err(Error::MissingValue);

		switch (it->second.type)
		{
			case ParameterType::Boolean:
				if (strict)
					return Err(Error::WouldRequireConversion);

				return std::string{ (it->second.value.boolVal) ? "true" : "false" };

			case ParameterType::Color:
				if (strict)
					return Err(Error::WouldRequireConversion);

				return it->second.value.colorVal.ToString();

			case ParameterType::Double:
				if (strict)
					return Err(Error::WouldRequireConversion);

				return std::to_string(it->second.value.doubleVal);

			case ParameterType::Integer:
				if (strict)
					return Err(Error::WouldRequireConversion);

				return std::to_string(it->second.value.intVal);

			case ParameterType::String:
				return it->second.value.stringVal;

			case ParameterType::Pointer:
				if (strict)
					return Err(Error::WouldRequireConversion);

				return PointerToString(it->second.value.ptrVal);

			case ParameterType::Userdata:
				if (strict)
					return Err(Error::WouldRequireConversion);

				return PointerToString(it->second.value.userdataVal->ptr);

			case ParameterType::None:
				if (strict)
					return Err(Error::WouldRequireConversion);

				return std::string{};
		}

		return Err(Error::WrongType);
	}

	/*!
	* \brief Gets a parameter as a string view
	* \return result containing the value or an error
	*
	* \param name Name of the parameter
	* \param strict If true, prevent conversions from compatible types
	*
	* \remark If the parameter is not a string, a conversion may be performed if strict parameter is set to false, the following types are compatibles:
			  Boolean:  A string view containing true or false
			  None:     An empty string view is returned
	*/
	auto ParameterList::GetStringViewParameter(std::string_view name, bool strict) const -> Result<std::string_view, Error>
	{
		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
			return Err(Error::MissingValue);

		switch (it->second.type)
		{
			case ParameterType::Boolean:
				if (strict)
					return Err(Error::WouldRequireConversion);

				return std::string_view{ (it->second.value.boolVal) ? "true" : "false" };

			case ParameterType::String:
				return std::string_view{ it->second.value.stringVal };

			case ParameterType::None:
				if (strict)
					return Err(Error::WouldRequireConversion);

				return std::string_view{};

			case ParameterType::Color:
			case ParameterType::Double:
			case ParameterType::Integer:
			case ParameterType::Pointer:
			case ParameterType::Userdata:
				break;
		}

		return Err(Error::WrongType);
	}

	/*!
	* \brief Gets a parameter as an userdata
	* \return result containing the value or an error
	*
	* \param name Name of the parameter
	* \param strict If true, prevent conversions from compatible types
	*
	* \remark If the parameter is not an userdata, the function fails
	*
	* \see GetPointerParameter
	*/
	auto ParameterList::GetUserdataParameter(std::string_view name, bool /*strict*/) const -> Result<void*, Error>
	{
		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
			return Err(Error::MissingValue);

		const auto& parameter = it->second;

		if (parameter.type != ParameterType::Userdata)
			return Err(Error::WrongType);

		return parameter.value.userdataVal->ptr.Get();
	}

	/*!
	* \brief Checks whether the parameter list contains a parameter named `name`
	* \return true if found
	*
	* \param name Name of the parameter
	*/
	bool ParameterList::HasParameter(std::string_view name) const
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
	void ParameterList::RemoveParameter(std::string_view name)
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
	void ParameterList::SetParameter(std::string name)
	{
		Parameter& parameter = CreateValue(std::move(name));
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
	void ParameterList::SetParameter(std::string name, const Color& value)
	{
		Parameter& parameter = CreateValue(std::move(name));
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
	void ParameterList::SetParameter(std::string name, std::string value)
	{
		Parameter& parameter = CreateValue(std::move(name));
		parameter.type = ParameterType::String;

		PlacementNew(&parameter.value.stringVal, std::move(value));
	}

	/*!
	* \brief Sets a string parameter named `name`
	*
	* If a parameter already exists with that name, it is destroyed and replaced by this call
	*
	* \param name Name of the parameter
	* \param value The string value
	*/
	void ParameterList::SetParameter(std::string name, const char* value)
	{
		Parameter& parameter = CreateValue(std::move(name));
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
	void ParameterList::SetParameter(std::string name, bool value)
	{
		Parameter& parameter = CreateValue(std::move(name));
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
	void ParameterList::SetParameter(std::string name, double value)
	{
		Parameter& parameter = CreateValue(std::move(name));
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
	void ParameterList::SetParameter(std::string name, long long value)
	{
		Parameter& parameter = CreateValue(std::move(name));
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
	void ParameterList::SetParameter(std::string name, void* value)
	{
		Parameter& parameter = CreateValue(std::move(name));
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
	void ParameterList::SetParameter(std::string name, void* value, Destructor destructor)
	{
		Parameter& parameter = CreateValue(std::move(name));
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
	ParameterList::Parameter& ParameterList::CreateValue(std::string&& name)
	{
		auto [it, newParam] = m_parameters.emplace(std::move(name), Parameter{});
		Parameter& parameter = it->second;

		if (!newParam)
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
}
