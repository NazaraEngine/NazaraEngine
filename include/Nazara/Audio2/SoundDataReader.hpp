// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO2_SOUNDDATAREADER_HPP
#define NAZARA_AUDIO2_SOUNDDATAREADER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio2/Export.hpp>

namespace Nz
{
	class SoundDataSource;

	class NAZARA_AUDIO2_API SoundDataReader
	{
		public:
			inline SoundDataReader(SoundDataSource* source);
			SoundDataReader(const SoundDataReader&) = delete;
			SoundDataReader(SoundDataReader&&) = delete;
			~SoundDataReader() = default;

			virtual void EnableLooping(bool looping) = 0;

			inline UInt64 GetReadOffset() const;
			inline SoundDataSource* GetSource();

			inline void UpdateReadOffset(UInt64 offset);

			SoundDataReader& operator=(const SoundDataReader&) = delete;
			SoundDataReader& operator=(SoundDataReader&&) = delete;

		private:
			SoundDataSource* m_source;
			UInt64 m_readOffset;
	};
}

#include <Nazara/Audio2/SoundDataReader.inl>

#endif // NAZARA_AUDIO2_SOUNDDATAREADER_HPP
