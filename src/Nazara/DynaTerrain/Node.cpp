// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "Node.hpp"
#include "QuadTree.hpp"
#include <stack>
//#include <Nazara/DynaTerrain/NoiseBase.hpp>
//#include <Nazara/DynaTerrain/Error.hpp>
//#include <Nazara/DynaTerrain/Config.hpp>
//#include <Nazara/DynaTerrain/Debug.hpp>


NzNode::NzNode(NzQuadTree* quad, NzNode* parent, const NzVector2f& center, const NzVector2f& size, nzDirection dir)
{
    m_direction = dir;
    m_center = center;
    m_size = size;
    m_isLeaf = false;
    m_patchMemoryAllocated = false;

    m_associatedQuadTree = quad;

    m_topLeftLeaf = nullptr;
    m_topRightLeaf = nullptr;
    m_bottomLeftLeaf = nullptr;
    m_bottomRightLeaf = nullptr;

    if(parent == 0)
    {
        m_isRoot = true;
        m_level = 0;
    }
    else
    {
        m_level = parent->GetLevel()+1;
        m_parent = parent;
        m_isRoot = false;
    }
}

NzNode::~NzNode()
{
    if(m_isLeaf)
        m_associatedQuadTree->UnRegisterLeaf(this);
    if(m_patchMemoryAllocated)
        delete m_patch;
}

void NzNode::Subdivide()
{
    m_isLeaf = false;
    m_associatedQuadTree->UnRegisterLeaf(this);

    if(m_topLeftLeaf == nullptr)
    {
        m_topLeftLeaf = new NzNode(m_associatedQuadTree,this,NzVector2f(m_center.x-m_size.x/2.f,
                                                                        m_center.y+m_size.y/2.f),m_size/2.f,TOPLEFT);
        m_topLeftLeaf->m_isLeaf = true;
        m_associatedQuadTree->RegisterLeaf(m_topLeftLeaf);
    }
    if(m_topRightLeaf == nullptr)
    {
        m_topRightLeaf = new NzNode(m_associatedQuadTree,this,NzVector2f(m_center.x+m_size.x/2.f,
                                                                         m_center.y+m_size.y/2.f),m_size/2.f,TOPRIGHT);
        m_topRightLeaf->m_isLeaf = true;
        m_associatedQuadTree->RegisterLeaf(m_topRightLeaf);
    }
    if(m_bottomLeftLeaf = nullptr)
    {
        m_bottomLeftLeaf = new NzNode(m_associatedQuadTree,this,NzVector2f(m_center.x-m_size.x/2.f,
                                                                           m_center.y-m_size.y/2.f),m_size/2.f,BOTTOMLEFT);
        m_bottomLeftLeaf->m_isLeaf = true;
        m_associatedQuadTree->RegisterLeaf(m_bottomLeftLeaf);
    }
    if(m_bottomRightLeaf = nullptr)
    {
        m_bottomRightLeaf = new NzNode(m_associatedQuadTree,this,NzVector2f(m_center.x+m_size.x/2.f,
                                                                            m_center.y-m_size.y/2.f),m_size/2.f,BOTTOMRIGHT);
        m_bottomRightLeaf->m_isLeaf = true;
        m_associatedQuadTree->RegisterLeaf(m_bottomRightLeaf);
    }


}

void NzNode::Refine(bool eraseMemory)
{
    m_isLeaf = true;
    m_associatedQuadTree->RegisterLeaf(this);
    if(eraseMemory)
    {
        /*
        delete m_topLeftLeaf;
        delete m_topRightLeaf;
        delete m_bottomLeftLeaf;
        delete m_bottomRightLeaf;

        m_topLeftLeaf = nullptr;
        m_topRightLeaf = nullptr;
        m_bottomLeftLeaf = nullptr;
        m_bottomRightLeaf = nullptr;*/

        m_topLeftLeaf->DeletePatch();
        m_topRightLeaf->DeletePatch();
        m_bottomLeftLeaf->DeletePatch();
        m_bottomRightLeaf->DeletePatch();
    }
    //else
    //{
        m_topLeftLeaf->m_isLeaf = false;
        m_topRightLeaf->m_isLeaf = false;
        m_bottomLeftLeaf->m_isLeaf = false;
        m_bottomRightLeaf->m_isLeaf = false;

        m_associatedQuadTree->UnRegisterLeaf(m_topLeftLeaf);
        m_associatedQuadTree->UnRegisterLeaf(m_topRightLeaf);
        m_associatedQuadTree->UnRegisterLeaf(m_bottomLeftLeaf);
        m_associatedQuadTree->UnRegisterLeaf(m_bottomRightLeaf);
    //}

}

void NzNode::CreatePatch(const NzVector2f& center, const NzVector2f& size)
{
    m_patchMemoryAllocated = true;
    m_patch = new NzPatch(center,size);
}

void NzNode::DeletePatch()
{
    m_patchMemoryAllocated = false;
    delete m_patch;
}

