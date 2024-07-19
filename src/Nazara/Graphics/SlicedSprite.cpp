// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/SlicedSprite.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/RenderSpriteChain.hpp>
#include <Nazara/Graphics/TextureAsset.hpp>

namespace Nz
{
	SlicedSprite::SlicedSprite(std::shared_ptr<MaterialInstance> material) :
	m_material(std::move(material)),
	m_color(Color::White()),
	m_textureCoords(0.f, 0.f, 1.f, 1.f),
	m_origin(0.f, 0.f)
	{
		m_size = Vector2f(Vector2ui(GetTextureSize()));
		UpdateVertices();
	}

	void SlicedSprite::BuildElement(ElementRendererRegistry& registry, const ElementData& elementData, std::size_t passIndex, std::vector<RenderElementOwner>& elements) const
	{
		const auto& materialPipeline = m_material->GetPipeline(passIndex);
		if (!materialPipeline)
			return;

		MaterialPassFlags passFlags = m_material->GetPassFlags(passIndex);

		const std::shared_ptr<VertexDeclaration>& vertexDeclaration = VertexDeclaration::Get(VertexLayout::XYZ_Color_UV);

		RenderPipelineInfo::VertexBufferData vertexBufferData = {
			0,
			vertexDeclaration
		};
		const auto& renderPipeline = materialPipeline->GetRenderPipeline(&vertexBufferData, 1);

		const auto& whiteTexture = Graphics::Instance()->GetDefaultTextures().whiteTextures[ImageType::E2D];

		elements.emplace_back(registry.AllocateElement<RenderSpriteChain>(GetRenderLayer(), m_material, passFlags, renderPipeline, *elementData.worldInstance, vertexDeclaration, whiteTexture, m_spriteCount, m_vertices.data(), *elementData.scissorBox));
	}

	const std::shared_ptr<MaterialInstance>& SlicedSprite::GetMaterial(std::size_t materialIndex) const
	{
		NazaraAssertFmt(materialIndex == 0, "material index out of range ({0} >= 1)", materialIndex);
		return m_material;
	}

	std::size_t SlicedSprite::GetMaterialCount() const
	{
		return 1;
	}

	Vector3ui SlicedSprite::GetTextureSize() const
	{
		assert(m_material);

		//TODO: Cache index in registry?
		if (const std::shared_ptr<TextureAsset>* textureOpt = m_material->GetTextureProperty("BaseColorMap"))
		{
			// Material should always have textures but we're better safe than sorry
			if (const std::shared_ptr<TextureAsset>& texture = *textureOpt)
				return texture->GetSize();
		}

		// Couldn't get material pass or texture
		return Vector3ui::Unit(); //< prevents division by zero
	}

	void SlicedSprite::UpdateVertices()
	{
		VertexStruct_XYZ_Color_UV* vertices = m_vertices.data();

		std::array<float, 3> heights = {
			m_topLeftCorner.size.y,
			m_size.y - m_topLeftCorner.size.y - m_bottomRightCorner.size.y,
			m_bottomRightCorner.size.y
		};

		std::array<float, 3> widths = {
			m_topLeftCorner.size.x,
			m_size.x - m_topLeftCorner.size.x - m_bottomRightCorner.size.x,
			m_bottomRightCorner.size.x
		};

		std::array<float, 3> texCoordsX = {
			m_topLeftCorner.textureCoords.x * m_textureCoords.width,
			m_textureCoords.width - (m_topLeftCorner.textureCoords.x + m_bottomRightCorner.textureCoords.x) * m_textureCoords.width,
			m_bottomRightCorner.textureCoords.x * m_textureCoords.width
		};

		std::array<float, 3> texCoordsY = {
			m_topLeftCorner.textureCoords.y * m_textureCoords.height,
			m_textureCoords.height - (m_topLeftCorner.textureCoords.y + m_bottomRightCorner.textureCoords.y) * m_textureCoords.height,
			m_bottomRightCorner.textureCoords.y * m_textureCoords.height
		};

		Vector3f originShift = m_origin * m_size;
		Vector3f topLeftCorner = -originShift;
		Vector2f topLeftUV = m_textureCoords.GetCorner<CoordinateSystem::UV>(RectCorner::LeftTop);

		m_spriteCount = 0;
		for (std::size_t y = 0; y < 3; ++y)
		{
			float height = heights[y];
			if (height > 0.f)
			{
				for (std::size_t x = 0; x < 3; ++x)
				{
					float width = widths[x];
					if (width > 0.f)
					{
						vertices->color = m_color;
						vertices->position = topLeftCorner;
						vertices->uv = topLeftUV;
						vertices++;

						vertices->color = m_color;
						vertices->position = topLeftCorner + width * Vector3f::Right();
						vertices->uv = topLeftUV + Vector2f(texCoordsX[x], 0.f);
						vertices++;

						vertices->color = m_color;
						vertices->position = topLeftCorner + height * Vector3f::Up();
						vertices->uv = topLeftUV + Vector2f(0.f, texCoordsY[y]);
						vertices++;

						vertices->color = m_color;
						vertices->position = topLeftCorner + width * Vector3f::Right() + height * Vector3f::Up();
						vertices->uv = topLeftUV + Vector2f(texCoordsX[x], texCoordsY[y]);
						vertices++;

						topLeftCorner.x += width;
						m_spriteCount++;
					}

					topLeftUV.x += texCoordsX[x];
				}

				topLeftCorner.y += height;
			}

			topLeftCorner.x = -originShift.x;

			topLeftUV.x = m_textureCoords.x;
			topLeftUV.y += texCoordsY[y];
		}

		Boxf aabb = Boxf::Zero();

		std::size_t vertexCount = 4 * m_spriteCount;
		if (vertexCount > 0)
		{
			// Reverse texcoords Y
			float yMin = m_textureCoords.GetMinimum().y;
			float yMax = m_textureCoords.GetMaximum().y;
			for (std::size_t i = 0; i < vertexCount; ++i)
				m_vertices[i].uv.y = yMin + yMax - m_vertices[i].uv.y;

			aabb = Boxf(m_vertices[0].position, Vector2f::Zero());
			for (std::size_t i = 1; i < vertexCount; ++i)
				aabb.ExtendTo(m_vertices[i].position);
		}

		UpdateAABB(aabb);
		OnElementInvalidated(this);
	}
}
