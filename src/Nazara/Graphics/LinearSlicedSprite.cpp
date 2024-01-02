// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/LinearSlicedSprite.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/RenderSpriteChain.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	LinearSlicedSprite::LinearSlicedSprite(std::shared_ptr<MaterialInstance> material, Orientation orientation) :
	m_material(std::move(material)),
	m_sectionCount(0),
	m_spriteCount(0),
	m_color(Color::White()),
	m_orientation(orientation),
	m_textureCoords(0.f, 0.f, 1.f, 1.f),
	m_origin(0.f, 0.f)
	{
		Vector2ui size = Vector2ui(GetTextureSize());
		m_size = SafeCast<float>((orientation == Orientation::Horizontal) ? size.x : size.y);

		UpdateVertices();
	}

	void LinearSlicedSprite::BuildElement(ElementRendererRegistry& registry, const ElementData& elementData, std::size_t passIndex, std::vector<RenderElementOwner>& elements) const
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

	const std::shared_ptr<MaterialInstance>& LinearSlicedSprite::GetMaterial(std::size_t i) const
	{
		assert(i == 0);
		NazaraUnused(i);

		return m_material;
	}

	std::size_t LinearSlicedSprite::GetMaterialCount() const
	{
		return 1;
	}

	Vector3ui LinearSlicedSprite::GetTextureSize() const
	{
		assert(m_material);

		// TODO: Move this in a separate function
		if (const std::shared_ptr<Texture>* textureOpt = m_material->GetTextureProperty("BaseColorMap"))
		{
			// Material should always have textures but we're better safe than sorry
			if (const std::shared_ptr<Texture>& texture = *textureOpt)
				return texture->GetSize();
		}

		return Vector3ui::Unit(); //< prevents division by zero
	}

	void LinearSlicedSprite::UpdateVertices()
	{
		float totalSectionSize = 0.f;
		for (const Section& section : m_sections)
			totalSectionSize += std::max(section.size, 0.f);

		Vector2f originShift = m_origin * ((m_orientation == Orientation::Horizontal) ? Vector2f(totalSectionSize, m_size) : Vector2f(m_size, totalSectionSize));

		Vector3f topLeftCorner = -originShift;
		Vector2f topLeftUV = m_textureCoords.GetCorner(RectCorner::LeftTop);

		m_spriteCount = 0;

		VertexStruct_XYZ_Color_UV* vertices = m_vertices.data();
		for (const Section& section : m_sections)
		{
			if (section.size <= 0.f)
				continue;

			Vector2f dir;
			Vector2f size;
			Vector2f texCoords;
			if (m_orientation == Orientation::Horizontal)
			{
				dir = Vector2(1.f, 0.f);
				size = Vector2f(section.size, m_size);
				texCoords = Vector2f(section.textureCoord, m_textureCoords.height);
			}
			else
			{
				dir = Vector2(0.f, 1.f);
				size = Vector2f(m_size, section.size);
				texCoords = Vector2f(m_textureCoords.width, section.textureCoord);
			}
			
			vertices->color = m_color;
			vertices->position = topLeftCorner;
			vertices->uv = topLeftUV;
			vertices++;

			vertices->color = m_color;
			vertices->position = topLeftCorner + size.x * Vector3f::Right();
			vertices->uv = topLeftUV + Vector2f(texCoords.x, 0.f);
			vertices++;

			vertices->color = m_color;
			vertices->position = topLeftCorner + size.y * Vector3f::Up();
			vertices->uv = topLeftUV + Vector2f(0.f, texCoords.y);
			vertices++;

			vertices->color = m_color;
			vertices->position = topLeftCorner + size.x * Vector3f::Right() + size.y * Vector3f::Up();
			vertices->uv = topLeftUV + Vector2f(texCoords.x, texCoords.y);
			vertices++;

			topLeftCorner += dir * section.size;
			topLeftUV += dir * section.textureCoord;
			m_spriteCount++;
		}

		Boxf aabb = Boxf::Zero();

		std::size_t vertexCount = 4 * m_spriteCount;
		if (vertexCount > 0)
		{
			// Reverse texcoords Y
			for (std::size_t i = 0; i < vertexCount; ++i)
				m_vertices[i].uv.y = m_textureCoords.height - m_vertices[i].uv.y;

			aabb = Boxf(m_vertices[0].position, Vector3f::Zero());
			for (std::size_t i = 1; i < vertexCount; ++i)
				aabb.ExtendTo(m_vertices[i].position);
		}

		UpdateAABB(aabb);
		OnElementInvalidated(this);
	}
}
