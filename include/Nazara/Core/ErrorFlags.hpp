// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ERRORFLAGS_HPP
#define NAZARA_ERRORFLAGS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/NonCopyable.hpp>

class NAZARA_API NzErrorFlags : NzNonCopyable
{
	public:
		NzErrorFlags(nzUInt32 flags, bool replace = false);
		~NzErrorFlags();

		nzUInt32 GetPreviousFlags() const;

		void SetFlags(nzUInt32 flags, bool replace = false);

	private:
		nzUInt32 m_previousFlags;
};

#endif // NAZARA_ERRORFLAGS_HPP
