// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERTEXTURE_HPP
#define NAZARA_GRAPHICS_RENDERTEXTURE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/RenderTarget.hpp>
#include <Nazara/Renderer/Enums.hpp>

namespace Nz
{
	class Texture;

	class NAZARA_GRAPHICS_API RenderTexture : public RenderTarget
	{
		public:
			inline RenderTexture(std::shared_ptr<Texture> targetTexture);
			RenderTexture(std::shared_ptr<Texture> targetTexture, PipelineStage targetPipelineStage, MemoryAccessFlags targetMemoryFlags, TextureLayout targetLayout);
			RenderTexture(const RenderTexture&) = delete;
			RenderTexture(RenderTexture&&) = delete;
			~RenderTexture() = default;

			void OnBuildGraph(FrameGraph& graph, std::size_t attachmentIndex) const override;
			void OnRenderEnd(RenderFrame& renderFrame, const BakedFrameGraph& frameGraph, std::size_t finalAttachment) const override;

			const Vector2ui& GetSize() const override;

			RenderTexture& operator=(const RenderTexture&) = delete;
			RenderTexture& operator=(RenderTexture&&) = delete;

		private:
			std::shared_ptr<Texture> m_targetTexture;
			MemoryAccessFlags m_targetMemoryFlags;
			PipelineStage m_targetPipelineStage;
			TextureLayout m_targetLayout;
			Vector2ui m_textureSize;
	};
}

#include <Nazara/Graphics/RenderTexture.inl>

#endif // NAZARA_GRAPHICS_RENDERTEXTURE_HPP
