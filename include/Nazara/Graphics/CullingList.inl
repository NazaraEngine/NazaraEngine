// Copyright (C) 2017 Jérôme Leclercq
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
		m_fullyVisibleResults.clear();
		m_partiallyVisibleResults.clear();

		bool forcedInvalidation = false;

		std::size_t fullyVisibleHash = 5U;
		std::size_t partiallyVisibleHash = 5U;

		auto CombineHash = [](std::size_t currentHash, std::size_t newHash)
		{
			return currentHash * 23 + newHash;
		};

		for (BoxVisibilityEntry& entry : m_boxTestList)
		{
			switch (frustum.Intersect(entry.box))
			{
				case IntersectionSide_Inside:
					m_fullyVisibleResults.push_back(entry.renderable);
					CombineHash(fullyVisibleHash, std::hash<const T*>()(entry.renderable));

					forcedInvalidation = forcedInvalidation | entry.forceInvalidation;
					entry.forceInvalidation = false;
					break;

				case IntersectionSide_Intersecting:
					m_partiallyVisibleResults.push_back(entry.renderable);
					CombineHash(partiallyVisibleHash, std::hash<const T*>()(entry.renderable));

					forcedInvalidation = forcedInvalidation | entry.forceInvalidation;
					entry.forceInvalidation = false;
					break;

				case IntersectionSide_Outside:
					break;
			}
		}

		for (NoTestVisibilityEntry& entry : m_noTestList)
		{
			m_fullyVisibleResults.push_back(entry.renderable);
			CombineHash(fullyVisibleHash, std::hash<const T*>()(entry.renderable));

			if (entry.forceInvalidation)
			{
				forcedInvalidation = true;
				entry.forceInvalidation = false;
			}
		}

		for (SphereVisibilityEntry& entry : m_sphereTestList)
		{
			switch (frustum.Intersect(entry.sphere))
			{
				case IntersectionSide_Inside:
					m_fullyVisibleResults.push_back(entry.renderable);
					CombineHash(fullyVisibleHash, std::hash<const T*>()(entry.renderable));

					forcedInvalidation = forcedInvalidation | entry.forceInvalidation;
					entry.forceInvalidation = false;
					break;

				case IntersectionSide_Intersecting:
					m_partiallyVisibleResults.push_back(entry.renderable);
					CombineHash(partiallyVisibleHash, std::hash<const T*>()(entry.renderable));

					forcedInvalidation = forcedInvalidation | entry.forceInvalidation;
					entry.forceInvalidation = false;
					break;

				case IntersectionSide_Outside:
					break;
			}
		}
		
		for (VolumeVisibilityEntry& entry : m_volumeTestList)
		{
			switch (frustum.Intersect(entry.volume))
			{
				case IntersectionSide_Inside:
					m_fullyVisibleResults.push_back(entry.renderable);
					CombineHash(fullyVisibleHash, std::hash<const T*>()(entry.renderable));

					forcedInvalidation = forcedInvalidation | entry.forceInvalidation;
					entry.forceInvalidation = false;
					break;

				case IntersectionSide_Intersecting:
					m_partiallyVisibleResults.push_back(entry.renderable);
					CombineHash(partiallyVisibleHash, std::hash<const T*>()(entry.renderable));

					forcedInvalidation = forcedInvalidation | entry.forceInvalidation;
					entry.forceInvalidation = false;
					break;

				case IntersectionSide_Outside:
					break;
			}
		}

		if (forceInvalidation)
			*forceInvalidation = forcedInvalidation;

		return 5 + partiallyVisibleHash * 17 + fullyVisibleHash;
	}

	template<typename T>
	std::size_t CullingList<T>::FillWithAllEntries(bool* forceInvalidation)
	{
		m_fullyVisibleResults.clear();
		m_partiallyVisibleResults.clear();

		bool forcedInvalidation = false;

		std::size_t visibleHash = 5U;

		auto FillWithList = [&](auto& testList)
		{
			for (auto& entry : testList)
			{
				m_fullyVisibleResults.push_back(entry.renderable);
				visibleHash = visibleHash * 23 + std::hash<const T*>()(entry.renderable);

				forcedInvalidation = forcedInvalidation | entry.forceInvalidation;
				entry.forceInvalidation = false;
			}
		};

		FillWithList(m_boxTestList);
		FillWithList(m_noTestList);
		FillWithList(m_sphereTestList);
		FillWithList(m_volumeTestList);

		if (forceInvalidation)
			*forceInvalidation = forcedInvalidation;

		return visibleHash;
	}

	template<typename T>
	auto CullingList<T>::GetFullyVisibleResults() const -> const ResultContainer&
	{
		return m_fullyVisibleResults;
	}

	template<typename T>
	auto CullingList<T>::GetPartiallyVisibleResults() const -> const ResultContainer&
	{
		return m_partiallyVisibleResults;
	}

	template<typename T>
	auto CullingList<T>::RegisterBoxTest(const T* renderable) -> BoxEntry
	{
		BoxEntry newEntry(this, m_boxTestList.size());
		m_boxTestList.emplace_back(BoxVisibilityEntry{ Nz::Boxf(), &newEntry, renderable, false }); //< Address of entry will be updated when moving

		return newEntry;
	}

	template<typename T>
	auto CullingList<T>::RegisterNoTest(const T* renderable) -> NoTestEntry
	{
		NoTestEntry newEntry(this, m_volumeTestList.size());
		m_noTestList.emplace_back(NoTestVisibilityEntry{&newEntry, renderable, false}); //< Address of entry will be updated when moving

		return newEntry;
	}

	template<typename T>
	auto CullingList<T>::RegisterSphereTest(const T* renderable) -> SphereEntry
	{
		SphereEntry newEntry(this, m_sphereTestList.size());
		m_sphereTestList.emplace_back(SphereVisibilityEntry{Nz::Spheref(), &newEntry, renderable, false}); //< Address of entry will be updated when moving

		return newEntry;
	}

	template<typename T>
	auto CullingList<T>::RegisterVolumeTest(const T* renderable) -> VolumeEntry
	{
		VolumeEntry newEntry(this, m_volumeTestList.size());
		m_volumeTestList.emplace_back(VolumeVisibilityEntry{Nz::BoundingVolumef(), &newEntry, renderable, false}); //< Address of entry will be updated when moving

		return newEntry;
	}

	template<typename T>
	inline void CullingList<T>::NotifyBoxUpdate(std::size_t index, const Boxf& box)
	{
		m_boxTestList[index].box = box;
	}

	template<typename T>
	void CullingList<T>::NotifyForceInvalidation(CullTest type, std::size_t index)
	{
		switch (type)
		{
			case CullTest::Box:
			{
				m_boxTestList[index].forceInvalidation = true;
				break;
			}

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
		NazaraUnused(oldPtr);

		switch (type)
		{
			case CullTest::Box:
			{
				BoxVisibilityEntry& entry = m_boxTestList[index];
				NazaraAssert(entry.entry == oldPtr, "Invalid box entry");

				entry.entry = static_cast<BoxEntry*>(newPtr);
				break;
			}

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
			case CullTest::Box:
			{
				m_boxTestList[index] = std::move(m_boxTestList.back());
				m_boxTestList[index].entry->UpdateIndex(index);
				m_boxTestList.pop_back();
				break;
			}

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
	#ifdef NAZARA_COMPILER_MSVC
	// MSVC bug
	typename CullingList<T>::Entry<Type>& CullingList<T>::Entry<Type>::operator=(Entry&& entry)
	#else
	typename CullingList<T>::template Entry<Type>& CullingList<T>::Entry<Type>::operator=(Entry&& entry)
	#endif
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
	CullingList<T>::BoxEntry::BoxEntry() :
	Entry<CullTest::Box>()
	{
	}

	template<typename T>
	CullingList<T>::BoxEntry::BoxEntry(CullingList* parent, std::size_t index) :
	Entry<CullTest::Box>(parent, index)
	{
	}

	template<typename T>
	void CullingList<T>::BoxEntry::UpdateBox(const Boxf& box)
	{
		this->m_parent->NotifyBoxUpdate(this->m_index, box);
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
