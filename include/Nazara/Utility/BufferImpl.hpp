// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BUFFERIMPL_HPP
#define NAZARA_BUFFERIMPL_HPP

#include <Nazara/Utility/Buffer.hpp>

class NAZARA_API NzBufferImpl
{
	public:
		NzBufferImpl() = default;
		virtual ~NzBufferImpl();

		virtual bool Create(unsigned int size, nzBufferUsage usage = nzBufferUsage_Static) = 0;
		virtual void Destroy() = 0;

		virtual bool Fill(const void* data, unsigned int offset, unsigned int size, bool forceDiscard = false) = 0;

		virtual void* GetPointer() = 0;

		virtual bool IsHardware() const = 0;

		virtual void* Map(nzBufferAccess access, unsigned int offset = 0, unsigned int size = 0) = 0;
		virtual bool Unmap() = 0;
};

#endif // NAZARA_BUFFERIMPL_INCLUDED
