// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Flags.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::Flags
	* \brief Core class that represents a set of bits
	*
	* This class meets the requirements of Container, AllocatorAwareContainer, SequenceContainer
	*/

	template<typename E>
	constexpr Flags<E>::Flags(UInt32 value) :
	m_value(value)
	{
	}

	template<typename E>
	constexpr Flags<E>::Flags(E enumVal) :
	Flags(GetFlagValue(enumVal))
	{
	}

	template<typename E>
	constexpr Flags<E>::operator bool() const
	{
		return m_value != 0;
	}

	template<typename E>
	constexpr Flags<E> Flags<E>::operator~() const
	{
		return Flags(~m_value);
	}

	template<typename E>
	constexpr Flags<E> Flags<E>::operator&(Flags rhs) const
	{
		return Flags(m_value & rhs.m_value);
	}

	template<typename E>
	constexpr Flags<E> Flags<E>::operator|(Flags rhs) const
	{
		return Flags(m_value | rhs.m_value);
	}

	template<typename E>
	constexpr Flags<E> Flags<E>::operator^(Flags rhs) const
	{
		return Flags(m_value ^ rhs.m_value);
	}

	template<typename E>
	constexpr bool Flags<E>::operator==(Flags rhs) const
	{
		return m_value == rhs.m_value;
	}

	template<typename E>
	constexpr bool Flags<E>::operator!=(Flags rhs) const
	{
		return !operator==(rhs);
	}

	template<typename E>
	/*constexpr*/ Flags<E>& Flags<E>::operator|=(Flags rhs)
	{
		m_value |= rhs.m_value;

		return *this;
	}

	template<typename E>
	/*constexpr*/ Flags<E>& Flags<E>::operator&=(Flags rhs)
	{
		m_value &= rhs.m_value;

		return *this;
	}

	template<typename E>
	/*constexpr*/ Flags<E>& Flags<E>::operator^=(Flags rhs)
	{
		m_value ^= rhs.m_value;

		return *this;
	}

	template<typename E>
	constexpr UInt32 Flags<E>::GetFlagValue(E enumValue)
	{
		return 1U << static_cast<UInt32>(enumValue);
	}


	template<typename E>
	constexpr std::enable_if_t<EnableFlagsOperators<E>::value, Flags<E>> operator~(E lhs)
	{
		return ~Flags<E>(lhs);
	}

	template<typename E>
	constexpr std::enable_if_t<EnableFlagsOperators<E>::value, Flags<E>> operator|(E lhs, E rhs)
	{
		return Flags<E>(lhs) | rhs;
	}

	template<typename E>
	constexpr std::enable_if_t<EnableFlagsOperators<E>::value, Flags<E>> operator&(E lhs, E rhs)
	{
		return Flags<E>(lhs) & rhs;
	}

	template<typename E>
	constexpr std::enable_if_t<EnableFlagsOperators<E>::value, Flags<E>> operator^(E lhs, E rhs)
	{
		return Flags<E>(lhs) ^ rhs;
	}
}

#include <Nazara/Core/DebugOff.hpp>
