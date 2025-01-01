// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_WIDGETS_HPP
#define NAZARA_WIDGETS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ModuleBase.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Widgets/Export.hpp>
#include <entt/entt.hpp>

namespace Nz
{
	class Material;

	class NAZARA_WIDGETS_API Widgets : public ModuleBase<Widgets>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Graphics>;

			struct Config;

			Widgets(Config config);
			~Widgets() = default;

			inline const std::shared_ptr<MaterialInstance>& GetOpaqueMaterial() const;
			inline const std::shared_ptr<MaterialInstance>& GetTransparentMaterial() const;

			struct Config {};

		private:
			void CreateDefaultMaterials();

			std::shared_ptr<MaterialInstance> m_opaqueMaterial;
			std::shared_ptr<MaterialInstance> m_transparentMaterial;

			static Widgets* s_instance;
	};
}

#include <Nazara/Widgets/Widgets.inl>

#endif // NAZARA_WIDGETS_HPP
