// Copyright (C) 2013 Jérôme Leclercq
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
		NzSprite(const NzSprite& sprite);
		NzSprite(NzSprite&& sprite);
		~NzSprite();

		void AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const override;

		const NzBoundingVolumef& GetBoundingVolume() const override;
		NzMaterial* GetMaterial() const;
		nzSceneNodeType GetSceneNodeType() const override;
		const NzVector2f& GetSize() const;
		const NzRectf& GetTextureCoords() const;

		void SetMaterial(NzMaterial* material);
		void SetSize(const NzVector2f& size);
		void SetTextureCoords(const NzRectf& coords);
		void SetTextureRect(const NzRectui& rect);

	private:
		bool FrustumCull(const NzFrustumf& frustum) override;
		void Register() override;
		void Unregister() override;

		NzRectf m_textureCoords;
		NzVector2f m_size;
		NzMaterialRef m_material;
};

#endif // NAZARA_SPRITE_HPP
