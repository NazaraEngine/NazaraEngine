// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOUND_HPP
#define NAZARA_SOUND_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Resource.hpp>

class NAZARA_API NzSound
{
	public:
		NzSound();
		~NzSound();

		bool LoadFromFile(const NzString& filePath);
		bool LoadFromMemory(const nzUInt8* ptr, std::size_t size);

	private:

};

#endif // NAZARA_SOUND_HPP
