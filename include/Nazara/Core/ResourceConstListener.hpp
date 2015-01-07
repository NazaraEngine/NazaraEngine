// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCECONSTLISTENER_HPP
#define NAZARA_RESOURCECONSTLISTENER_HPP

#include <Nazara/Prerequesites.hpp>

class NzResource;

class NAZARA_API NzResourceConstListener
{
	public:
		NzResourceConstListener() = default;
		virtual ~NzResourceConstListener();

		virtual bool OnResourceCreated(const NzResource* resource, int index) const;
		virtual bool OnResourceDestroy(const NzResource* resource, int index) const;
		virtual bool OnResourceModified(const NzResource* resource, int index, unsigned int code) const;
		virtual void OnResourceReleased(const NzResource* resource, int index) const;
};

#endif // NAZARA_RESOURCECONSTLISTENER_HPP
