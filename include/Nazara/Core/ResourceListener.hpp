// Copyright (C) 2012 Jérôme Leclercq
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

		virtual void OnResourceCreated(const NzResource* resource, int index);
		virtual void OnResourceDestroy(const NzResource* resource, int index);
		virtual void OnResourceReleased(const NzResource* resource, int index);
};

#endif // NAZARA_RESOURCELISTENER_HPP
