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
m_parent(parent),
m_matIndex(0)
{
}

NzSubMesh::~NzSubMesh() = default;

const NzMesh* NzSubMesh::GetParent() const
{
	return m_parent;
}

nzPrimitiveType NzSubMesh::GetPrimitiveType() const
{
	return m_primitiveType;
}

unsigned int NzSubMesh::GetMaterialIndex() const
{
	return m_matIndex;
}

void NzSubMesh::SetPrimitiveType(nzPrimitiveType primitiveType)
{
	m_primitiveType = primitiveType;
}

void NzSubMesh::SetMaterialIndex(unsigned int matIndex)
{
	m_matIndex = matIndex;
}
