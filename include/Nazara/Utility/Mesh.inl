// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Mesh.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	Mesh::Mesh() :
	m_materialData(1),
	m_aabbUpdated(false)
	{
	}

	Mesh::~Mesh()
	{
		OnMeshRelease(this);

		Destroy();
	}

	template<typename... Args>
	MeshRef Mesh::New(Args&&... args)
	{
		std::unique_ptr<Mesh> object(new Mesh(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Utility/DebugOff.hpp>
