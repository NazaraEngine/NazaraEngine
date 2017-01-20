// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OFFSETOF_HPP
#define NAZARA_OFFSETOF_HPP

// By "Jesse Good" from SO:
// http://stackoverflow.com/questions/12811330/c-compile-time-offsetof-inside-a-template?answertab=votes#tab-top

namespace Nz
{
	namespace Detail
	{
		template <typename T, typename M> T GetClassType(M T::*);
		template <typename T, typename M> M GetMemberType(M T::*);

		template <typename T, typename R, R T::*M>
		constexpr std::size_t OffsetOf()
		{
			return reinterpret_cast<std::size_t>(&((static_cast<T*>(0))->*M));
		}
	}
}

#define NazaraOffsetOf(type, member) Nz::Detail::OffsetOf<decltype(Nz::Detail::GetClassType(&type::member)), decltype(Nz::Detail::GetMemberType(&type::member)), &type::member>()

#endif // NAZARA_OFFSETOF_HPP
