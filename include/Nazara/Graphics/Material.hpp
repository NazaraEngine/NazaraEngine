// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIAL_HPP
#define NAZARA_GRAPHICS_MATERIAL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceSaver.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>

namespace Nz
{
	struct NAZARA_GRAPHICS_API MaterialParams : ResourceParameters
	{
		MaterialLightingType lightingType = MaterialLightingType::None;

		bool IsValid() const;
	};

	class Material;
	class MaterialPass;

	using MaterialLibrary = ObjectLibrary<Material>;
	using MaterialLoader = ResourceLoader<Material, MaterialParams>;
	using MaterialManager = ResourceManager<Material, MaterialParams>;
	using MaterialSaver = ResourceSaver<Material, MaterialParams>;

	class NAZARA_GRAPHICS_API Material : public Resource
	{
		public:
			Material() = default;
			~Material() = default;

			inline void AddPass(std::size_t passIndex, std::shared_ptr<MaterialPass> pass);
			void AddPass(std::string passName, std::shared_ptr<MaterialPass> pass);

			const std::shared_ptr<MaterialPass>& FindPass(const std::string& passName) const;

			template<typename F> void ForEachPass(F&& callback);

			inline const std::shared_ptr<MaterialPass>& GetPass(std::size_t passIndex) const;

			inline bool HasPass(std::size_t passIndex) const;

			inline void RemovePass(std::size_t passIndex);
			void RemovePass(const std::string& passName);

			static std::shared_ptr<Material> LoadFromFile(const std::filesystem::path& filePath, const MaterialParams& params = MaterialParams());
			static std::shared_ptr<Material> LoadFromMemory(const void* data, std::size_t size, const MaterialParams& params = MaterialParams());
			static std::shared_ptr<Material> LoadFromStream(Stream& stream, const MaterialParams& params = MaterialParams());

		private:
			std::vector<std::shared_ptr<MaterialPass>> m_passes;
	};
}

#include <Nazara/Graphics/Material.inl>

#endif // NAZARA_GRAPHICS_MATERIAL_HPP