unsigned short int NzNode::GetLevel() const
{
    return m_level;
}

bool NzNode::LocateNeighbor(nzDirection dir, NzNode* neighbor)
{
    NzNode* temp = m_parent;
    std::stack<nzDirection> treePath;
    treePath.push(m_direction);
    neighbor = nullptr;

    switch(dir)
    {
        case TOP:
            //Part 1
            while(temp->m_direction != (BOTTOMLEFT || BOTTOMRIGHT || CENTER))
            {
                treePath.push(temp->m_direction);
                temp = temp->m_parent;
            }

            //Part 2
            if(temp->m_direction == BOTTOMLEFT)
                temp = temp->m_parent->m_topLeftLeaf;
            else if(temp->m_direction == BOTTOMRIGHT)
                temp = temp->m_parent->m_topRightLeaf;
            else if(temp->m_direction == CENTER)
                return false;//No Neighbor existing

            //Part 3
            while(!temp->IsLeaf())
            {
                if(treePath.empty())
                {
                    //Le chemin de redescente est plus court que celui de montée (level[node départ] < level[node d'arrivée])
                    break;
                }
                else
                {
                    if(treePath.top() == TOPRIGHT)
                    temp = temp->m_bottomRightLeaf;
                    else if(treePath.top() == TOPLEFT)
                        temp = temp->m_bottomLeftLeaf;
                    else//D'uh ?
                        {
                            //Logger une erreur
                            return false;
                        }
                    treePath.pop();
                }
            }
            neighbor = temp;
            return true;
        break;

        case BOTTOM:
            //Part 1
            while(temp->m_direction != (TOPLEFT || TOPRIGHT || CENTER))
            {
                treePath.push(temp->m_direction);
                temp = temp->m_parent;
            }

            //Part 2
            if(temp->m_direction == TOPLEFT)
                temp = temp->m_parent->m_bottomLeftLeaf;
            else if(temp->m_direction == TOPRIGHT)
                temp = temp->m_parent->m_bottomRightLeaf;
            else if(temp->m_direction == CENTER)
                return false;//No Neighbor existing

            //Part 3
            while(!temp->IsLeaf())
            {
                if(treePath.empty())
                {
                    //Le chemin de redescente est plus court que celui de montée (level[node départ] < level[node d'arrivée])
                    break;
                }
                else
                {
                    if(treePath.top() == BOTTOMRIGHT)
                        temp = temp->m_topRightLeaf;
                    else if(treePath.top() == BOTTOMLEFT)
                        temp = temp->m_topLeftLeaf;
                    else//D'uh ?
                        {
                            //Logger une erreur
                            return false;
                        }
                    treePath.pop();
                }
            }
            neighbor = temp;
            return true;
        break;

        case LEFT:
                //Part 1
                while(temp->m_direction != (TOPRIGHT || BOTTOMRIGHT || CENTER))
                {
                    treePath.push(temp->m_direction);
                    temp = temp->m_parent;
                }

                //Part 2
                if(temp->m_direction == TOPRIGHT)
                    temp = temp->m_parent->m_topLeftLeaf;
                else if(temp->m_direction == BOTTOMRIGHT)
                    temp = temp->m_parent->m_bottomLeftLeaf;
                else if(temp->m_direction == CENTER)
                    return false;//No Neighbor existing

                //Part 3
                while(!temp->IsLeaf())
                {
                    if(treePath.top() == TOPLEFT)
                        temp = temp->m_topRightLeaf;
                    else if(treePath.top() == BOTTOMLEFT)
                        temp = temp->m_bottomRightLeaf;
                    else//D'uh ?
                        {
                            //Logger une erreur
                            return false;
                        }
                    treePath.pop();
                }
                neighbor = temp;
                return true;
        break;

        case RIGHT:
                //Part 1
                while(temp->m_direction != (TOPLEFT || BOTTOMLEFT || CENTER))
                {
                    treePath.push(temp->m_direction);
                    temp = temp->m_parent;
                }

                //Part 2
                if(temp->m_direction == TOPLEFT)
                    temp = temp->m_parent->m_topRightLeaf;
                else if(temp->m_direction == BOTTOMLEFT)
                    temp = temp->m_parent->m_bottomRightLeaf;
                else if(temp->m_direction == CENTER)
                    return false;//No Neighbor existing

                //Part 3
                while(!temp->IsLeaf())
                {
                    if(treePath.top() == TOPRIGHT)
                        temp = temp->m_topLeftLeaf;
                    else if(treePath.top() == BOTTOMRIGHT)
                        temp = temp->m_bottomLeftLeaf;
                    else//D'uh ?
                        {
                            //Logger une erreur
                            return false;
                        }
                    treePath.pop();
                }
                neighbor = temp;
                return true;
        break;

        default :
            return false;
        break;
    }
}

bool NzNode::IsLeaf() const
{
    return m_isLeaf;
}

bool NzNode::IsRoot() const
{
    return m_isRoot;
}

