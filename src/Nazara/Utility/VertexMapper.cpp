// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/VertexMapper.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/Debug.hpp>

NzVertexMapper::NzVertexMapper(NzSubMesh* subMesh)
{
	NzErrorFlags flags(nzErrorFlag_ThrowException);
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

	m_declaration = buffer->GetVertexDeclaration();
	m_vertexCount = subMesh->GetVertexCount();

	m_mapper.Map(buffer, nzBufferAccess_ReadWrite);
}

NzVertexMapper::~NzVertexMapper() = default;

unsigned int NzVertexMapper::GetVertexCount() const
{
	return m_vertexCount;
}

void NzVertexMapper::Unmap()
{
	m_mapper.Unmap();
}
