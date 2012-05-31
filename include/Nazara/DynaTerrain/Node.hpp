// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NODE_H
#define NODE_H

#include <Nazara/Prerequesites.hpp>

#include <Nazara/Math/Vector2.hpp>
#include "Patch.hpp"
class NzQuadTree;


//SUPPRIMER LES NODES FILS OU SEULEMENT LES PATCHS CONTENUS ? POUR L'INSTANT SUPPRESSION DES PATCHS

class NzNode
{
    public:
        enum nzDirection
        {
            TOP,
            BOTTOM,
            LEFT,
            RIGHT,
            TOPLEFT,
            TOPRIGHT,
            BOTTOMLEFT,
            BOTTOMRIGHT,
            CENTER
        };

        NzNode(NzQuadTree* quad, NzNode* parent, const NzVector2f& center, const NzVector2f& size, nzDirection dir = CENTER);
        ~NzNode();

        void Subdivide();
        void Refine(bool eraseMemory);

        void CreatePatch(const NzVector2f& center, const NzVector2f& size);
        void DeletePatch();

        unsigned short int GetLevel() const;

        bool IsLeaf() const;
        bool IsRoot() const;

        //Retourne le voisin le plus proche dans la direction indiqué, de niveau de profondeur inférieur ou égal
        bool LocateNeighbor(nzDirection dir, NzNode* neighbor);


    private:
        NzQuadTree* m_associatedQuadTree;
        NzNode* m_parent;
        NzNode* m_topLeftLeaf;
        NzNode* m_topRightLeaf;
        NzNode* m_bottomLeftLeaf;
        NzNode* m_bottomRightLeaf;
        bool m_isLeaf;
        bool m_isRoot;
        bool m_patchMemoryAllocated;
        unsigned short int m_level;

        NzPatch* m_patch;
        NzVector2f m_center;
        NzVector2f m_size;
        nzDirection m_direction;
};

#endif // NODE_H
