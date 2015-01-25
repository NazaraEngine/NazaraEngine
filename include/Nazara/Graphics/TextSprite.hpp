// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTSPRITE_HPP
#define NAZARA_TEXTSPRITE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/SceneNode.hpp>
#include <Nazara/Utility/AbstractAtlas.hpp>
#include <Nazara/Utility/AbstractTextDrawer.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <memory>
#include <set>

class NAZARA_API NzTextSprite : public NzSceneNode, NzAbstractAtlas::Listener
{
	public:
		NzTextSprite();
		NzTextSprite(const NzTextSprite& sprite);
		~NzTextSprite();

		void AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const override;

		void Clear();

		NzTextSprite* Clone() const;
		NzTextSprite* Create() const;

		const NzColor& GetColor() const;
		NzMaterial* GetMaterial() const;
		nzSceneNodeType GetSceneNodeType() const override;

		void InvalidateVertices();
		bool IsDrawable() const;

		void SetColor(const NzColor& color);
		void SetDefaultMaterial();
		void SetMaterial(NzMaterial* material);

		void Update(const NzAbstractTextDrawer& drawer);

		NzTextSprite& operator=(const NzTextSprite& text);

	private:
		void ClearAtlases();
		void InvalidateNode() override;
		void MakeBoundingVolume() const override;
		bool OnAtlasCleared(const NzAbstractAtlas* atlas, void* userdata) override;
		bool OnAtlasLayerChange(const NzAbstractAtlas* atlas, NzAbstractImage* oldLayer, NzAbstractImage* newLayer, void* userdata) override;
		void OnAtlasReleased(const NzAbstractAtlas* atlas, void* userdata) override;
		void Register() override;
		void Unregister() override;
		void UpdateVertices() const;

		struct RenderIndices
		{
			unsigned int first;
			unsigned int count;
		};

		std::set<const NzAbstractAtlas*> m_atlases;
		mutable std::unordered_map<NzTexture*, RenderIndices> m_renderInfos;
		mutable std::vector<NzVertexStruct_XY_Color> m_localVertices;
		mutable std::vector<NzVertexStruct_XYZ_Color_UV> m_vertices;
		NzColor m_color;
		NzMaterialRef m_material;
		NzRectui m_localBounds;
		mutable bool m_verticesUpdated;
};

#endif // NAZARA_TEXTSPRITE_HPP
