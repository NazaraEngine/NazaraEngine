// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NETWORK_HPP
#define NAZARA_NETWORK_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Network/Config.hpp>
#include <memory>

namespace Nz
{
	class WebService;

	class NAZARA_NETWORK_API Network : public ModuleBase<Network>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Core>;

			struct Config;

			Network(Config config);
			~Network();

			std::unique_ptr<WebService> InstantiateWebService();

			struct Config
			{
				// Initialize web services and fails module initialization if it failed to initialize them
				bool webServices = false;
			};

		private:
#ifndef NAZARA_PLATFORM_WEB
			std::unique_ptr<class CurlLibrary> m_curlLibrary;
#endif

			static Network* s_instance;
	};
}

#endif // NAZARA_NETWORK_HPP
