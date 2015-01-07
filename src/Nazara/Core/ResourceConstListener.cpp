// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ResourceConstListener.hpp>
#include <Nazara/Core/Debug.hpp>

NzResourceConstListener::~NzResourceConstListener() = default;

bool NzResourceConstListener::OnResourceCreated(const NzResource* resource, int index) const
{
	NazaraUnused(resource);
	NazaraUnused(index);

	return true;
}

bool NzResourceConstListener::OnResourceDestroy(const NzResource* resource, int index) const
{
	NazaraUnused(resource);
	NazaraUnused(index);

	return true;
}

bool NzResourceConstListener::OnResourceModified(const NzResource* resource, int index, unsigned int code) const
{
	NazaraUnused(resource);
	NazaraUnused(index);
	NazaraUnused(code);

	return true;
}

void NzResourceConstListener::OnResourceReleased(const NzResource* resource, int index) const
{
	NazaraUnused(resource);
	NazaraUnused(index);
}
