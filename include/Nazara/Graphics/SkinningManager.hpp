// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SKINNINGMANAGER_HPP
#define NAZARA_SKINNINGMANAGER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/Config.hpp>

namespace Nz
{
	class Skeleton;
	class SkeletalMesh;
	class VertexBuffer;

	class NAZARA_GRAPHICS_API SkinningManager
	{
		friend class Graphics;

		public:
			using SkinFunction = void (*)(const SkeletalMesh* mesh, const Skeleton* skeleton, VertexBuffer* buffer);

			SkinningManager() = delete;
			~SkinningManager() = delete;

			static VertexBuffer* GetBuffer(const SkeletalMesh* mesh, const Skeleton* skeleton);
			static void Skin();

		private:
			static bool Initialize();
			static void OnSkeletalMeshDestroy(const SkeletalMesh* mesh);
			static void OnSkeletonInvalidated(const Skeleton* skeleton);
			static void OnSkeletonRelease(const Skeleton* skeleton);
			static void Uninitialize();

			static SkinFunction s_skinFunc;
	};
}

#endif // NAZARA_SKINNINGMANAGER_HPP
