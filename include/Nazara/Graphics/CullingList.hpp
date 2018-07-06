// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CULLINGLIST_HPP
#define NAZARA_CULLINGLIST_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Math/BoundingVolume.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <vector>

namespace Nz
{
	template<typename T>
	class CullingList
	{
		public:
			template<CullTest> class Entry;
			class NoTestEntry;
			class SphereEntry;
			class VolumeEntry;

			template<CullTest> friend class Entry;
			friend NoTestEntry;
			friend SphereEntry;
			friend VolumeEntry;

			using ResultContainer = std::vector<const T*>;

			CullingList() = default;
			CullingList(const CullingList& renderable) = delete;
			CullingList(CullingList&& renderable) = delete;
			~CullingList();

			std::size_t Cull(const Frustumf& frustum, bool* forceInvalidation = nullptr);

			void RegisterNoTest(const T* renderable, NoTestEntry* entry);
			void RegisterSphereTest(const T* renderable, SphereEntry* entry);
			void RegisterVolumeTest(const T* renderable, VolumeEntry* entry);

			CullingList& operator=(const CullingList& renderable) = delete;
			CullingList& operator=(CullingList&& renderable) = delete;

			// STL API
			typename ResultContainer::iterator begin();
			typename ResultContainer::const_iterator begin() const;

			typename ResultContainer::const_iterator cbegin() const;
			typename ResultContainer::const_iterator cend() const;
			typename ResultContainer::const_reverse_iterator crbegin() const;
			typename ResultContainer::const_reverse_iterator crend() const;

			bool empty() const;

			typename ResultContainer::iterator end();
			typename ResultContainer::const_iterator end() const;

			typename ResultContainer::reverse_iterator rbegin();
			typename ResultContainer::const_reverse_iterator rbegin() const;

			typename ResultContainer::reverse_iterator rend();
			typename ResultContainer::const_reverse_iterator rend() const;

			typename ResultContainer::size_type size() const;

			NazaraSignal(OnCullingListRelease, CullingList* /*cullingList*/);

		private:
			inline void NotifyForceInvalidation(CullTest type, std::size_t index);
			inline void NotifyMovement(CullTest type, std::size_t index, void* oldPtr, void* newPtr);
			inline void NotifyRelease(CullTest type, std::size_t index);
			inline void NotifySphereUpdate(std::size_t index, const Spheref& sphere);
			inline void NotifyVolumeUpdate(std::size_t index, const BoundingVolumef& boundingVolume);

			struct NoTestVisibilityEntry
			{
				NoTestEntry* entry;
				const T* renderable;
				bool forceInvalidation;
			};

			struct SphereVisibilityEntry
			{
				Spheref sphere;
				SphereEntry* entry;
				const T* renderable;
				bool forceInvalidation;
			};

			struct VolumeVisibilityEntry
			{
				BoundingVolumef volume;
				VolumeEntry* entry;
				const T* renderable;
				bool forceInvalidation;
			};

			std::vector<NoTestVisibilityEntry> m_noTestList;
			std::vector<SphereVisibilityEntry> m_sphereTestList;
			std::vector<VolumeVisibilityEntry> m_volumeTestList;
			ResultContainer m_results;
	};

	template<typename T>
	template<CullTest Type>
	class CullingList<T>::Entry
	{
		public:
			Entry();
			Entry(const Entry&) = delete;
			Entry(Entry&& entry);
			~Entry();

			void ForceInvalidation();

			CullingList* GetParent() const;

			void UpdateIndex(std::size_t index);

			Entry& operator=(const Entry&) = delete;
			Entry& operator=(Entry&& entry);

		protected:
			Entry(CullingList* parent, std::size_t index);

			std::size_t m_index;
			CullingList* m_parent;
	};

	template<typename T>
	class CullingList<T>::NoTestEntry : public CullingList<T>::template Entry<CullTest::NoTest>
	{
		friend CullingList;

		public:
			NoTestEntry();
			NoTestEntry(NoTestEntry&&) = default;
			~NoTestEntry() = default;

			NoTestEntry& operator=(NoTestEntry&&) = default;

		private:
			NoTestEntry(CullingList* parent, std::size_t index);
	};

	template<typename T>
	class CullingList<T>::SphereEntry : public CullingList<T>::template Entry<CullTest::Sphere>
	{
		friend CullingList;

		public:
			SphereEntry();
			SphereEntry(SphereEntry&&) = default;
			~SphereEntry() = default;

			void UpdateSphere(const Spheref& sphere);

			SphereEntry& operator=(SphereEntry&&) = default;

		private:
			SphereEntry(CullingList* parent, std::size_t index);
	};

	template<typename T>
	class CullingList<T>::VolumeEntry : public CullingList<T>::template Entry<CullTest::Volume>
	{
		friend CullingList;

		public:
			VolumeEntry();
			VolumeEntry(VolumeEntry&&) = default;
			~VolumeEntry() = default;

			void UpdateVolume(const BoundingVolumef& sphere);

			VolumeEntry& operator=(VolumeEntry&&) = default;

		private:
			VolumeEntry(CullingList* parent, std::size_t index);
	};
}

#include <Nazara/Graphics/CullingList.inl>

#endif // NAZARA_CULLINGLIST_HPP
