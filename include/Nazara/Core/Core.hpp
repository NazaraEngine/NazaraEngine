// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_HPP
#define NAZARA_CORE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Animation.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/Image.hpp>
#include <Nazara/Core/ImageStream.hpp>
#include <Nazara/Core/Mesh.hpp>
#include <Nazara/Core/ModuleBase.hpp>
#include <Nazara/Core/Modules.hpp>
#include <NazaraUtils/TypeList.hpp>
#include <optional>

namespace Nz
{
	class NAZARA_CORE_API Core : public ModuleBase<Core>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<>;

			struct Config {};

			Core(Config /*config*/);
			~Core();

			AnimationLoader& GetAnimationLoader();
			const AnimationLoader& GetAnimationLoader() const;
			inline const HardwareInfo& GetHardwareInfo() const;
			ImageLoader& GetImageLoader();
			const ImageLoader& GetImageLoader() const;
			ImageSaver& GetImageSaver();
			const ImageSaver& GetImageSaver() const;
			ImageStreamLoader& GetImageStreamLoader();
			const ImageStreamLoader& GetImageStreamLoader() const;
			MeshLoader& GetMeshLoader();
			const MeshLoader& GetMeshLoader() const;
			MeshSaver& GetMeshSaver();
			const MeshSaver& GetMeshSaver() const;

		private:
			std::optional<HardwareInfo> m_hardwareInfo;
			AnimationLoader m_animationLoader;
			ImageLoader m_imageLoader;
			ImageSaver m_imageSaver;
			ImageStreamLoader m_imageStreamLoader;
			MeshLoader m_meshLoader;
			MeshSaver m_meshSaver;

			static Core* s_instance;
	};
}

#include <Nazara/Core/Core.inl>

#endif // NAZARA_CORE_HPP
