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
			class BoxEntry;
			class NoTestEntry;
			class SphereEntry;
			class VolumeEntry;

			template<CullTest> friend class Entry;
			friend BoxEntry;
			friend NoTestEntry;
			friend SphereEntry;
			friend VolumeEntry;

			using ResultContainer = std::vector<const T*>;

			CullingList() = default;
			CullingList(const CullingList& renderable) = delete;
			CullingList(CullingList&& renderable) = delete;
			~CullingList();

			std::size_t Cull(const Frustumf& frustum, bool* forceInvalidation = nullptr);

			std::size_t FillWithAllEntries(bool* forceInvalidation = nullptr);

			const ResultContainer& GetFullyVisibleResults() const;
			const ResultContainer& GetPartiallyVisibleResults() const;

			BoxEntry RegisterBoxTest(const T* renderable);
			NoTestEntry RegisterNoTest(const T* renderable);
			SphereEntry RegisterSphereTest(const T* renderable);
			VolumeEntry RegisterVolumeTest(const T* renderable);

			CullingList& operator=(const CullingList& renderable) = delete;
			CullingList& operator=(CullingList&& renderable) = delete;

			NazaraSignal(OnCullingListRelease, CullingList* /*cullingList*/);

		private:
			inline void NotifyBoxUpdate(std::size_t index, const Boxf& boundingVolume);
			inline void NotifyForceInvalidation(CullTest type, std::size_t index);
			inline void NotifyMovement(CullTest type, std::size_t index, void* oldPtr, void* newPtr);
			inline void NotifyRelease(CullTest type, std::size_t index);
			inline void NotifySphereUpdate(std::size_t index, const Spheref& sphere);
			inline void NotifyVolumeUpdate(std::size_t index, const BoundingVolumef& boundingVolume);

			struct BoxVisibilityEntry
			{
				Boxf box;
				BoxEntry* entry;
				const T* renderable;
				bool forceInvalidation;
			};

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

			std::vector<BoxVisibilityEntry> m_boxTestList;
			std::vector<NoTestVisibilityEntry> m_noTestList;
			std::vector<SphereVisibilityEntry> m_sphereTestList;
			std::vector<VolumeVisibilityEntry> m_volumeTestList;
			ResultContainer m_fullyVisibleResults;
			ResultContainer m_partiallyVisibleResults;
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
	class CullingList<T>::BoxEntry : public CullingList<T>::template Entry<CullTest::Box>
	{
		friend CullingList;

		public:
			BoxEntry();
			BoxEntry(BoxEntry&&) = default;
			~BoxEntry() = default;

			void UpdateBox(const Boxf& box);

			BoxEntry& operator=(BoxEntry&&) = default;

		private:
			BoxEntry(CullingList* parent, std::size_t index);
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
