// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_HPP
#define NAZARA_GRAPHICS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/TextureSamplerCache.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <optional>

namespace Nz
{
	class AbstractBuffer;
	class RenderDevice;

	class NAZARA_GRAPHICS_API Graphics : public ModuleBase<Graphics>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Renderer>;

			struct Config;

			Graphics(Config config);
			~Graphics();

			inline const std::shared_ptr<RenderDevice>& GetRenderDevice() const;
			inline TextureSamplerCache& GetSamplerCache();
			inline const std::shared_ptr<AbstractBuffer>& GetViewerDataUBO();

			struct Config
			{
				bool useDedicatedRenderDevice = true;
			};

		private:
			std::optional<TextureSamplerCache> m_samplerCache;
			std::shared_ptr<AbstractBuffer> m_viewerDataUBO;
			std::shared_ptr<RenderDevice> m_renderDevice;

			static Graphics* s_instance;
	};
}

#include <Nazara/Graphics/Graphics.inl>

#endif
