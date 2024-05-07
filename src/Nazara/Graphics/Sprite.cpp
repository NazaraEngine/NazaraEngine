// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/ElementRendererRegistry.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/RenderSpriteChain.hpp>
#include <Nazara/Graphics/TextureAsset.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>

namespace Nz
{
	Sprite::Sprite(std::shared_ptr<MaterialInstance> material) :
	m_material(std::move(material)),
	m_color(Color::White()),
	m_textureCoords(0.f, 0.f, 1.f, 1.f),
	m_origin(0.f, 0.f)
	{
		m_cornerColor.fill(Color::White());
		m_size = Vector2f(Vector2ui(GetTextureSize()));

		UpdateVertices();
	}

	void Sprite::BuildElement(ElementRendererRegistry& registry, const ElementData& elementData, std::size_t passIndex, std::vector<RenderElementOwner>& elements) const
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

		elements.emplace_back(registry.AllocateElement<RenderSpriteChain>(GetRenderLayer(), m_material, passFlags, renderPipeline, *elementData.worldInstance, vertexDeclaration, whiteTexture, 1, m_vertices.data(), *elementData.scissorBox));
	}

	const std::shared_ptr<MaterialInstance>& Sprite::GetMaterial(std::size_t i) const
	{
		assert(i == 0);
		NazaraUnused(i);

		return m_material;
	}

	std::size_t Sprite::GetMaterialCount() const
	{
		return 1;
	}

	Vector3ui Sprite::GetTextureSize() const
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
}
