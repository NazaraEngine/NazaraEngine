// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OFFSETOF_HPP
#define NAZARA_OFFSETOF_HPP

// Par "Jesse Good" de SO:
// http://stackoverflow.com/questions/12811330/c-compile-time-offsetof-inside-a-template?answertab=votes#tab-top

template <typename T, typename M> T NzImplGetClassType(M T::*);
template <typename T, typename M> M NzImplGetMemberType(M T::*);

template <typename T, typename R, R T::*M>
constexpr std::size_t NzImplOffsetOf()
{
	return reinterpret_cast<std::size_t>(&(((T*)0)->*M));
}

#define NzOffsetOf(type, member) NzImplOffsetOf<decltype(NzImplGetClassType(&type::member)), decltype(NzImplGetMemberType(&type::member)), &type::member>()

#endif // NAZARA_OFFSETOF_HPP
