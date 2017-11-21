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
	};

	// From: https://stackoverflow.com/questions/11927032/sfinae-check-for-static-member-using-decltype
	template <typename T>
	class IsEnumFlag
	{
		template<typename U, typename = typename std::enable_if<!std::is_member_pointer<decltype(&EnumAsFlags<U>::max)>::value>::type>
		static std::true_type check(int);

		template <typename> static std::false_type check(...);

		public:
			static constexpr bool value = decltype(check<T>(0))::value;
	};

	template<typename E>
	class Flags
	{
		static_assert(std::is_enum<E>::value, "Type must be an enumeration");
		static_assert(IsEnumFlag<E>::value, "Enum has not been enabled as flags by an EnumAsFlags specialization");

		static constexpr std::size_t MaxValue = static_cast<std::size_t>(EnumAsFlags<E>::max);

		using BitField16 = typename std::conditional<(MaxValue > 8), UInt16, UInt8>::type;
		using BitField32 = typename std::conditional<(MaxValue > 16), UInt32, BitField16>::type;

		public:
			using BitField = typename std::conditional<(MaxValue > 32), UInt64, BitField32>::type;

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

			static constexpr BitField ValueMask = ((BitField(1) << (MaxValue + 1)) - 1);

		private:
			BitField m_value;
	};

	// Little hack to have them in both Nz and global scope
	namespace FlagsOperators
	{
		template<typename E> constexpr std::enable_if_t<IsEnumFlag<E>::value, Flags<E>> operator~(E lhs);
		template<typename E> constexpr std::enable_if_t<IsEnumFlag<E>::value, Flags<E>> operator|(E lhs, E rhs);
		template<typename E> constexpr std::enable_if_t<IsEnumFlag<E>::value, Flags<E>> operator&(E lhs, E rhs);
		template<typename E> constexpr std::enable_if_t<IsEnumFlag<E>::value, Flags<E>> operator^(E lhs, E rhs);
	}

	using namespace FlagsOperators;
}

using namespace Nz::FlagsOperators;

#include <Nazara/Core/Flags.inl>

#endif // NAZARA_FLAGS_HPP
