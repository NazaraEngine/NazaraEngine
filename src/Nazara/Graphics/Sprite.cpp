// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/RenderSpriteChain.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	Sprite::Sprite(std::shared_ptr<Material> material) :
	InstancedRenderable(Nz::Boxf(-1000.f, -1000.f, -1000.f, 2000.f, 2000.f, 2000.f)),
	m_material(std::move(material))
	{
		m_vertices[0] = VertexStruct_XYZ_Color_UV{ Vector3f(0.f, 0.f, 0.f), Nz::Color::White, Vector2f(0.f, 0.f) };
		m_vertices[1] = VertexStruct_XYZ_Color_UV{ Vector3f(1.f, 0.f, 0.f), Nz::Color::White, Vector2f(1.f, 0.f) };
		m_vertices[2] = VertexStruct_XYZ_Color_UV{ Vector3f(0.f, 1.f, 0.f), Nz::Color::White, Vector2f(0.f, 1.f) };
		m_vertices[3] = VertexStruct_XYZ_Color_UV{ Vector3f(1.f, 1.f, 0.f), Nz::Color::White, Vector2f(1.f, 1.f) };
	}

	void Sprite::BuildElement(std::size_t passIndex, const WorldInstance& worldInstance, std::vector<std::unique_ptr<RenderElement>>& elements) const
	{
		MaterialPass* materialPass = m_material->GetPass(passIndex);
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

		const auto& whiteTexture = Graphics::Instance()->GetDefaultTextures().whiteTexture2d;

		elements.emplace_back(std::make_unique<RenderSpriteChain>(0, renderPipeline, vertexDeclaration, whiteTexture, 1, m_vertices.data(), materialPass->GetShaderBinding(), worldInstance, materialPass->GetFlags()));
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
}
