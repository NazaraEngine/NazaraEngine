// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <cstring>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	void Sprite::AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData) const
	{
		if (!m_material)
			return;

		const VertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<const VertexStruct_XYZ_Color_UV*>(instanceData.data.data());
		renderQueue->AddSprites(instanceData.renderOrder, m_material, vertices, 1);
	}

	void Sprite::MakeBoundingVolume() const
	{
		m_boundingVolume.Set(Vector3f(0.f), m_size.x*Vector3f::Right() + m_size.y*Vector3f::Down());
	}

	void Sprite::UpdateData(InstanceData* instanceData) const
	{
		instanceData->data.resize(4 * sizeof(VertexStruct_XYZ_Color_UV));
		VertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<VertexStruct_XYZ_Color_UV*>(instanceData->data.data());

		SparsePtr<Color> colorPtr(&vertices[0].color, sizeof(VertexStruct_XYZ_Color_UV));
		SparsePtr<Vector3f> posPtr(&vertices[0].position, sizeof(VertexStruct_XYZ_Color_UV));
		SparsePtr<Vector2f> texCoordPtr(&vertices[0].uv, sizeof(VertexStruct_XYZ_Color_UV));

		*colorPtr++ = m_color;
		*posPtr++ = instanceData->transformMatrix.Transform(Vector3f(0.f));
		*texCoordPtr++ = m_textureCoords.GetCorner(RectCorner_LeftTop);

		*colorPtr++ = m_color;
		*posPtr++ = instanceData->transformMatrix.Transform(m_size.x*Vector3f::Right());
		*texCoordPtr++ = m_textureCoords.GetCorner(RectCorner_RightTop);

		*colorPtr++ = m_color;
		*posPtr++ = instanceData->transformMatrix.Transform(m_size.y*Vector3f::Down());
		*texCoordPtr++ = m_textureCoords.GetCorner(RectCorner_LeftBottom);

		*colorPtr++ = m_color;
		*posPtr++ = instanceData->transformMatrix.Transform(m_size.x*Vector3f::Right() + m_size.y*Vector3f::Down());
		*texCoordPtr++ = m_textureCoords.GetCorner(RectCorner_RightBottom);
	}

	bool Sprite::Initialize()
	{
		if (!SpriteLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	void Sprite::Uninitialize()
	{
		SpriteLibrary::Uninitialize();
	}

	SpriteLibrary::LibraryMap Sprite::s_library;
}
