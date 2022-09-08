// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const std::shared_ptr<RenderPipeline>& Graphics::GetBlitPipeline(bool transparent) const
	{
		return (transparent) ? m_blitPipelineTransparent : m_blitPipeline;
	}

	inline const std::shared_ptr<RenderPipelineLayout>& Graphics::GetBlitPipelineLayout() const
	{
		return m_blitPipelineLayout;
	}

	inline auto Graphics::GetDefaultMaterials() const -> const DefaultMaterials&
	{
		return m_defaultMaterials;
	}

	inline auto Graphics::GetDefaultTextures() const -> const DefaultTextures&
	{
		return m_defaultTextures;
	}

	inline MaterialPassRegistry& Graphics::GetMaterialPassRegistry()
	{
		return m_materialPassRegistry;
	}

	inline const MaterialPassRegistry& Graphics::GetMaterialPassRegistry() const
	{
		return m_materialPassRegistry;
	}

	inline MaterialLoader& Graphics::GetMaterialLoader()
	{
		return m_materialLoader;
	}

	inline const MaterialLoader& Graphics::GetMaterialLoader() const
	{
		return m_materialLoader;
	}

	inline PixelFormat Graphics::GetPreferredDepthStencilFormat() const
	{
		return m_preferredDepthStencilFormat;
	}

	inline const std::shared_ptr<RenderDevice>& Graphics::GetRenderDevice() const
	{
		return m_renderDevice;
	}

	inline const RenderPassCache& Graphics::GetRenderPassCache() const
	{
		assert(m_renderPassCache);
		return *m_renderPassCache;
	}

	inline TextureSamplerCache& Graphics::GetSamplerCache()
	{
		return *m_samplerCache;
	}

	inline const std::shared_ptr<nzsl::FilesystemModuleResolver>& Graphics::GetShaderModuleResolver() const
	{
		return m_shaderModuleResolver;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
