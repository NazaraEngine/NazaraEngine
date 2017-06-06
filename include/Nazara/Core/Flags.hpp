// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FLAGS_HPP
#define NAZARA_FLAGS_HPP

#include <Nazara/Prerequesites.hpp>
#include <type_traits>

namespace Nz
{
	// From: https://www.justsoftwaresolutions.co.uk/cplusplus/using-enum-classes-as-bitfields.html
	template<typename E>
	struct EnumAsFlags
	{
		static constexpr bool value = false;
		static constexpr int  max   = 0;
	};

	template<typename E>
	class Flags
	{
		static_assert(std::is_enum<E>::value, "Type must be an enumeration");
		static_assert(EnumAsFlags<E>::value, "Enum has not been enabled as flags by an EnumAsFlags specialization");

		public:
			using BitField = typename std::conditional<(EnumAsFlags<E>::max > 32), UInt64, UInt32>::type;

			constexpr Flags(BitField value = 0);
			constexpr Flags(E enumVal);

			explicit constexpr operator bool() const;
			explicit constexpr operator BitField() const;

			constexpr Flags operator~() const;
			constexpr Flags operator&(const Flags& rhs) const;
			constexpr Flags operator|(const Flags& rhs) const;
			constexpr Flags operator^(const Flags& rhs) const;

			constexpr bool operator==(const Flags& rhs) const;
			constexpr bool operator!=(const Flags& rhs) const;

			/*constexpr*/ Flags& operator|=(const Flags& rhs);
			/*constexpr*/ Flags& operator&=(const Flags& rhs);
			/*constexpr*/ Flags& operator^=(const Flags& rhs);

			static constexpr BitField GetFlagValue(E enumValue);

			static constexpr BitField ValueMask = ((BitField(1) << (EnumAsFlags<E>::max + 1)) - 1);

		private:
			BitField m_value;
	};

	template<typename E> constexpr std::enable_if_t<EnumAsFlags<E>::value, Flags<E>> operator~(E lhs);
	template<typename E> constexpr std::enable_if_t<EnumAsFlags<E>::value, Flags<E>> operator|(E lhs, E rhs);
	template<typename E> constexpr std::enable_if_t<EnumAsFlags<E>::value, Flags<E>> operator&(E lhs, E rhs);
	template<typename E> constexpr std::enable_if_t<EnumAsFlags<E>::value, Flags<E>> operator^(E lhs, E rhs);
}

template<typename E> constexpr std::enable_if_t<Nz::EnumAsFlags<E>::value, Nz::Flags<E>> operator~(E lhs);
template<typename E> constexpr std::enable_if_t<Nz::EnumAsFlags<E>::value, Nz::Flags<E>> operator|(E lhs, E rhs);
template<typename E> constexpr std::enable_if_t<Nz::EnumAsFlags<E>::value, Nz::Flags<E>> operator&(E lhs, E rhs);
template<typename E> constexpr std::enable_if_t<Nz::EnumAsFlags<E>::value, Nz::Flags<E>> operator^(E lhs, E rhs);

#include <Nazara/Core/Flags.inl>

#endif // NAZARA_FLAGS_HPP
