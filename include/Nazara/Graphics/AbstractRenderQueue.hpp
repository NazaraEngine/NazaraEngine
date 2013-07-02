// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTRENDERQUEUE_HPP
#define NAZARA_ABSTRACTRENDERQUEUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>

class NzDrawable;
class NzLight;
class NzModel;

class NAZARA_API NzAbstractRenderQueue : NzNonCopyable
{
	public:
		NzAbstractRenderQueue() = default;
		virtual ~NzAbstractRenderQueue();

		virtual void AddDrawable(const NzDrawable* drawable) = 0;
		virtual void AddLight(const NzLight* light) = 0;
		virtual void AddModel(const NzModel* model) = 0;

		virtual void Clear(bool fully) = 0;
};

#endif // NAZARA_ABSTRACTRENDERQUEUE_HPP
