// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <NazaraUtils/TypeTraits.hpp>
#include <cstring>

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

		return std::visit(Overloaded{
			[](Primitive<bool> primitive) -> Result<bool, Error> { return primitive.value; },
			[&](Primitive<long long> primitive) -> Result<bool, Error>
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				return (primitive.value != 0);
			},
			[&](const std::string& value) -> Result<bool, Error>
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				if (value == "1" || value == "yes" || value == "true")
					return true;
				else if (value == "0" || value == "no" || value == "false")
					return false;

				return Err(Error::ConversionFailed);
			},
			[](auto) -> Result<bool, Error> { return Err(Error::WrongType); }
		}, it->second);
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

		if (!std::holds_alternative<Color>(it->second))
			return Err(Error::WrongType);

		return std::get<Color>(it->second);
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

		return std::visit(Overloaded{
			[&](Primitive<double> primitive) -> Result<double, Error>
			{
				return primitive.value;
			},
			[&](Primitive<long long> primitive) -> Result<double, Error>
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				return static_cast<double>(primitive.value);
			},
			[&](const std::string& value) -> Result<double, Error>
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				int& err = errno;
				err = 0;

				char* endStr;
				double ret = std::strtod(value.data(), &endStr);

				if (value.data() == endStr || err == ERANGE)
					return Err(Error::ConversionFailed);

				return ret;
			},
			[](auto) -> Result<double, Error> { return Err(Error::WrongType); }
		}, it->second);
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

		return std::visit(Overloaded{
			[&](Primitive<bool> primitive) -> Result<long long, Error>
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				return (primitive.value) ? 1LL : 0LL;
			},
			[&](Primitive<double> primitive) -> Result<long long, Error>
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				return static_cast<long long>(primitive.value);
			},
			[&](Primitive<long long> primitive) -> Result<long long, Error>
			{
				return primitive.value;
			},
			[&](const std::string& value) -> Result<long long, Error>
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				int& err = errno;
				err = 0;

				char* endStr;
				long long ret = std::strtoll(value.data(), &endStr, 0);

				if (value.data() == endStr || err == ERANGE)
					return Err(Error::ConversionFailed);

				return ret;
			},
			[](auto) -> Result<long long, Error> { return Err(Error::WrongType); }
		}, it->second);
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

		return std::visit(Overloaded{
			[&](Primitive<void*> primitive) -> Result<void*, Error>
			{
				return primitive.value;
			},
			[&](const UserdataValue* userdata) -> Result<void*, Error>
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				return userdata->ptr.Get();
			},
			[](auto) -> Result<void*, Error> { return Err(Error::WrongType); }
		}, it->second);
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

		return std::visit(Overloaded{
			[](std::monostate) ->Result<std::string, Error>
			{
				return std::string{};
			},
			[&](Primitive<bool> primitive) -> Result<std::string, Error>
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				return std::string{ (primitive.value) ? "true" : "false" };
			},
			[&](Primitive<double> primitive) -> Result<std::string, Error>
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				return std::to_string(primitive.value);
			},
			[&](Primitive<long long> primitive) -> Result<std::string, Error>
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				return std::to_string(primitive.value);
			},
			[&](Primitive<void*> primitive) -> Result<std::string, Error>
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				return PointerToString(primitive.value);
			},
			[&](const UserdataValue* userdata) -> Result<std::string, Error>
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				return PointerToString(userdata->ptr.Get());
			},
			[&](const Color& value) -> Result<std::string, Error>
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				return value.ToString();
			},
			[&](const std::string& value) -> Result<std::string, Error>
			{
				return value;
			},
			[](auto) -> Result<std::string, Error> { return Err(Error::WrongType); }
		}, it->second);
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

		return std::visit(Overloaded{
			[](std::monostate) ->Result<std::string_view, Error>
			{
				return std::string_view{};
			},
			[&](Primitive<bool> primitive) -> Result<std::string_view, Error>
			{
				if (strict)
					return Err(Error::WouldRequireConversion);

				return std::string_view{ (primitive.value) ? "true" : "false" };
			},
			[&](const std::string& value) -> Result<std::string_view, Error>
			{
				return std::string_view(value);
			},
			[](auto) -> Result<std::string_view, Error> { return Err(Error::WrongType); }
		}, it->second);
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
		if (!std::holds_alternative<UserdataValue*>(parameter))
			return Err(Error::WrongType);

		return std::get<UserdataValue*>(parameter)->ptr.Get();
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
		CreateValue(std::move(name));
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
		parameter = value;
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
		parameter = std::move(value);
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
		parameter = std::string(value);
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
		parameter = Primitive<bool>{ value };
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
		parameter = Primitive<double>{ value };
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
		parameter = Primitive<long long>{ value };
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
		parameter = Primitive<void*>{ value };
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
			ss << it->first << ": ";

			std::visit(Overloaded{
				[&](std::monostate)
				{
					ss << "None";
				},
				[&](Primitive<bool> primitive)
				{
					ss << "Boolean(" << primitive.value << ")";
				},
				[&](Primitive<double> primitive)
				{
					ss << "Double(" << primitive.value << ")";
				},
				[&](Primitive<long long> primitive)
				{
					ss << "Integer(" << primitive.value << ")";
				},
				[&](Primitive<void*> primitive)
				{
					ss << "Pointer(" << primitive.value << ")";
				},
				[&](const UserdataValue* userdata)
				{
					ss << "Userdata(" << userdata->ptr << ")";
				},
				[&](const Color& value)
				{
					ss << "Color(" << value.ToString() << ")";
				},
				[&](const std::string& value)
				{
					ss << "String(" << value << ")";
				}
			}, it->second);

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
		parameter = new UserdataValue(destructor, value);
	}

	/*!
	* \brief Copies the content of the other parameter list to this
	* \return A reference to this
	*
	* \param list List to assign
	*/
	ParameterList& ParameterList::operator=(const ParameterList& list)
	{
		m_parameters = list.m_parameters;
		for (auto&& [parameterName, parameterValue] : m_parameters)
		{
			if (std::holds_alternative<UserdataValue*>(parameterValue))
				std::get<UserdataValue*>(parameterValue)->counter++;
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
		if (std::holds_alternative<UserdataValue*>(parameter))
		{
			UserdataValue* userdata = std::get<UserdataValue*>(parameter);
			if (--userdata->counter == 0)
			{
				userdata->destructor(userdata->ptr);
				delete userdata;
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
}
