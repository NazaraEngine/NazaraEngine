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
	/*!
	* \ingroup graphics
	* \class Nz::Sprite
	* \brief Graphics class that represents the rendering of a sprite
	*/

	/*!
	* \brief Adds the sprite to the rendering queue
	*
	* \param renderQueue Queue to be added
	* \param instanceData Data for the instance
	*/

	void Sprite::AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData) const
	{
		if (!m_material)
			return;

		const VertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<const VertexStruct_XYZ_Color_UV*>(instanceData.data.data());
		renderQueue->AddSprites(instanceData.renderOrder, m_material, vertices, 1);
	}

	/*!
	* \brief Makes the bounding volume of this text
	*/

	void Sprite::MakeBoundingVolume() const
	{
		Vector3f origin(m_origin.x, -m_origin.y, m_origin.z);

		m_boundingVolume.Set(-origin, m_size.x*Vector3f::Right() + m_size.y*Vector3f::Down() - origin);
	}

	/*!
	* \brief Updates the data of the sprite
	*
	* \param instanceData Data of the instance
	*/
	void Sprite::UpdateData(InstanceData* instanceData) const
	{
		instanceData->data.resize(4 * sizeof(VertexStruct_XYZ_Color_UV));
		VertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<VertexStruct_XYZ_Color_UV*>(instanceData->data.data());

		SparsePtr<Color> colorPtr(&vertices[0].color, sizeof(VertexStruct_XYZ_Color_UV));
		SparsePtr<Vector3f> posPtr(&vertices[0].position, sizeof(VertexStruct_XYZ_Color_UV));
		SparsePtr<Vector2f> texCoordPtr(&vertices[0].uv, sizeof(VertexStruct_XYZ_Color_UV));

		Vector3f origin(m_origin.x, -m_origin.y, m_origin.z);

		*colorPtr++ = m_color * m_cornerColor[RectCorner_LeftTop];
		*posPtr++ = instanceData->transformMatrix.Transform(Vector3f(-origin));
		*texCoordPtr++ = m_textureCoords.GetCorner(RectCorner_LeftTop);

		*colorPtr++ = m_color * m_cornerColor[RectCorner_RightTop];
		*posPtr++ = instanceData->transformMatrix.Transform(m_size.x*Vector3f::Right() - origin);
		*texCoordPtr++ = m_textureCoords.GetCorner(RectCorner_RightTop);

		*colorPtr++ = m_color * m_cornerColor[RectCorner_LeftBottom];
		*posPtr++ = instanceData->transformMatrix.Transform(m_size.y*Vector3f::Down() - origin);
		*texCoordPtr++ = m_textureCoords.GetCorner(RectCorner_LeftBottom);

		*colorPtr++ = m_color * m_cornerColor[RectCorner_RightBottom];
		*posPtr++ = instanceData->transformMatrix.Transform(m_size.x*Vector3f::Right() + m_size.y*Vector3f::Down() - origin);
		*texCoordPtr++ = m_textureCoords.GetCorner(RectCorner_RightBottom);
	}

	/*!
	* \brief Initializes the sprite library
	* \return true If successful
	*
	* \remark Produces a NazaraError if the sprite library failed to be initialized
	*/

	bool Sprite::Initialize()
	{
		if (!SpriteLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	/*!
	* \brief Uninitializes the sprite library
	*/

	void Sprite::Uninitialize()
	{
		SpriteLibrary::Uninitialize();
	}

	SpriteLibrary::LibraryMap Sprite::s_library;
}
