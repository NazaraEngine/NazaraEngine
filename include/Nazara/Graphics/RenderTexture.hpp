// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERTEXTURE_HPP
#define NAZARA_GRAPHICS_RENDERTEXTURE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/RenderTarget.hpp>
#include <Nazara/Renderer/Enums.hpp>

namespace Nz
{
	class Texture;

	class NAZARA_GRAPHICS_API RenderTexture : public RenderTarget
	{
		public:
			RenderTexture(std::shared_ptr<Texture> targetTexture);
			RenderTexture(const RenderTexture&) = delete;
			RenderTexture(RenderTexture&&) = delete;
			~RenderTexture() = default;

			std::size_t OnBuildGraph(FrameGraph& graph, std::size_t attachmentIndex) const override;

			const Vector2ui& GetSize() const override;

			RenderTexture& operator=(const RenderTexture&) = delete;
			RenderTexture& operator=(RenderTexture&&) = delete;

		private:
			std::shared_ptr<Texture> m_targetTexture;
			Vector2ui m_textureSize;
	};
}

#include <Nazara/Graphics/RenderTexture.inl>

#endif // NAZARA_GRAPHICS_RENDERTEXTURE_HPP
