// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SCENELAYER_HPP
#define NAZARA_SCENELAYER_HPP

#include <Nazara/Prerequesites.hpp>

class NzAbstractRenderTechnique;
class NzAbstractViewer;

class NAZARA_API NzSceneLayer
{
	public:
		NzSceneLayer();
		~NzSceneLayer();

		void Draw();

		nzUInt32 GetBufferClearFlags() const;
		NzAbstractRenderQueue* GetRenderQueue() const;
		NzAbstractRenderTechnique* GetRenderTechnique() const;
		NzScene* GetScene() const;
		NzAbstractViewer* GetViewer() const;

		void SetBufferClearFlags(nzUInt32 flags);
		void SetRenderTechnique(NzAbstractRenderTechnique* renderTechnique);
		void SetViewer(NzAbstractViewer* viewer);
};

#endif // NAZARA_SCENELAYER_HPP
