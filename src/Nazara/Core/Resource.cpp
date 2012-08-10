// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

NzResource::NzResource(bool persistent) :
m_resourcePersistent(persistent),
m_resourceReferenceCount(0)
{
}

NzResource::NzResource(const NzResource& resource) :
m_resourcePersistent(resource.m_resourcePersistent),
m_resourceReferenceCount(0)
{
}

NzResource::~NzResource() = default;

void NzResource::AddResourceReference() const
{
	m_resourceReferenceCount++;
}

bool NzResource::IsPersistent() const
{
	return m_resourcePersistent;
}

void NzResource::RemoveResourceReference() const
{
	#if NAZARA_CORE_SAFE
	if (m_resourceReferenceCount == 0)
	{
		NazaraError("Impossible to remove reference (Ref. counter is already 0)");
		return;
	}
	#endif

	if (--m_resourceReferenceCount == 0 && !m_resourcePersistent)
		delete this;
}

void NzResource::SetPersistent(bool persistent)
{
	m_resourcePersistent = persistent;

	if (!persistent && m_resourceReferenceCount == 0)
		delete this;
}
