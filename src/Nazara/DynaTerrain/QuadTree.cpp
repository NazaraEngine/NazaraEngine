// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "QuadTree.hpp"

//#include <Nazara/DynaTerrain/NoiseBase.hpp>
//#include <Nazara/DynaTerrain/Error.hpp>
//#include <Nazara/DynaTerrain/Config.hpp>
//#include <Nazara/DynaTerrain/Debug.hpp>

NzQuadTree::NzQuadTree(const NzVector2f& terrainCenter, const NzVector2f& terrainSize)
{
    root = new NzNode(this,0,terrainCenter,terrainSize);
}

void NzQuadTree::RegisterLeaf(NzNode* node)
{
    leaves.push_back(node);
}

bool NzQuadTree::UnRegisterLeaf(NzNode* node)
{
    leaves.remove(node);
}

NzNode* NzQuadTree::GetRootPtr()
{
    return root;
}

NzQuadTree::~NzQuadTree()
{
    //dtor
}

