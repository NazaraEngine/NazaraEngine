// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPRITE_HPP
#define NAZARA_SPRITE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/SceneNode.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <array>

class NzSprite;

using NzSpriteConstRef = NzObjectRef<const NzSprite>;
using NzSpriteLibrary = NzObjectLibrary<NzSprite>;
using NzSpriteRef = NzObjectRef<NzSprite>;

class NAZARA_GRAPHICS_API NzSprite : public NzInstancedRenderable
{
	public:
		inline NzSprite();
		inline NzSprite(NzMaterialRef material);
		inline NzSprite(NzTexture* texture);
		inline NzSprite(const NzSprite& sprite);
		~NzSprite() = default;

		void AddToRenderQueue(NzAbstractRenderQueue* renderQueue, const InstanceData& instanceData) const override;

		inline const NzColor& GetColor() const;
		inline const NzMaterialRef& GetMaterial() const;
		inline const NzVector2f& GetSize() const;
		inline const NzRectf& GetTextureCoords() const;

		inline void SetColor(const NzColor& color);
		inline void SetDefaultMaterial();
		inline void SetMaterial(NzMaterialRef material, bool resizeSprite = true);
		inline void SetSize(const NzVector2f& size);
		inline void SetSize(float sizeX, float sizeY);
		inline void SetTexture(NzTextureRef texture, bool resizeSprite = true);
		inline void SetTextureCoords(const NzRectf& coords);
		inline void SetTextureRect(const NzRectui& rect);

		inline NzSprite& operator=(const NzSprite& sprite);

		template<typename... Args> static NzSpriteRef New(Args&&... args);

	private:
		inline void InvalidateVertices();
		void MakeBoundingVolume() const override;
		void UpdateData(InstanceData* instanceData) const override;

		NzColor m_color;
		NzMaterialRef m_material;
		NzRectf m_textureCoords;
		NzVector2f m_size;

		static NzSpriteLibrary::LibraryMap s_library;
};

#include <Nazara/Graphics/Sprite.inl>

#endif // NAZARA_SPRITE_HPP
