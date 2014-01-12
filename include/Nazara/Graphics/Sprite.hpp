// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPRITE_HPP
#define NAZARA_SPRITE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/SceneNode.hpp>
#include <Nazara/Renderer/Material.hpp>

class NAZARA_API NzSprite : public NzSceneNode
{
	public:
		NzSprite();
		NzSprite(NzTexture* texture);
		NzSprite(const NzSprite& sprite);
		NzSprite(NzSprite&& sprite);
		~NzSprite();

		void AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const override;

		const NzBoundingVolumef& GetBoundingVolume() const override;
		NzMaterial* GetMaterial() const;
		nzSceneNodeType GetSceneNodeType() const override;
		const NzVector2f& GetSize() const;
		const NzRectf& GetTextureCoords() const;

		bool IsDrawable() const;

		void SetMaterial(NzMaterial* material, bool resizeSprite = true);
		void SetSize(const NzVector2f& size);
		void SetTexture(NzTexture* texture, bool resizeSprite = true);
		void SetTextureCoords(const NzRectf& coords);
		void SetTextureRect(const NzRectui& rect);

	private:
		bool FrustumCull(const NzFrustumf& frustum) override;
		void Invalidate() override;
		void Register() override;
		void Unregister() override;
		void UpdateBoundingVolume() const;

		mutable NzBoundingVolumef m_boundingVolume;
		NzMaterialRef m_material;
		NzRectf m_textureCoords;
		NzVector2f m_size;
		mutable bool m_boundingVolumeUpdated;
};

#endif // NAZARA_SPRITE_HPP
