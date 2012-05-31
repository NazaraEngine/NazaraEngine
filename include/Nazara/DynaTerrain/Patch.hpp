// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef PATCH_H
#define PATCH_H

#include <Nazara/Prerequesites.hpp>

#include <Nazara/Math/Vector2.hpp>

class NzPatch
{
    public:
        NzPatch(NzVector2f center, NzVector2f size);
        ~NzPatch();

        NzVector2f GetCenter() const;
        NzVector2f GetSize() const;

        bool IntersectsCircle(const NzVector2f& center, double radius);
        bool IsContainedByCircle(const NzVector2f& center, double radius);

        NzPatch* LocatePatch(const NzVector2f& position);

    protected:
    private:

        NzVector2f m_center;
        NzVector2f m_size;
};

#endif // PATCH_H
