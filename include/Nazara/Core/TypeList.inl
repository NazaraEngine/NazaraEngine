// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/TypeList.hpp>
#include <type_traits>
#include <utility>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		template<typename NewType, typename... ListTypes>
		struct ListAppend<TypeList<ListTypes...>, NewType>
		{
			using Result = TypeList<ListTypes..., NewType>;
		};


		template<template<typename> typename Functor, typename T, typename... ListTypes>
		struct ListApply<TypeList<T, ListTypes...>, Functor>
		{
			template<typename... Args>
			static void Apply(Args&&... args)
			{
				Functor<T>()(std::forward<Args>(args)...);
				if constexpr (sizeof...(ListTypes) > 0)
					ListApply<TypeList<ListTypes...>, Functor>::Apply(std::forward<Args>(args)...);
			}
		};

		template<typename T, typename... ListTypes>
		struct ListAt<TypeList<T, ListTypes...>, 0>
		{
			using Type = T;
		};

		template<std::size_t Index, typename T, typename... ListTypes>
		struct ListAt<TypeList<T, ListTypes...>, Index>
		{
			static_assert(Index <= sizeof...(ListTypes), "Index out of range");

			using Type = typename ListAt<TypeList<ListTypes...>, Index - 1>::Type;
		};


		template<typename... First, typename... Second>
		struct ListConcat<TypeList<First...>, TypeList<Second...>>
		{
			using Result = TypeList<First..., Second...>;
		};


		struct ListFindHelper
		{
			template<typename ToFind, typename Type, typename... Rest> static constexpr bool Find()
			{
				if constexpr (std::is_same_v<ToFind, Type>)
					return true;
				else
					return Find<ToFind, Rest...>();
			}

			template<typename ToFind> static constexpr bool Find()
			{
				return false;
			}
		};

		template<typename TypeToFind, typename... ListTypes>
		struct ListFind<TypeList<ListTypes...>, TypeToFind>
		{
			static constexpr bool Find()
			{
				return ListFindHelper::Find<TypeToFind, ListTypes...>();
			}
		};


		template<template<typename...> typename Class, typename... ListTypes>
		struct ListInstantiate<TypeList<ListTypes...>, Class>
		{
			using Result = Class<ListTypes...>;
		};


		template<typename NewType, typename... ListTypes>
		struct ListPrepend<TypeList<ListTypes...>, NewType>
		{
			using Result = TypeList<NewType, ListTypes...>;
		};


		template<typename... ListTypes>
		struct ListSize<TypeList<ListTypes...>>
		{
			static constexpr std::size_t Size = sizeof...(ListTypes);
		};


		template<typename... Types, typename T1>
		struct ListUnique<TypeList<Types...>, TypeList<T1>>
		{
			static constexpr bool IsTypePresent = ListFind<TypeList<Types...>, T1>::Find();
			using Result = std::conditional_t<!IsTypePresent, TypeList<Types..., T1>, TypeList<Types...>>;
		};

		template<typename... Types, typename T1, typename T2, typename... Rest>
		struct ListUnique<TypeList<Types...>, TypeList<T1, T2, Rest...>>
		{
			using Result = typename ListUnique<typename ListUnique<TypeList<Types...>, TypeList<T1>>::Result, TypeList<T2, Rest...>>::Result;
		};
	}

	template<typename List, template<typename> typename Functor, typename... Args>
	void TypeListApply(Args&&... args)
	{
		if constexpr (!TypeListEmpty<List>)
			Detail::ListApply<List, Functor>::Apply(std::forward<Args>(args)...);
	}
}

#include <Nazara/Core/DebugOff.hpp>
