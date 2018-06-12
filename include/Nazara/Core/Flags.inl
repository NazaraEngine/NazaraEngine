// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Flags.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::Flags
	* \brief Core class used to combine enumeration values into flags bitfield
	*/

	/*!
	* \brief Constructs a Flags object using a bitfield
	*
	* \param value Bitfield to be used
	*
	* Uses a bitfield to builds the flag value. (e.g. if bit 0 is active, then Enum value 0 will be set as active).
	*/
	template<typename E>
	constexpr Flags<E>::Flags(BitField value) :
	m_value(value)
	{
	}

	/*!
	* \brief Constructs a Flags object using an Enum value
	*
	* \param enumVal enumVal
	*
	* Setup a Flags object with only one flag active (corresponding to the enum value passed as argument).
	*/
	template<typename E>
	constexpr Flags<E>::Flags(E enumVal) :
	Flags(GetFlagValue(enumVal))
	{
	}

	/*!
	* \brief Tests if all flags from a Flags object are enabled
	* \return True if all tested flags are enabled.
	*/
	template<typename E>
	constexpr bool Flags<E>::Test(const Flags& flags) const
	{
		return (m_value & flags.m_value) == flags.m_value;
	}

	/*!
	* \brief Tests any flag
	* \return True if any flag is enabled.
	*
	* This will convert to a boolean value allowing to check if any flag is set.
	*/
	template<typename E>
	constexpr Flags<E>::operator bool() const
	{
		return m_value != 0;
	}

	/*!
	* \brief Converts to an integer
	* \return Enabled flags as a integer
	*
	* This will only works if the integer type is large enough to store all flags states
	*/
	template<typename E>
	template<typename T, typename>
	constexpr Flags<E>::operator T() const
	{
		return m_value;
	}

	/*!
	* \brief Reverse flag states
	* \return Opposite enabled flags
	*
	* This will returns a copy of the Flags object with reversed flags states.
	*/
	template<typename E>
	constexpr Flags<E> Flags<E>::operator~() const
	{
		return Flags((~m_value) & ValueMask);
	}

	/*!
	* \brief Compare flag states
	* \return Shared flags
	*
	* \param rhs Flags to compare with.
	*
	* This will returns a copy of the Flags object with only enabled flags in common with the parameter
	*/
	template<typename E>
	constexpr Flags<E> Flags<E>::operator&(const Flags& rhs) const
	{
		return Flags(m_value & rhs.m_value);
	}

	/*!
	* \brief Combine flag states
	* \return Combined flags
	*
	* This will returns a copy of the Flags object with combined flags from the parameter.
	*
	* \param rhs Flags to combine with.
	*/
	template<typename E>
	constexpr Flags<E> Flags<E>::operator|(const Flags& rhs) const
	{
		return Flags(m_value | rhs.m_value);
	}

	/*!
	* \brief XOR flag states
	* \return XORed flags.
	*
	* \param rhs Flags to XOR with.
	*
	* This performs a XOR (Exclusive OR) on a copy of the flag object.
	* This will returns a copy of the object with disabled common flags and enabled unique ones.
	*/
	template<typename E>
	constexpr Flags<E> Flags<E>::operator^(const Flags& rhs) const
	{
		return Flags((m_value ^ rhs.m_value) & ValueMask);
	}

	/*!
	* \brief Check equality with flag object
	* \return True if both flags objects have the same states.
	*
	* \param rhs Flags to compare with.
	*
	* Compare two Flags object and returns true if the flag states are identical.
	*/
	template<typename E>
	constexpr bool Flags<E>::operator==(const Flags& rhs) const
	{
		return m_value == rhs.m_value;
	}

	/*!
	* \brief Check inequality with flag object
	* \return True if both flags objects have different states.
	*
	* \param rhs Flags to compare with.
	*
	* Compare two Flags object and returns true if the flag states are identical.
	*/
	template<typename E>
	constexpr bool Flags<E>::operator!=(const Flags& rhs) const
	{
		return !operator==(rhs);
	}

	/*!
	* \brief Combine flag states
	* \return A reference to the object.
	*
	* \param rhs Flags to combine with.
	*
	* This will enable flags which are enabled in parameter object and not in Flag object.
	*/
	template<typename E>
	/*constexpr*/ Flags<E>& Flags<E>::operator|=(const Flags& rhs)
	{
		m_value |= rhs.m_value;

		return *this;
	}

	/*!
	* \brief Compare flag states
	* \return A reference to the object.
	*
	* \param rhs Flags to compare with.
	*
	* This will disable flags which are disabled in parameter object and enabled in Flag object (and vice-versa).
	*/
	template<typename E>
	/*constexpr*/ Flags<E>& Flags<E>::operator&=(const Flags& rhs)
	{
		m_value &= rhs.m_value;

		return *this;
	}

	/*!
	* \brief XOR flag states
	* \return A reference to the object.
	*
	* \param rhs Flags to XOR with.
	*
	* This performs a XOR (Exclusive OR) on the flag object.
	* This will disable flags enabled in both Flags objects and enable those enabled in only one of the Flags objects.
	*/
	template<typename E>
	/*constexpr*/ Flags<E>& Flags<E>::operator^=(const Flags& rhs)
	{
		m_value ^= rhs.m_value;
		m_value &= ValueMask;

		return *this;
	}

	/*!
	* \brief Returns a bitfield corresponding to an enum value.
	* \return Bitfield representation of the enum value
	*
	* \param enumValue Enumeration value to get as a bitfield.
	*
	* Internally, every enum option is turned into a bit, this function allows to get a bitfield with only the bit of the enumeration value enabled.
	*/
	template<typename E>
	constexpr typename Flags<E>::BitField Flags<E>::GetFlagValue(E enumValue)
	{
		return 1U << static_cast<BitField>(enumValue);
	}

	/*!
	* \brief Compare flag states
	* \return Compared flags
	*
	* This will returns a copy of the Flags object compared with the enum state.
	*
	* \param lhs Enum to compare with flags.
	* \param rhs Flags object.
	*/
	template<typename E>
	constexpr Flags<E> operator&(E lhs, Flags<E> rhs)
	{
		return rhs & lhs;
	}

	/*!
	* \brief Combine flag states
	* \return Combined flags
	*
	* This will returns a copy of the Flags object combined with the enum state.
	*
	* \param lhs Enum to combine with flags.
	* \param rhs Flags object.
	*/
	template<typename E>
	constexpr Flags<E> operator|(E lhs, Flags<E> rhs)
	{
		return rhs | lhs;
	}

	/*!
	* \brief XOR flag states
	* \return XORed flags
	*
	* This will returns a copy of the Flags object XORed with the enum state.
	*
	* \param lhs Enum to XOR with flags.
	* \param rhs Flags object.
	*/
	template<typename E>
	constexpr Flags<E> operator^(E lhs, Flags<E> rhs)
	{
		return rhs ^ lhs;
	}


	namespace FlagsOperators
	{
		/*!
		* \brief Override binary NOT operator on enum to turns into a Flags object.
		* \return A Flags object with reversed bits.
		*
		* \param lhs Enumeration value to reverse.
		*
		* Returns a Flags object with all state enabled except for the enum one.
		*/
		template<typename E>
		constexpr std::enable_if_t<IsEnumFlag<E>::value, Flags<E>> operator~(E lhs)
		{
			return ~Flags<E>(lhs);
		}

		/*!
		* \brief Override binary AND operator on enum to turns into a Flags object.
		* \return A Flags object with compare enum states.
		*
		* \param lhs First enumeration value to compare.
		* \param rhs Second enumeration value to compare.
		*
		* Returns a Flags object with compared states from the two enumeration values.
		* In this case, only one flag will be enabled if both enumeration values are the same.
		*/
		template<typename E>
		constexpr std::enable_if_t<IsEnumFlag<E>::value, Flags<E>> operator&(E lhs, E rhs)
		{
			return Flags<E>(lhs) & rhs;
		}

		/*!
		* \brief Override binary OR operator on enum to turns into a Flags object.
		* \return A Flags object with combined enum states.
		*
		* \param lhs First enumeration value to combine.
		* \param rhs Second enumeration value to combine.
		*
		* Returns a Flags object with combined states from the two enumeration values.
		*/
		template<typename E>
		constexpr std::enable_if_t<IsEnumFlag<E>::value, Flags<E>> operator|(E lhs, E rhs)
		{
			return Flags<E>(lhs) | rhs;
		}

		/*!
		* \brief Override binary XOR operator on enum to turns into a Flags object.
		* \return A Flags object with XORed enum states.
		*
		* \param lhs First enumeration value to compare.
		* \param rhs Second enumeration value to compare.
		*
		* Returns a Flags object with XORed states from the two enumeration values.
		* In this case, two flags will be enabled if both the enumeration values are different.
		*/
		template<typename E>
		constexpr std::enable_if_t<IsEnumFlag<E>::value, Flags<E>> operator^(E lhs, E rhs)
		{
			return Flags<E>(lhs) ^ rhs;
		}
	}
}

#include <Nazara/Core/DebugOff.hpp>
