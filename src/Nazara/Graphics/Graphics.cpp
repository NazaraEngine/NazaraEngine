// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Core/ECS.hpp>
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
		ECS::RegisterComponents();

		Renderer* renderer = Renderer::Instance();

		const std::vector<RenderDeviceInfo>& renderDeviceInfo = renderer->QueryRenderDevices();
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
			else if (!config.useDedicatedRenderDevice && deviceInfo.type == RenderDeviceType::Integrated)
			{
				bestRenderDeviceIndex = i;
				break;
			}
		}

		RenderDeviceFeatures enabledFeatures;
		enabledFeatures.anisotropicFiltering = renderDeviceInfo[bestRenderDeviceIndex].features.anisotropicFiltering;
		enabledFeatures.nonSolidFaceFilling = renderDeviceInfo[bestRenderDeviceIndex].features.nonSolidFaceFilling;

		m_renderDevice = renderer->InstanciateRenderDevice(bestRenderDeviceIndex, enabledFeatures);
		if (!m_renderDevice)
			throw std::runtime_error("failed to instantiate render device");

		m_samplerCache.emplace(m_renderDevice);

		MaterialPipeline::Initialize();

		RenderPipelineLayoutInfo referenceLayoutInfo;
		FillViewerPipelineLayout(referenceLayoutInfo);
		FillWorldPipelineLayout(referenceLayoutInfo);

		m_referencePipelineLayout = m_renderDevice->InstantiateRenderPipelineLayout(std::move(referenceLayoutInfo));
	}

	Graphics::~Graphics()
	{
		MaterialPipeline::Uninitialize();
	}

	void Graphics::FillViewerPipelineLayout(RenderPipelineLayoutInfo& layoutInfo, UInt32 set)
	{
		layoutInfo.bindings.push_back({
			set, 0,
			ShaderBindingType::UniformBuffer,
			ShaderStageType_All
		});
	}

	void Graphics::FillWorldPipelineLayout(RenderPipelineLayoutInfo& layoutInfo, UInt32 set)
	{
		layoutInfo.bindings.push_back({
			set, 0,
			ShaderBindingType::UniformBuffer,
			ShaderStageType_All
		});
	}

	Graphics* Graphics::s_instance = nullptr;
}
