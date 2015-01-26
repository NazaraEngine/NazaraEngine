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

NzVertexMapper::NzVertexMapper(NzSubMesh* subMesh, nzBufferAccess access)
{
	NzErrorFlags flags(nzErrorFlag_ThrowException, true);

	NzVertexBuffer* buffer = nullptr;
	switch (subMesh->GetAnimationType())
	{
		case nzAnimationType_Skeletal:
		{
			NzSkeletalMesh* skeletalMesh = static_cast<NzSkeletalMesh*>(subMesh);
			buffer = skeletalMesh->GetVertexBuffer();
			break;
		}

		case nzAnimationType_Static:
		{
			NzStaticMesh* staticMesh = static_cast<NzStaticMesh*>(subMesh);
			buffer = staticMesh->GetVertexBuffer();
			break;
		}
	}

	if (!buffer)
	{
		NazaraInternalError("Animation type not handled (0x" + NzString::Number(subMesh->GetAnimationType(), 16) + ')');
	}

	m_mapper.Map(buffer, access);
}

NzVertexMapper::NzVertexMapper(NzVertexBuffer* vertexBuffer, nzBufferAccess access)
{
	NzErrorFlags flags(nzErrorFlag_ThrowException, true);
	m_mapper.Map(vertexBuffer, access);
}

NzVertexMapper::~NzVertexMapper() = default;

void NzVertexMapper::Unmap()
{
	m_mapper.Unmap();
}
