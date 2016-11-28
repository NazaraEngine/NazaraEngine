// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FLAGS_HPP
#define NAZARA_FLAGS_HPP

#include <Nazara/Prerequesites.hpp>
#include <type_traits>

namespace Nz
{
	template<typename E>
	class Flags
	{
		static_assert(std::is_enum<E>::value, "Type must be an enumeration");

		public:
			constexpr Flags(UInt32 value);
			constexpr Flags(E enumVal);

			explicit constexpr operator bool() const;
			explicit constexpr operator UInt32() const;

			constexpr Flags operator~() const;
			constexpr Flags operator&(Flags rhs) const;
			constexpr Flags operator|(Flags rhs) const;
			constexpr Flags operator^(Flags rhs) const;

			constexpr bool operator==(Flags rhs) const;
			constexpr bool operator!=(Flags rhs) const;

			/*constexpr*/ Flags& operator|=(Flags rhs);
			/*constexpr*/ Flags& operator&=(Flags rhs);
			/*constexpr*/ Flags& operator^=(Flags rhs);

			static constexpr UInt32 GetFlagValue(E enumValue);

		private:
			UInt32 m_value;
	};

	// From: https://www.justsoftwaresolutions.co.uk/cplusplus/using-enum-classes-as-bitfields.html
	template<typename E>
	struct EnableFlagsOperators
	{
		static constexpr bool value = false;
	};

	template<typename E> constexpr std::enable_if_t<EnableFlagsOperators<E>::value, Flags<E>> operator~(E lhs);
	template<typename E> constexpr std::enable_if_t<EnableFlagsOperators<E>::value, Flags<E>> operator|(E lhs, E rhs);
	template<typename E> constexpr std::enable_if_t<EnableFlagsOperators<E>::value, Flags<E>> operator&(E lhs, E rhs);
	template<typename E> constexpr std::enable_if_t<EnableFlagsOperators<E>::value, Flags<E>> operator^(E lhs, E rhs);
}

#include <Nazara/Core/Flags.inl>

#endif // NAZARA_FLAGS_HPP
