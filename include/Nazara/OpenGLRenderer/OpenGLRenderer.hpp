// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_HPP
#define NAZARA_OPENGLRENDERER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <Nazara/Renderer/RendererImpl.hpp>
#include <memory>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLRenderer : public RendererImpl
	{
		public:
			OpenGLRenderer() = default;
			~OpenGLRenderer();

			std::shared_ptr<RenderDevice> InstanciateRenderDevice(std::size_t deviceIndex, const RenderDeviceFeatures& enabledFeatures) override;

			RenderAPI QueryAPI() const override;
			std::string QueryAPIString() const override;
			UInt32 QueryAPIVersion() const override;
			const std::vector<RenderDeviceInfo>& QueryRenderDevices() const override;

			bool Prepare(const Renderer::Config& config) override;

		private:
			std::unique_ptr<GL::Loader> SelectLoader(const Renderer::Config& config);

			std::shared_ptr<OpenGLDevice> m_device;
			std::unique_ptr<GL::Loader> m_loader;
			std::vector<RenderDeviceInfo> m_deviceInfos;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLRenderer.inl>

#endif // NAZARA_OPENGLRENDERER_HPP
