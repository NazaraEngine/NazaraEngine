// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPRITE_HPP
#define NAZARA_SPRITE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/SceneNode.hpp>
#include <Nazara/Utility/VertexStruct.hpp>

class NAZARA_API NzSprite : public NzSceneNode
{
	public:
		NzSprite();
		NzSprite(NzTexture* texture);
		NzSprite(const NzSprite& sprite);
		~NzSprite() = default;

		void AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const override;

		NzSprite* Clone() const;
		NzSprite* Create() const;

		const NzColor& GetColor() const;
		NzMaterial* GetMaterial() const;
		nzSceneNodeType GetSceneNodeType() const override;
		const NzVector2f& GetSize() const;
		const NzRectf& GetTextureCoords() const;

		bool IsDrawable() const;

		void SetColor(const NzColor& color);
		void SetDefaultMaterial();
		void SetMaterial(NzMaterial* material, bool resizeSprite = true);
		void SetSize(const NzVector2f& size);
		void SetSize(float sizeX, float sizeY);
		void SetTexture(NzTexture* texture, bool resizeSprite = true);
		void SetTextureCoords(const NzRectf& coords);
		void SetTextureRect(const NzRectui& rect);

		NzSprite& operator=(const NzSprite& sprite);

	private:
		void InvalidateNode() override;
		void MakeBoundingVolume() const override;
		void Register() override;
		void Unregister() override;
		void UpdateVertices() const;

		NzColor m_color;
		NzMaterialRef m_material;
		NzRectf m_textureCoords;
		NzVector2f m_size;
		mutable NzVertexStruct_XYZ_Color_UV m_vertices[4];
		mutable bool m_verticesUpdated;
};

#endif // NAZARA_SPRITE_HPP
