// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/RenderSpriteChain.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	Sprite::Sprite(std::shared_ptr<Material> material) :
	m_material(std::move(material)),
	m_color(Color::White),
	m_textureCoords(0.f, 0.f, 1.f, 1.f),
	m_size(64.f, 64.f),
	m_origin(0.f, 0.f, 0.f)
	{
		m_cornerColor.fill(Color::White);

		UpdateVertices();
	}

	void Sprite::BuildElement(std::size_t passIndex, const WorldInstance& worldInstance, const SkeletonInstance* skeletonInstance, std::vector<std::unique_ptr<RenderElement>>& elements, const Recti& scissorBox) const
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

		elements.emplace_back(std::make_unique<RenderSpriteChain>(GetRenderLayer(), materialPass, renderPipeline, worldInstance, vertexDeclaration, whiteTexture, 1, m_vertices.data(), scissorBox));
	}

	const std::shared_ptr<Material>& Sprite::GetMaterial(std::size_t i) const
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
		if (const auto& material = m_material->FindPass("ForwardPass"))
		{
			BasicMaterial mat(*material);
			if (mat.HasBaseColorMap())
			{
				// Material should always have textures but we're better safe than sorry
				if (const auto& texture = mat.GetBaseColorMap())
					return texture->GetSize();
			}
		}

		// Couldn't get material pass or texture
		return Vector3ui::Unit(); //< prevents division by zero
	}
}
