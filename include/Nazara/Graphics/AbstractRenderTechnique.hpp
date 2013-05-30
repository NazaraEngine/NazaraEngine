// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTRENDERTECHNIQUE_HPP
#define NAZARA_ABSTRACTRENDERTECHNIQUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>

class NzBackground;
class NzScene;

class NAZARA_API NzAbstractRenderTechnique : NzNonCopyable
{
	public:
		NzAbstractRenderTechnique() = default;
		virtual ~NzAbstractRenderTechnique();

		virtual void Clear(const NzScene* scene) = 0;
		virtual void Draw(const NzScene* scene) = 0;

		virtual NzAbstractRenderQueue* GetRenderQueue() = 0;
};

#endif // NAZARA_ABSTRACTRENDERTECHNIQUE_HPP
