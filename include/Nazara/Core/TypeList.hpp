// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_TYPELIST_HPP
#define NAZARA_CORE_TYPELIST_HPP

#include <cstddef>

namespace Nz
{
	template<typename...> struct TypeList {};

	namespace Detail
	{
		template<typename, typename>
		struct ListAppend;

		template<typename, template<typename> typename>
		struct ListApply;

		template<typename, std::size_t>
		struct ListAt;

		template<typename, typename>
		struct ListConcat;

		template<typename, typename>
		struct ListFind;

		template<typename, template<typename...> typename>
		struct ListInstantiate;

		template<typename, typename>
		struct ListPrepend;

		template<typename>
		struct ListSize;

		template<typename, typename>
		struct ListUnique;
	}

	template<typename List, typename NewType>
	using TypeListAppend = typename Detail::ListAppend<List, NewType>::Result;

	template<typename List, template<typename> typename Functor, typename... Args>
	void TypeListApply(Args&&... args);

	template<typename List, std::size_t Index>
	using TypeListAt = typename Detail::ListAt<List, Index>::Type;

	template<typename FirstList, typename SecondList>
	using TypeListConcat = typename Detail::ListConcat<FirstList, SecondList>::Result;

	template<typename List>
	constexpr bool TypeListEmpty = Detail::ListSize<List>::Size == 0;

	template<typename List, typename Type>
	constexpr bool TypeListFind = Detail::ListFind<List, Type>::Find();

	template<typename List, template<typename...> typename Class>
	using TypeListInstantiate = typename Detail::ListInstantiate<List, Class>::Result;

	template<typename List, typename NewType>
	using TypeListPrepend = typename Detail::ListPrepend<List, NewType>::Result;

	template<typename List>
	constexpr std::size_t TypeListSize = Detail::ListSize<List>::Size;

	template<typename List>
	using TypeListUnique = typename Detail::ListUnique<TypeList<>, List>::Result;
}

#include <Nazara/Core/TypeList.inl>

#endif // NAZARA_CORE_TYPELIST_HPP
