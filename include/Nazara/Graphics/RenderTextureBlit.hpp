// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERTEXTUREBLIT_HPP
#define NAZARA_GRAPHICS_RENDERTEXTUREBLIT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/RenderTarget.hpp>
#include <Nazara/Renderer/Enums.hpp>

namespace Nz
{
	class Texture;

	class NAZARA_GRAPHICS_API RenderTextureBlit : public RenderTarget
	{
		public:
			inline RenderTextureBlit(const Vector2ui& textureSize, std::shared_ptr<Texture> targetTexture, SamplerFilter filter = SamplerFilter::Linear);
			inline RenderTextureBlit(const Vector2ui& textureSize, std::shared_ptr<Texture> targetTexture, SamplerFilter filter, PipelineStage targetPipelineStage, MemoryAccessFlags targetMemoryFlags, TextureLayout targetLayout);
			RenderTextureBlit(const RenderTextureBlit&) = delete;
			RenderTextureBlit(RenderTextureBlit&&) = delete;
			~RenderTextureBlit() = default;

			std::size_t OnBuildGraph(FrameGraph& graph, std::size_t attachmentIndex) const override;

			const Vector2ui& GetSize() const override;

			RenderTextureBlit& operator=(const RenderTextureBlit&) = delete;
			RenderTextureBlit& operator=(RenderTextureBlit&&) = delete;

		private:
			std::shared_ptr<Texture> m_targetTexture;
			MemoryAccessFlags m_targetMemoryFlags;
			PipelineStage m_targetPipelineStage;
			SamplerFilter m_samplerFilter;
			TextureLayout m_targetLayout;
			Vector2ui m_textureSize;
	};
}

#include <Nazara/Graphics/RenderTextureBlit.inl>

#endif // NAZARA_GRAPHICS_RENDERTEXTUREBLIT_HPP
