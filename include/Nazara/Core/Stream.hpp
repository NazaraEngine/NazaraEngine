// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_STREAM_HPP
#define NAZARA_STREAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>

namespace Nz
{
	class String; //< Do not include String.hpp in this file

	class NAZARA_CORE_API Stream
	{
		public:
			Stream(const Stream&) = default;
			Stream(Stream&&) = default;
			virtual ~Stream();

			virtual UInt64 GetCursorPos() const = 0;
			virtual String GetDirectory() const;
			virtual String GetPath() const;
			inline UInt32 GetOpenMode() const;
			inline UInt32 GetStreamOptions() const;

			inline bool IsReadable() const;
			inline bool IsWritable() const;

			virtual bool SetCursorPos(UInt64 offset) = 0;
			void SetStreamOptions(UInt32 options);

			Stream& operator=(const Stream&) = default;
			Stream& operator=(Stream&&) = default;

		protected:
			inline Stream(UInt32 openMode);
			
			UInt32 m_openMode;
			UInt32 m_streamOptions;
	};
}

#include <Nazara/Core/Stream.inl>

#endif // NAZARA_STREAM_HPP
