// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TYPELIST_HPP
#define NAZARA_TYPELIST_HPP

#include <cstddef>

namespace Nz
{
	template<typename...> struct TypeList {};

	namespace Detail
	{
		template<typename, typename>
		struct ListAppend;

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

		template<typename, typename>
		struct ListUnique;
	}

	template<typename List, typename NewType>
	using TypeListAppend = typename Detail::ListAppend<List, NewType>::Result;

	template<typename List, std::size_t Index>
	using TypeListAt = typename Detail::ListAt<List, Index>::Type;

	template<typename FirstList, typename SecondList>
	using TypeListConcat = typename Detail::ListConcat<FirstList, SecondList>::Result;

	template<typename List, typename Type>
	constexpr bool TypeListFind = Detail::ListFind<List, Type>::Find();

	template<typename List, template<typename...> typename Class>
	using TypeListInstantiate = typename Detail::ListInstantiate<List, Class>::Result;

	template<typename List, typename NewType>
	using TypeListPrepend = typename Detail::ListPrepend<List, NewType>::Result;

	template<typename List>
	using TypeListUnique = typename Detail::ListUnique<TypeList<>, List>::Result;
}

#include <Nazara/Core/TypeList.inl>

#endif // NAZARA_TYPELIST_HPP
