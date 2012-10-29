// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

NzSubMesh::NzSubMesh(const NzMesh* parent) :
NzResource(false), // Un SubMesh n'est pas persistant par défaut
m_parent(parent)
{
	#ifdef NAZARA_DEBUG
	if (!m_parent)
	{
		NazaraError("Parent mesh must be valid");
		throw std::invalid_argument("Parent mesh must be valid");
	}
	#endif
}

NzSubMesh::~NzSubMesh() = default;

void NzSubMesh::Animate(unsigned int frameA, unsigned int frameB, float interpolation)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_parent->HasAnimation())
	{
		NazaraError("Parent mesh has no animation");
		return;
	}

	unsigned int frameCount = m_parent->GetFrameCount();
	if (frameA >= frameCount)
	{
		NazaraError("Frame A is out of range (" + NzString::Number(frameA) + " >= " + NzString::Number(frameCount) + ')');
		return;
	}

	if (frameB >= frameCount)
	{
		NazaraError("Frame B is out of range (" + NzString::Number(frameB) + " >= " + NzString::Number(frameCount) + ')');
		return;
	}
	#endif

	#ifdef NAZARA_DEBUG
	if (interpolation < 0.f || interpolation > 1.f)
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return;
	}
	#endif

	AnimateImpl(frameA, frameB, interpolation);

	m_parent->InvalidateAABB();
}

const NzMesh* NzSubMesh::GetParent() const
{
	return m_parent;
}

unsigned int NzSubMesh::GetVertexCount() const
{
	return GetVertexBuffer()->GetVertexCount();
}
