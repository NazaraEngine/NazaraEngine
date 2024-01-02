// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_HPP
#define NAZARA_UTILITY_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Font.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/ImageStream.hpp>
#include <Nazara/Utility/Mesh.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API Utility : public ModuleBase<Utility>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Core>;

			struct Config {};

			Utility(Config /*config*/);
			Utility(const Utility&) = delete;
			Utility(Utility&&) = delete;
			~Utility();

			AnimationLoader& GetAnimationLoader();
			const AnimationLoader& GetAnimationLoader() const;
			FontLoader& GetFontLoader();
			const FontLoader& GetFontLoader() const;
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

			Utility& operator=(const Utility&) = delete;
			Utility& operator=(Utility&&) = delete;

		private:
			AnimationLoader m_animationLoader;
			FontLoader m_fontLoader;
			ImageLoader m_imageLoader;
			ImageSaver m_imageSaver;
			ImageStreamLoader m_imageStreamLoader;
			MeshLoader m_meshLoader;
			MeshSaver m_meshSaver;

			static Utility* s_instance;
	};
}

#endif // NAZARA_UTILITY_HPP
