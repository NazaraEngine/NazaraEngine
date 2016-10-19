// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/CullingList.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	template<typename T>
	CullingList<T>::~CullingList()
	{
		OnCullingListRelease(this);
	}

	template<typename T>
	std::size_t CullingList<T>::Cull(const Frustumf& frustum, bool* forceInvalidation)
	{
		m_results.clear();

		bool forcedInvalidation = false;

		std::size_t visibleHash = 0U;

		for (NoTestVisibilityEntry& entry : m_noTestList)
		{
			m_results.push_back(entry.renderable);
			Nz::HashCombine(visibleHash, entry.renderable);

			if (entry.forceInvalidation)
			{
				forcedInvalidation = true;
				entry.forceInvalidation = false;
			}
		}

		for (SphereVisibilityEntry& entry : m_sphereTestList)
		{
			if (frustum.Contains(entry.sphere))
			{
				m_results.push_back(entry.renderable);
				Nz::HashCombine(visibleHash, entry.renderable);

				if (entry.forceInvalidation)
				{
					forcedInvalidation = true;
					entry.forceInvalidation = false;
				}
			}
		}

		for (VolumeVisibilityEntry& entry : m_volumeTestList)
		{
			if (frustum.Contains(entry.volume))
			{
				m_results.push_back(entry.renderable);
				Nz::HashCombine(visibleHash, entry.renderable);

				if (entry.forceInvalidation)
				{
					forcedInvalidation = true;
					entry.forceInvalidation = false;
				}
			}
		}

		if (forceInvalidation)
			*forceInvalidation = forcedInvalidation;

		return visibleHash;
	}

	template<typename T>
	typename CullingList<T>::NoTestEntry CullingList<T>::RegisterNoTest(const T* renderable)
	{
		NoTestEntry entry(this, m_noTestList.size());
		m_noTestList.emplace_back(NoTestVisibilityEntry{&entry, renderable, false}); //< Address of entry will be updated when moving

		return entry;
	}

	template<typename T>
	typename CullingList<T>::SphereEntry CullingList<T>::RegisterSphereTest(const T* renderable)
	{
		SphereEntry entry(this, m_sphereTestList.size());
		m_sphereTestList.emplace_back(SphereVisibilityEntry{Nz::Spheref(), &entry, renderable, false}); //< Address of entry will be updated when moving

		return entry;
	}

	template<typename T>
	typename CullingList<T>::VolumeEntry CullingList<T>::RegisterVolumeTest(const T* renderable)
	{
		VolumeEntry entry(this, m_volumeTestList.size());
		m_volumeTestList.emplace_back(VolumeVisibilityEntry{Nz::BoundingVolumef(), &entry, renderable, false}); //< Address of entry will be updated when moving

		return entry;
	}

	// Interface STD
	template<typename T>
	typename CullingList<T>::ResultContainer::iterator CullingList<T>::begin()
	{
		return m_results.begin();
	}

	template<typename T>
	typename CullingList<T>::ResultContainer::const_iterator CullingList<T>::begin() const
	{
		return m_results.begin();
	}

	template<typename T>
	typename CullingList<T>::ResultContainer::const_iterator CullingList<T>::cbegin() const
	{
		return m_results.cbegin();
	}

	template<typename T>
	typename CullingList<T>::ResultContainer::const_iterator CullingList<T>::cend() const
	{
		return m_results.cend();
	}

	template<typename T>
	typename CullingList<T>::ResultContainer::const_reverse_iterator CullingList<T>::crbegin() const
	{
		return m_results.crbegin();
	}

	template<typename T>
	typename CullingList<T>::ResultContainer::const_reverse_iterator CullingList<T>::crend() const
	{
		return m_results.crend();
	}

	template<typename T>
	bool CullingList<T>::empty() const
	{
		return m_results.empty();
	}

	template<typename T>
	typename CullingList<T>::ResultContainer::iterator CullingList<T>::end()
	{
		return m_results.end();
	}

	template<typename T>
	typename CullingList<T>::ResultContainer::const_iterator CullingList<T>::end() const
	{
		return m_results.end();
	}

	template<typename T>
	typename CullingList<T>::ResultContainer::reverse_iterator CullingList<T>::rbegin()
	{
		return m_results.rbegin();
	}

	template<typename T>
	typename CullingList<T>::ResultContainer::const_reverse_iterator CullingList<T>::rbegin() const
	{
		return m_results.rbegin();
	}

	template<typename T>
	typename CullingList<T>::ResultContainer::reverse_iterator CullingList<T>::rend()
	{
		return m_results.rend();
	}

	template<typename T>
	typename CullingList<T>::ResultContainer::const_reverse_iterator CullingList<T>::rend() const
	{
		return m_results.rend();
	}

	template<typename T>
	typename CullingList<T>::ResultContainer::size_type CullingList<T>::size() const
	{
		return m_results.size();
	}

	template<typename T>
	void CullingList<T>::NotifyForceInvalidation(CullTest type, std::size_t index)
	{
		switch (type)
		{
			case CullTest::NoTest:
			{
				m_noTestList[index].forceInvalidation = true;
				break;
			}

			case CullTest::Sphere:
			{
				m_sphereTestList[index].forceInvalidation = true;
				break;
			}

			case CullTest::Volume:
			{
				m_volumeTestList[index].forceInvalidation = true;
				break;
			}

			default:
				NazaraInternalError("Unhandled culltype");
				break;
		}
	}

	template<typename T>
	void CullingList<T>::NotifyMovement(CullTest type, std::size_t index, void* oldPtr, void* newPtr)
	{
		switch (type)
		{
			case CullTest::NoTest:
			{
				NoTestVisibilityEntry& entry = m_noTestList[index];
				NazaraAssert(entry.entry == oldPtr, "Invalid entry");

				entry.entry = static_cast<NoTestEntry*>(newPtr);
				break;
			}

			case CullTest::Sphere:
			{
				SphereVisibilityEntry& entry = m_sphereTestList[index];
				NazaraAssert(entry.entry == oldPtr, "Invalid sphere entry");

				entry.entry = static_cast<SphereEntry*>(newPtr);
				break;
			}

			case CullTest::Volume:
			{
				VolumeVisibilityEntry& entry = m_volumeTestList[index];
				NazaraAssert(entry.entry == oldPtr, "Invalid volume entry");

				entry.entry = static_cast<VolumeEntry*>(newPtr);
				break;
			}

			default:
				NazaraInternalError("Unhandled culltype");
				break;
		}
	}

	template<typename T>
	void CullingList<T>::NotifyRelease(CullTest type, std::size_t index)
	{
		switch (type)
		{
			case CullTest::NoTest:
			{
				m_noTestList[index] = std::move(m_noTestList.back());
				m_noTestList[index].entry->UpdateIndex(index);
				m_noTestList.pop_back();
				break;
			}

			case CullTest::Sphere:
			{
				m_sphereTestList[index] = std::move(m_sphereTestList.back());
				m_sphereTestList[index].entry->UpdateIndex(index);
				m_sphereTestList.pop_back();
				break;
			}

			case CullTest::Volume:
			{
				m_volumeTestList[index] = std::move(m_volumeTestList.back());
				m_volumeTestList[index].entry->UpdateIndex(index);
				m_volumeTestList.pop_back();
				break;
			}

			default:
				NazaraInternalError("Unhandled culltype");
				break;
		}
	}

	template<typename T>
	void CullingList<T>::NotifySphereUpdate(std::size_t index, const Spheref& sphere)
	{
		m_sphereTestList[index].sphere = sphere;
	}

	template<typename T>
	void CullingList<T>::NotifyVolumeUpdate(std::size_t index, const BoundingVolumef& boundingVolume)
	{
		m_volumeTestList[index].volume = boundingVolume;
	}

	//////////////////////////////////////////////////////////////////////////

	template<typename T>
	template<CullTest Type>
	CullingList<T>::Entry<Type>::Entry() :
	m_parent(nullptr)
	{
	}

	template<typename T>
	template<CullTest Type>
	CullingList<T>::Entry<Type>::Entry(CullingList* parent, std::size_t index) :
	m_index(index),
	m_parent(parent)
	{
	}

	template<typename T>
	template<CullTest Type>
	CullingList<T>::Entry<Type>::Entry(Entry&& entry) :
	m_index(entry.m_index),
	m_parent(entry.m_parent)
	{
		if (m_parent)
			m_parent->NotifyMovement(Type, m_index, &entry, this);

		entry.m_parent = nullptr;
	}

	template<typename T>
	template<CullTest Type>
	CullingList<T>::Entry<Type>::~Entry()
	{
		if (m_parent)
			m_parent->NotifyRelease(Type, m_index);
	}

	template<typename T>
	template<CullTest Type>
	void CullingList<T>::Entry<Type>::ForceInvalidation()
	{
		m_parent->NotifyForceInvalidation(Type, m_index);
	}

	template<typename T>
	template<CullTest Type>
	CullingList<T>* CullingList<T>::Entry<Type>::GetParent() const
	{
		return m_parent;
	}

	template<typename T>
	template<CullTest Type>
	void CullingList<T>::Entry<Type>::UpdateIndex(std::size_t index)
	{
		m_index = index;
	}

	template<typename T>
	template<CullTest Type>
	typename CullingList<T>::Entry<Type>& CullingList<T>::Entry<Type>::operator=(Entry&& entry)
	{
		m_index = entry.m_index;
		m_parent = entry.m_parent;
		if (m_parent)
			m_parent->NotifyMovement(Type, m_index, &entry, this);

		entry.m_parent = nullptr;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////

	template<typename T>
	CullingList<T>::NoTestEntry::NoTestEntry() :
	Entry<CullTest::NoTest>()
	{
	}

	template<typename T>
	CullingList<T>::NoTestEntry::NoTestEntry(CullingList* parent, std::size_t index) :
	Entry<CullTest::NoTest>(parent, index)
	{
	}

	//////////////////////////////////////////////////////////////////////////

	template<typename T>
	CullingList<T>::SphereEntry::SphereEntry() :
	Entry<CullTest::Sphere>()
	{
	}

	template<typename T>
	CullingList<T>::SphereEntry::SphereEntry(CullingList* parent, std::size_t index) :
	Entry<CullTest::Sphere>(parent, index)
	{
	}

	template<typename T>
	void CullingList<T>::SphereEntry::UpdateSphere(const Spheref& sphere)
	{
		this->m_parent->NotifySphereUpdate(this->m_index, sphere);
	}

	//////////////////////////////////////////////////////////////////////////
	
	template<typename T>
	CullingList<T>::VolumeEntry::VolumeEntry() :
	Entry<CullTest::Volume>()
	{
	}

	template<typename T>
	CullingList<T>::VolumeEntry::VolumeEntry(CullingList* parent, std::size_t index) :
	Entry<CullTest::Volume>(parent, index)
	{
	}

	template<typename T>
	void CullingList<T>::VolumeEntry::UpdateVolume(const BoundingVolumef& volume)
	{
		this->m_parent->NotifyVolumeUpdate(this->m_index, volume);
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
