// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "Patch.hpp"
//#include <Nazara/DynaTerrain/NoiseBase.hpp>
//#include <Nazara/DynaTerrain/Error.hpp>
//#include <Nazara/DynaTerrain/Config.hpp>
//#include <Nazara/DynaTerrain/Debug.hpp>



NzPatch::NzPatch(NzVector2f center, NzVector2f size)
{
    m_center = center;
    m_size = size;
}

NzPatch::~NzPatch()
{
    //dtor
}

NzVector2f NzPatch::GetCenter() const
{

}

NzVector2f NzPatch::GetSize() const
{

}

bool NzPatch::IntersectsCircle(const NzVector2f& center, double radius)
{

}

bool NzPatch::IsContainedByCircle(const NzVector2f& center, double radius)
{

}

NzPatch* NzPatch::LocatePatch(const NzVector2f& position)
{

}
