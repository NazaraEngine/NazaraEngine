// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <stdexcept>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::Graphics
	* \brief Graphics class that represents the module initializer of Graphics
	*/
	Graphics::Graphics(Config config) :
	ModuleBase("Graphics", this)
	{
		Renderer* renderer = Renderer::Instance();
		RendererImpl* rendererImpl = renderer->GetRendererImpl(); //< FIXME
		std::vector<RenderDeviceInfo> renderDeviceInfo = rendererImpl->QueryRenderDevices();
		if (renderDeviceInfo.empty())
			throw std::runtime_error("no render device available");

		std::size_t bestRenderDeviceIndex = 0;
		for (std::size_t i = 0; i < renderDeviceInfo.size(); ++i)
		{
			const auto& deviceInfo = renderDeviceInfo[i];
			if (config.useDedicatedRenderDevice && deviceInfo.type == RenderDeviceType::Dedicated)
			{
				bestRenderDeviceIndex = i;
				break;
			}
		}

		m_renderDevice = rendererImpl->InstanciateRenderDevice(bestRenderDeviceIndex);
		if (!m_renderDevice)
			throw std::runtime_error("failed to instantiate render device");

		MaterialPipeline::Initialize();

		Nz::PredefinedViewerData viewerUboOffsets = Nz::PredefinedViewerData::GetOffsets();

		m_viewerDataUBO = m_renderDevice->InstantiateBuffer(Nz::BufferType_Uniform);
		if (!m_viewerDataUBO->Initialize(viewerUboOffsets.totalSize, Nz::BufferUsage_DeviceLocal | Nz::BufferUsage_Dynamic))
			throw std::runtime_error("failed to initialize viewer data UBO");
	}

	Graphics::~Graphics()
	{
		MaterialPipeline::Uninitialize();
	}

	Graphics* Graphics::s_instance = nullptr;
}
