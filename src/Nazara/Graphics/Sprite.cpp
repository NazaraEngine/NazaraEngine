// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <cstring>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

void NzSprite::AddToRenderQueue(NzAbstractRenderQueue* renderQueue, const InstanceData& instanceData) const
{
	if (!m_material)
		return;

	const NzVertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<const NzVertexStruct_XYZ_Color_UV*>(instanceData.data.data());
	renderQueue->AddSprites(m_material, vertices, 1);
}

void NzSprite::MakeBoundingVolume() const
{
	m_boundingVolume.Set(NzVector3f(0.f), m_size.x*NzVector3f::Right() + m_size.y*NzVector3f::Down());
}

void NzSprite::UpdateData(InstanceData* instanceData) const
{
	instanceData->data.resize(4 * sizeof(NzVertexStruct_XYZ_Color_UV));
	NzVertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<NzVertexStruct_XYZ_Color_UV*>(instanceData->data.data());

	NzSparsePtr<NzColor> colorPtr(&vertices[0].color, sizeof(NzVertexStruct_XYZ_Color_UV));
	NzSparsePtr<NzVector3f> posPtr(&vertices[0].position, sizeof(NzVertexStruct_XYZ_Color_UV));
	NzSparsePtr<NzVector2f> texCoordPtr(&vertices[0].uv, sizeof(NzVertexStruct_XYZ_Color_UV));

	*colorPtr++ = m_color;
	*posPtr++ = instanceData->transformMatrix.Transform(NzVector3f(0.f));
	*texCoordPtr++ = m_textureCoords.GetCorner(nzRectCorner_LeftTop);

	*colorPtr++ = m_color;
	*posPtr++ = instanceData->transformMatrix.Transform(m_size.x*NzVector3f::Right());
	*texCoordPtr++ = m_textureCoords.GetCorner(nzRectCorner_RightTop);

	*colorPtr++ = m_color;
	*posPtr++ = instanceData->transformMatrix.Transform(m_size.y*NzVector3f::Down());
	*texCoordPtr++ = m_textureCoords.GetCorner(nzRectCorner_LeftBottom);

	*colorPtr++ = m_color;
	*posPtr++ = instanceData->transformMatrix.Transform(m_size.x*NzVector3f::Right() + m_size.y*NzVector3f::Down());
	*texCoordPtr++ = m_textureCoords.GetCorner(nzRectCorner_RightBottom);
}

NzSpriteLibrary::LibraryMap NzSprite::s_library;
