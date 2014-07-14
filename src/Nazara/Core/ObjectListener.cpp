// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ObjectListener.hpp>
#include <Nazara/Core/Debug.hpp>

NzObjectListener::~NzObjectListener() = default;

bool NzObjectListener::OnObjectCreated(const NzRefCounted* object, int index)
{
	NazaraUnused(object);
	NazaraUnused(index);

	return true;
}

bool NzObjectListener::OnObjectDestroy(const NzRefCounted* object, int index)
{
	NazaraUnused(object);
	NazaraUnused(index);

	return true;
}

bool NzObjectListener::OnObjectModified(const NzRefCounted* object, int index, unsigned int code)
{
	NazaraUnused(object);
	NazaraUnused(index);
	NazaraUnused(code);

	return true;
}

void NzObjectListener::OnObjectReleased(const NzRefCounted* object, int index)
{
	NazaraUnused(object);
	NazaraUnused(index);
}
