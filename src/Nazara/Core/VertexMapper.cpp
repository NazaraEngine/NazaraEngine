// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/VertexMapper.hpp>
#include <Nazara/Core/BufferMapper.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/SkeletalMesh.hpp>
#include <Nazara/Core/StaticMesh.hpp>
#include <Nazara/Core/SubMesh.hpp>

namespace Nz
{
	VertexMapper::VertexMapper(SubMesh& subMesh)
	{
		ErrorFlags flags(ErrorMode::ThrowException);

		std::shared_ptr<VertexBuffer> buffer = nullptr;
		switch (subMesh.GetAnimationType())
		{
			case AnimationType::Skeletal:
			{
				SkeletalMesh& skeletalMesh = static_cast<SkeletalMesh&>(subMesh);
				buffer = skeletalMesh.GetVertexBuffer();
				break;
			}

			case AnimationType::Static:
			{
				StaticMesh& staticMesh = static_cast<StaticMesh&>(subMesh);
				buffer = staticMesh.GetVertexBuffer();
				break;
			}
		}

		if (!buffer)
		{
			NazaraInternalError("animation type not handled ({0:#x})", UnderlyingCast(subMesh.GetAnimationType()));
		}

		m_mapper.Map(*buffer, 0, buffer->GetVertexCount());
	}

	VertexMapper::VertexMapper(VertexBuffer& vertexBuffer)
	{
		ErrorFlags flags(ErrorMode::ThrowException);
		m_mapper.Map(vertexBuffer, 0, vertexBuffer.GetVertexCount());
	}

	VertexMapper::~VertexMapper() = default;

	void VertexMapper::Unmap()
	{
		m_mapper.Unmap();
	}
}
