// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_HPP
#define NAZARA_GRAPHICS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/MaterialPassRegistry.hpp>
#include <Nazara/Graphics/TextureSamplerCache.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Renderer/RenderPassCache.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <optional>

namespace Nz
{
	class AbstractBuffer;

	class NAZARA_GRAPHICS_API Graphics : public ModuleBase<Graphics>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Renderer>;

			struct Config;
			struct DefaultTextures;

			Graphics(Config config);
			~Graphics();

			inline const std::shared_ptr<RenderPipeline>& GetBlitPipeline() const;
			inline const std::shared_ptr<RenderPipelineLayout>& GetBlitPipelineLayout() const;
			inline const DefaultTextures& GetDefaultTextures() const;
			inline const std::shared_ptr<AbstractBuffer>& GetFullscreenVertexBuffer() const;
			inline const std::shared_ptr<VertexDeclaration>& GetFullscreenVertexDeclaration() const;
			inline MaterialPassRegistry& GetMaterialPassRegistry();
			inline const MaterialPassRegistry& GetMaterialPassRegistry() const;
			inline PixelFormat GetPreferredDepthStencilFormat() const;
			inline const std::shared_ptr<RenderPipelineLayout>& GetReferencePipelineLayout() const;
			inline const std::shared_ptr<RenderDevice>& GetRenderDevice() const;
			inline const RenderPassCache& GetRenderPassCache() const;
			inline TextureSamplerCache& GetSamplerCache();

			struct Config
			{
				RenderDeviceFeatures forceDisableFeatures;
				bool useDedicatedRenderDevice = true;
			};

			struct DefaultTextures
			{
				std::shared_ptr<Texture> whiteTexture2d;
			};

			static constexpr UInt32 DrawDataBindingSet = 2;
			static constexpr UInt32 MaterialBindingSet = 3;
			static constexpr UInt32 ViewerBindingSet = 0;
			static constexpr UInt32 WorldBindingSet = 1;

			static void FillDrawDataPipelineLayout(RenderPipelineLayoutInfo& layoutInfo, UInt32 set = DrawDataBindingSet);
			static void FillViewerPipelineLayout(RenderPipelineLayoutInfo& layoutInfo, UInt32 set = ViewerBindingSet);
			static void FillWorldPipelineLayout(RenderPipelineLayoutInfo& layoutInfo, UInt32 set = WorldBindingSet);

		private:
			void BuildBlitPipeline();
			void BuildDefaultTextures();
			void BuildFullscreenVertexBuffer();
			void RegisterMaterialPasses();
			void SelectDepthStencilFormats();

			std::optional<RenderPassCache> m_renderPassCache;
			std::optional<TextureSamplerCache> m_samplerCache;
			std::shared_ptr<AbstractBuffer> m_fullscreenVertexBuffer;
			std::shared_ptr<RenderDevice> m_renderDevice;
			std::shared_ptr<RenderPipeline> m_blitPipeline;
			std::shared_ptr<RenderPipelineLayout> m_blitPipelineLayout;
			std::shared_ptr<RenderPipelineLayout> m_referencePipelineLayout;
			std::shared_ptr<VertexDeclaration> m_fullscreenVertexDeclaration;
			DefaultTextures m_defaultTextures;
			MaterialPassRegistry m_materialPassRegistry;
			PixelFormat m_preferredDepthStencilFormat;

			static Graphics* s_instance;
	};
}

#include <Nazara/Graphics/Graphics.inl>

#endif // NAZARA_GRAPHICS_HPP
