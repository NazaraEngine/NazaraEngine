// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCELISTENER_HPP
#define NAZARA_RESOURCELISTENER_HPP

#include <Nazara/Prerequesites.hpp>

class NzResource;

class NAZARA_API NzResourceListener
{
	public:
		NzResourceListener() = default;
		virtual ~NzResourceListener();

		virtual bool OnResourceCreated(const NzResource* resource, int index);
		virtual bool OnResourceDestroy(const NzResource* resource, int index);
		virtual bool OnResourceModified(const NzResource* resource, int index, unsigned int code);
		virtual void OnResourceReleased(const NzResource* resource, int index);
};

#endif // NAZARA_RESOURCELISTENER_HPP
