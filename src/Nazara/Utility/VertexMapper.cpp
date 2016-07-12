// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexMapper.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	VertexMapper::VertexMapper(SubMesh* subMesh, BufferAccess access)
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);

		VertexBuffer* buffer = nullptr;
		switch (subMesh->GetAnimationType())
		{
			case AnimationType_Skeletal:
			{
				SkeletalMesh* skeletalMesh = static_cast<SkeletalMesh*>(subMesh);
				buffer = skeletalMesh->GetVertexBuffer();
				break;
			}

			case AnimationType_Static:
			{
				StaticMesh* staticMesh = static_cast<StaticMesh*>(subMesh);
				buffer = staticMesh->GetVertexBuffer();
				break;
			}
		}

		if (!buffer)
		{
			NazaraInternalError("Animation type not handled (0x" + String::Number(subMesh->GetAnimationType(), 16) + ')');
		}

		m_mapper.Map(buffer, access);
	}

	VertexMapper::VertexMapper(VertexBuffer* vertexBuffer, BufferAccess access)
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);
		m_mapper.Map(vertexBuffer, access);
	}
	
	VertexMapper::VertexMapper(const SubMesh* subMesh, BufferAccess access)
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);

		const VertexBuffer* buffer = nullptr;
		switch (subMesh->GetAnimationType())
		{
			case AnimationType_Skeletal:
			{
				const SkeletalMesh* skeletalMesh = static_cast<const SkeletalMesh*>(subMesh);
				buffer = skeletalMesh->GetVertexBuffer();
				break;
			}

			case AnimationType_Static:
			{
				const StaticMesh* staticMesh = static_cast<const StaticMesh*>(subMesh);
				buffer = staticMesh->GetVertexBuffer();
				break;
			}
		}

		if (!buffer)
		{
			NazaraInternalError("Animation type not handled (0x" + String::Number(subMesh->GetAnimationType(), 16) + ')');
		}

		m_mapper.Map(buffer, access);
	}

	VertexMapper::VertexMapper(const VertexBuffer* vertexBuffer, BufferAccess access)
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);
		m_mapper.Map(vertexBuffer, access);
	}

	VertexMapper::~VertexMapper() = default;

	void VertexMapper::Unmap()
	{
		m_mapper.Unmap();
	}
}
