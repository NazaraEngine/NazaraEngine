// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef QUADTREE_H
#define QUADTREE_H

#include <Nazara/Prerequesites.hpp>

#include <list>
#include <Nazara/Math/Vector2.hpp>
#include "Node.hpp"

class NzQuadTree
{
    public:
        NzQuadTree(const NzVector2f& terrainCenter, const NzVector2f& terrainSize);
        void RegisterLeaf(NzNode* node);
        bool UnRegisterLeaf(NzNode* node);
        NzNode* GetRootPtr();
        ~NzQuadTree();
    private:
        NzNode* root;
        //N'a pas la charge des objets en mémoire
        std::list<NzNode*> leaves;
};

#endif // QUADTREE_H
