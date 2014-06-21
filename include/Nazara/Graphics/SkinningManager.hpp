// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SKINNINGMANAGER_HPP
#define NAZARA_SKINNINGMANAGER_HPP

#include <Nazara/Prerequesites.hpp>

class NzSkeleton;
class NzSkeletalMesh;
class NzVertexBuffer;

class NAZARA_API NzSkinningManager
{
	friend class NzGraphics;

	public:
		using SkinFunction = void (*)(const NzSkeletalMesh* mesh, const NzSkeleton* skeleton, NzVertexBuffer* buffer);

		NzSkinningManager() = delete;
		~NzSkinningManager() = delete;

		static NzVertexBuffer* GetBuffer(const NzSkeletalMesh* mesh, const NzSkeleton* skeleton);
		static void Skin();

	private:
		static bool Initialize();
		static void Uninitialize();

		static SkinFunction s_skinFunc;
};

#endif // NAZARA_SKINNINGMANAGER_HPP
