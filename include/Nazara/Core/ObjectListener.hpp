// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OBJECTLISTENER_HPP
#define NAZARA_OBJECTLISTENER_HPP

#include <Nazara/Prerequesites.hpp>

class NzRefCounted;

class NAZARA_API NzObjectListener
{
	public:
		NzObjectListener() = default;
		virtual ~NzObjectListener();

		virtual bool OnObjectCreated(const NzRefCounted* object, int index);
		virtual bool OnObjectDestroy(const NzRefCounted* object, int index);
		virtual bool OnObjectModified(const NzRefCounted* object, int index, unsigned int code);
		virtual void OnObjectReleased(const NzRefCounted* object, int index);
};

#endif // NAZARA_OBJECTLISTENER_HPP
