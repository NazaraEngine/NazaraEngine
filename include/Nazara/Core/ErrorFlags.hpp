// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ERRORFLAGS_HPP
#define NAZARA_ERRORFLAGS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>

class NAZARA_CORE_API NzErrorFlags
{
	public:
		NzErrorFlags(nzUInt32 flags, bool replace = false);
        NzErrorFlags(const NzErrorFlags&) = delete;
        NzErrorFlags(NzErrorFlags&&) = delete;
		~NzErrorFlags();

		nzUInt32 GetPreviousFlags() const;

		void SetFlags(nzUInt32 flags, bool replace = false);

        NzErrorFlags& operator=(const NzErrorFlags&) = delete;
        NzErrorFlags& operator=(NzErrorFlags&&) = delete;

	private:
		nzUInt32 m_previousFlags;
};

#endif // NAZARA_ERRORFLAGS_HPP
