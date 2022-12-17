// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Tilemap.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/RenderSpriteChain.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a Tilemap object, containing mapSize tileSize-sized tiles
	*
	* \param mapSize Number of tiles in each dimension, must be
	* \param tileSize Size of each tile of the Tilemap
	* \param materialCount The maximum number of differents Material this Tilemap will use
	*
	* \remark When constructed, a Tilemap has no tile active and will not be rendered
	* To use it, you have to enable some tiles.
	*
	* \remark The default material is used for every material requested
	*/
	Tilemap::Tilemap(const Vector2ui& mapSize, const Vector2f& tileSize, std::size_t materialCount) :
	m_tiles(mapSize.x* mapSize.y),
	m_layers(materialCount),
	m_mapSize(mapSize),
	m_tileSize(tileSize),
	m_origin(0.f, 0.f),
	m_isometricModeEnabled(false),
	m_shouldRebuildVertices(false)
	{
		NazaraAssert(m_tiles.size() != 0U, "Invalid map size");
		NazaraAssert(m_tileSize.x > 0 && m_tileSize.y > 0, "Invalid tile size");
		NazaraAssert(m_layers.size() != 0U, "Invalid material count");

		std::shared_ptr<MaterialInstance> defaultMaterialInstance = Graphics::Instance()->GetDefaultMaterials().basicDefault;
		for (auto& layer : m_layers)
			layer.material = defaultMaterialInstance;
	}

	void Tilemap::BuildElement(ElementRendererRegistry& registry, const ElementData& elementData, std::size_t passIndex, std::vector<RenderElementOwner>& elements) const
	{
		if (m_shouldRebuildVertices)
			UpdateVertices();

		const std::shared_ptr<VertexDeclaration>& vertexDeclaration = VertexDeclaration::Get(VertexLayout::XYZ_Color_UV);

		RenderPipelineInfo::VertexBufferData vertexBufferData = {
			0,
			vertexDeclaration
		};

		const auto& whiteTexture = Graphics::Instance()->GetDefaultTextures().whiteTextures[UnderlyingCast(ImageType::E2D)];

		const VertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<const VertexStruct_XYZ_Color_UV*>(m_vertices.data());
		for (std::size_t layerIndex = 0; layerIndex < m_layers.size(); ++layerIndex)
		{
			const auto& layer = m_layers[layerIndex];
			if (layer.tiles.empty())
				continue;

			const auto& materialPipeline = layer.material->GetPipeline(passIndex);
			if (!materialPipeline)
				return;

			MaterialPassFlags passFlags = layer.material->GetPassFlags(passIndex);

			const auto& renderPipeline = materialPipeline->GetRenderPipeline(&vertexBufferData, 1);

			std::size_t spriteCount = layer.tiles.size();
			elements.emplace_back(registry.AllocateElement<RenderSpriteChain>(GetRenderLayer(), layer.material, passFlags, renderPipeline, *elementData.worldInstance, vertexDeclaration, whiteTexture, spriteCount, vertices, *elementData.scissorBox));

			vertices += 4 * spriteCount;
		}
	}
	
	const std::shared_ptr<MaterialInstance>& Tilemap::GetMaterial(std::size_t i) const
	{
		assert(i < m_layers.size());
		return m_layers[i].material;
	}

	std::size_t Tilemap::GetMaterialCount() const
	{
		return m_layers.size();
	}

	void Tilemap::SetMaterial(std::size_t matIndex, std::shared_ptr<MaterialInstance> material)
	{
		NazaraAssert(matIndex < m_layers.size(), "Material index out of bounds");
		NazaraAssert(material, "invalid material");

		OnMaterialInvalidated(this, matIndex, material);
		m_layers[matIndex].material = std::move(material);
	}

	Vector3ui Tilemap::GetTextureSize(std::size_t matIndex) const
	{
		assert(matIndex < m_layers.size());

		//TODO: Cache index in registry?
		if (const std::shared_ptr<Texture>* textureOpt = m_layers[matIndex].material->GetTextureProperty("BaseColorMap"))
		{
			// Material should always have textures but we're better safe than sorry
			if (const std::shared_ptr<Texture>& texture = *textureOpt)
				return texture->GetSize();
		}

		// Couldn't get material pass or texture
		return Vector3ui::Unit(); //< prevents division by zero
	}

	void Tilemap::UpdateVertices() const
	{
		std::size_t spriteCount = 0;
		for (const Layer& layer : m_layers)
			spriteCount += layer.tiles.size();

		m_vertices.resize(spriteCount * 4);
		VertexStruct_XYZ_Color_UV* vertexPtr = reinterpret_cast<VertexStruct_XYZ_Color_UV*>(m_vertices.data());

		float topCorner = m_tileSize.y * (m_mapSize.y - 1);
		Vector3f originShift = m_origin * GetSize();

		for (const Layer& layer : m_layers)
		{
			for (std::size_t tileIndex : layer.tiles)
			{
				const Tile& tile = m_tiles[tileIndex];
				NazaraAssert(tile.enabled, "Tile specified for rendering is not enabled");

				std::size_t x = tileIndex % m_mapSize.x;
				std::size_t y = tileIndex / m_mapSize.x;

				Vector3f tileLeftBottom;
				if (m_isometricModeEnabled)
					tileLeftBottom.Set(x * m_tileSize.x + m_tileSize.x / 2.f * (y % 2), topCorner - y / 2.f * m_tileSize.y, 0.f);
				else
					tileLeftBottom.Set(x * m_tileSize.x, topCorner - y * m_tileSize.y, 0.f);

				std::array<Vector2f, RectCornerCount> cornerExtent;
				cornerExtent[UnderlyingCast(RectCorner::LeftBottom)] = Vector2f(0.f, 0.f);
				cornerExtent[UnderlyingCast(RectCorner::RightBottom)] = Vector2f(1.f, 0.f);
				cornerExtent[UnderlyingCast(RectCorner::LeftTop)] = Vector2f(0.f, 1.f);
				cornerExtent[UnderlyingCast(RectCorner::RightTop)] = Vector2f(1.f, 1.f);

				for (RectCorner corner : { RectCorner::LeftBottom, RectCorner::RightBottom, RectCorner::LeftTop, RectCorner::RightTop })
				{
					vertexPtr->color = tile.color;
					vertexPtr->position = tileLeftBottom + Vector3f(m_tileSize * cornerExtent[UnderlyingCast(corner)], 0.f) - originShift;
					vertexPtr->uv = tile.textureCoords.GetCorner(corner);
					++vertexPtr;
				}
			}
		}

		m_shouldRebuildVertices = false;
	}
}
