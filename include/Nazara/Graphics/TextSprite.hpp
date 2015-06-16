// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTSPRITE_HPP
#define NAZARA_TEXTSPRITE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Utility/AbstractAtlas.hpp>
#include <Nazara/Utility/AbstractTextDrawer.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <memory>
#include <set>

class NzTextSprite;

using NzTextSpriteConstRef = NzObjectRef<const NzTextSprite>;
using NzTextSpriteLibrary = NzObjectLibrary<NzTextSprite>;
using NzTextSpriteRef = NzObjectRef<NzTextSprite>;

class NAZARA_GRAPHICS_API NzTextSprite : public NzInstancedRenderable
{
	public:
		inline NzTextSprite();
		inline NzTextSprite(const NzTextSprite& sprite);
		~NzTextSprite() = default;

		void AddToRenderQueue(NzAbstractRenderQueue* renderQueue, const InstanceData& instanceData) const override;

		inline void Clear();

		inline const NzColor& GetColor() const;
		inline const NzMaterialRef& GetMaterial() const;
		inline float GetScale() const;

		inline void SetColor(const NzColor& color);
		inline void SetDefaultMaterial();
		inline void SetMaterial(NzMaterialRef material);
		inline void SetScale(float scale);

		void Update(const NzAbstractTextDrawer& drawer);

		inline NzTextSprite& operator=(const NzTextSprite& text);

		template<typename... Args> static NzTextSpriteRef New(Args&&... args);

	private:
		inline void InvalidateVertices();
		void MakeBoundingVolume() const override;
		void OnAtlasInvalidated(const NzAbstractAtlas* atlas);
		void OnAtlasLayerChange(const NzAbstractAtlas* atlas, NzAbstractImage* oldLayer, NzAbstractImage* newLayer);
		void UpdateData(InstanceData* instanceData) const override;

		struct RenderIndices
		{
			unsigned int first;
			unsigned int count;
		};

		struct AtlasSlots
		{
			NazaraSlot(NzAbstractAtlas, OnAtlasCleared, clearSlot);
			NazaraSlot(NzAbstractAtlas, OnAtlasLayerChange, layerChangeSlot);
			NazaraSlot(NzAbstractAtlas, OnAtlasRelease, releaseSlot);
		};

		std::unordered_map<const NzAbstractAtlas*, AtlasSlots> m_atlases;
		mutable std::unordered_map<NzTexture*, RenderIndices> m_renderInfos;
		mutable std::vector<NzVertexStruct_XY_Color_UV> m_localVertices;
		NzColor m_color;
		NzMaterialRef m_material;
		NzRectui m_localBounds;
		mutable bool m_verticesUpdated;
		float m_scale;

		static NzTextSpriteLibrary::LibraryMap s_library;
};

#include <Nazara/Graphics/TextSprite.inl>

#endif // NAZARA_TEXTSPRITE_HPP
