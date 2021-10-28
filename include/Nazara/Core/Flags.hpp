// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_FLAGS_HPP
#define NAZARA_CORE_FLAGS_HPP

#include <Nazara/Prerequisites.hpp>
#include <type_traits>

namespace Nz
{
	// From: https://www.justsoftwaresolutions.co.uk/cplusplus/using-enum-classes-as-bitfields.html
	template<typename E>
	struct EnumAsFlags
	{
	};


	template<typename, typename = void>
	struct IsEnumFlag : std::false_type {};

	template<typename T>
	struct IsEnumFlag<T, std::void_t<decltype(EnumAsFlags<T>::max)>> : std::true_type {};


	template<typename, typename = void>
	struct GetEnumAutoFlag : std::integral_constant<bool, true> {};

	template<typename T>
	struct GetEnumAutoFlag<T, std::void_t<decltype(T::AutoFlag)>> : std::integral_constant<bool, T::AutoFlag> {};

	template<typename E>
	class Flags
	{
		static_assert(std::is_enum_v<E>, "Type must be an enumeration");
		static_assert(IsEnumFlag<E>(), "Enum has not been enabled as flags by an EnumAsFlags specialization");
		static_assert(std::is_same_v<std::remove_cv_t<decltype(EnumAsFlags<E>::max)>, E>, "EnumAsFlags field max should be of the same type as the enum");

		static constexpr std::size_t MaxValue = static_cast<std::size_t>(EnumAsFlags<E>::max);
		static constexpr bool AutoFlag = GetEnumAutoFlag<E>();

		using BitField16 = std::conditional_t<(MaxValue >= 8), UInt16, UInt8>;
		using BitField32 = std::conditional_t<(MaxValue >= 16), UInt32, BitField16>;

		public:
			using BitField = std::conditional_t<(MaxValue >= 32), UInt64, BitField32>;

			constexpr Flags(BitField value = 0);
			constexpr Flags(E enumVal);

			void Clear();
			void Clear(const Flags& flags);

			void Set(const Flags& flags);

			constexpr bool Test(const Flags& flags) const;

			explicit constexpr operator bool() const;
			template<typename T, typename = std::enable_if_t<std::is_integral<T>::value && sizeof(T) >= sizeof(BitField)>> explicit constexpr operator T() const;

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

			static constexpr BitField ValueMask = BitField((UInt64(1) << (MaxValue + 1)) - 1);

		private:
			BitField m_value;
	};

	template<typename E> constexpr Flags<E> operator&(E lhs, Flags<E> rhs);
	template<typename E> constexpr Flags<E> operator|(E lhs, Flags<E> rhs);
	template<typename E> constexpr Flags<E> operator^(E lhs, Flags<E> rhs);

	// Little hack to have them in both Nz and global scope
	namespace FlagsOperators
	{
		template<typename E> constexpr std::enable_if_t<IsEnumFlag<E>::value, Flags<E>> operator~(E lhs);
		template<typename E> constexpr std::enable_if_t<IsEnumFlag<E>::value, Flags<E>> operator&(E lhs, E rhs);
		template<typename E> constexpr std::enable_if_t<IsEnumFlag<E>::value, Flags<E>> operator|(E lhs, E rhs);
		template<typename E> constexpr std::enable_if_t<IsEnumFlag<E>::value, Flags<E>> operator^(E lhs, E rhs);
	}

	using namespace FlagsOperators;
}

using namespace Nz::FlagsOperators;

#include <Nazara/Core/Flags.inl>

#endif // NAZARA_CORE_FLAGS_HPP
