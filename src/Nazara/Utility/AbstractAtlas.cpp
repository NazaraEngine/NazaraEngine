// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/AbstractAtlas.hpp>
#include <Nazara/Utility/Debug.hpp>

NzAbstractAtlas::~NzAbstractAtlas()
{
	OnAtlasRelease(this);
}
