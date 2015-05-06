// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/AbstractAtlas.hpp>
#include <Nazara/Utility/Debug.hpp>

NzAbstractAtlas::~NzAbstractAtlas()
{
	NotifyRelease(Listener::OnAtlasReleased);
}

NzAbstractAtlas::Listener::~Listener() = default;

bool NzAbstractAtlas::Listener::OnAtlasCleared(const NzAbstractAtlas* atlas, void* userdata)
{
	NazaraUnused(atlas);
	NazaraUnused(userdata);

	return true;
}

bool NzAbstractAtlas::Listener::OnAtlasLayerChange(const NzAbstractAtlas* atlas, NzAbstractImage* oldLayer, NzAbstractImage* newLayer, void* userdata)
{
	NazaraUnused(atlas);
	NazaraUnused(oldLayer);
	NazaraUnused(newLayer);
	NazaraUnused(userdata);

	return true;
}

void NzAbstractAtlas::Listener::OnAtlasReleased(const NzAbstractAtlas* atlas, void* userdata)
{
	NazaraUnused(atlas);
	NazaraUnused(userdata);
}
