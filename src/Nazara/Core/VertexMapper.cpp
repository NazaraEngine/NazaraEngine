// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/VertexMapper.hpp>
#include <Nazara/Core/BufferMapper.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/SkeletalMesh.hpp>
#include <Nazara/Core/StaticMesh.hpp>
#include <Nazara/Core/Submesh.hpp>

namespace Nz
{
	VertexMapper::VertexMapper(Submesh& submesh)
	{
		ErrorFlags flags(ErrorMode::ThrowException);

		std::shared_ptr<VertexBuffer> buffer = nullptr;
		switch (submesh.GetAnimationType())
		{
			case AnimationType::Skeletal:
			{
				SkeletalMesh& skeletalMesh = static_cast<SkeletalMesh&>(submesh);
				buffer = skeletalMesh.GetVertexBuffer();
				break;
			}

			case AnimationType::Static:
			{
				StaticMesh& staticMesh = static_cast<StaticMesh&>(submesh);
				buffer = staticMesh.GetVertexBuffer();
				break;
			}
		}

		if (!buffer)
		{
			NazaraInternalError("animation type not handled ({0:#x})", UnderlyingCast(submesh.GetAnimationType()));
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
