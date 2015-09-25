// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_STREAM_HPP
#define NAZARA_STREAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/String.hpp>

namespace Nz
{
	class NAZARA_CORE_API Stream
	{
		public:
			Stream() = default;
			Stream(const Stream&) = default;
			Stream(Stream&&) = default;
			virtual ~Stream();

			virtual UInt64 GetCursorPos() const = 0;
			virtual String GetDirectory() const;
			virtual String GetPath() const;
			unsigned int GetStreamOptions() const;

			virtual bool SetCursorPos(UInt64 offset) = 0;
			void SetStreamOptions(unsigned int options);

			Stream& operator=(const Stream&) = default;
			Stream& operator=(Stream&&) = default;

		protected:
			unsigned int m_streamOptions = 0;
	};
}

#endif // NAZARA_STREAM_HPP
