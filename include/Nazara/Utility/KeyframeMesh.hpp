// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_KEYFRAMEMESH_HPP
#define NAZARA_KEYFRAMEMESH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/SubMesh.hpp>

class NzMesh;
struct NzAnimation;

class NAZARA_API NzKeyframeMesh : public NzSubMesh
{
	public:
		NzKeyframeMesh(const NzMesh* parent);
		virtual ~NzKeyframeMesh();
};

#endif // NAZARA_KEYFRAMEMESH_HPP
