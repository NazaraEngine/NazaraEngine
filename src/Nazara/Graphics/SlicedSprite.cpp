// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SlicedSprite.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/RenderSpriteChain.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	SlicedSprite::SlicedSprite(std::shared_ptr<Material> material) :
	m_material(std::move(material)),
	m_color(Color::White),
	m_textureCoords(0.f, 0.f, 1.f, 1.f),
	m_size(64.f, 64.f)
	{
		UpdateVertices();
	}

	void SlicedSprite::BuildElement(std::size_t passIndex, const WorldInstance& worldInstance, std::vector<std::unique_ptr<RenderElement>>& elements) const
	{
		const auto& materialPass = m_material->GetPass(passIndex);
		if (!materialPass)
			return;

		const std::shared_ptr<VertexDeclaration>& vertexDeclaration = VertexDeclaration::Get(VertexLayout::XYZ_Color_UV);

		std::vector<RenderPipelineInfo::VertexBufferData> vertexBufferData = {
			{
				{
					0,
					vertexDeclaration
				}
			}
		};
		const auto& renderPipeline = materialPass->GetPipeline()->GetRenderPipeline(vertexBufferData);

		const auto& whiteTexture = Graphics::Instance()->GetDefaultTextures().whiteTextures[UnderlyingCast(ImageType::E2D)];

		elements.emplace_back(std::make_unique<RenderSpriteChain>(0, materialPass, renderPipeline, worldInstance, vertexDeclaration, whiteTexture, m_spriteCount, m_vertices.data()));
	}

	const std::shared_ptr<Material>& SlicedSprite::GetMaterial(std::size_t i) const
	{
		assert(i == 0);
		NazaraUnused(i);

		return m_material;
	}

	std::size_t SlicedSprite::GetMaterialCount() const
	{
		return 1;
	}

	inline auto SlicedSprite::GetTopLeftCorner() const -> const Corner&
	{
		return m_topLeftCorner;
	}

	Vector3ui SlicedSprite::GetTextureSize() const
	{
		assert(m_material);

		//TODO: Cache index in registry?
		if (const auto& material = m_material->FindPass("ForwardPass"))
		{
			BasicMaterial mat(*material);
			if (mat.HasDiffuseMap())
			{
				// Material should always have textures but we're better safe than sorry
				if (const auto& texture = mat.GetDiffuseMap())
					return mat.GetDiffuseMap()->GetSize();
			}
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
			m_textureCoords.width - m_topLeftCorner.textureCoords.x * m_textureCoords.width - m_bottomRightCorner.textureCoords.x * m_textureCoords.width,
			m_bottomRightCorner.textureCoords.x * m_textureCoords.width
		};

		std::array<float, 3> texCoordsY = {
			m_topLeftCorner.textureCoords.y * m_textureCoords.height,
			m_textureCoords.height - m_topLeftCorner.textureCoords.y * m_textureCoords.height - m_bottomRightCorner.textureCoords.y * m_textureCoords.height,
			m_bottomRightCorner.textureCoords.y * m_textureCoords.height
		};

		Vector3f origin = Vector3f::Zero();
		Vector2f topLeftUV = m_textureCoords.GetCorner(RectCorner::LeftTop);

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
						vertices->position = origin;
						vertices->uv = topLeftUV;
						vertices++;

						vertices->color = m_color;
						vertices->position = origin + width * Vector3f::Right();
						vertices->uv = topLeftUV + Vector2f(texCoordsX[x], 0.f);
						vertices++;

						vertices->color = m_color;
						vertices->position = origin + height * Vector3f::Up();
						vertices->uv = topLeftUV + Vector2f(0.f, texCoordsY[y]);
						vertices++;

						vertices->color = m_color;
						vertices->position = origin + width * Vector3f::Right() + height * Vector3f::Up();
						vertices->uv = topLeftUV + Vector2f(texCoordsX[x], texCoordsY[y]);
						vertices++;

						origin.x += width;
						m_spriteCount++;
					}

					topLeftUV.x += texCoordsX[x];
				}

				origin.y += height;
			}

			origin.x = 0;

			topLeftUV.x = m_textureCoords.x;
			topLeftUV.y += texCoordsY[y];
		}

		Boxf aabb = Boxf::Zero();

		std::size_t vertexCount = 4 * m_spriteCount;
		if (vertexCount > 0)
		{
			// Reverse texcoords Y
			for (std::size_t i = 0; i < vertexCount; ++i)
				m_vertices[i].uv.y = m_textureCoords.height - m_vertices[i].uv.y;

			aabb.Set(m_vertices[0].position);
			for (std::size_t i = 1; i < vertexCount; ++i)
				aabb.ExtendTo(m_vertices[i].position);
		}

		UpdateAABB(aabb);
		OnElementInvalidated(this);
	}
}
