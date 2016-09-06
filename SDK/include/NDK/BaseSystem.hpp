// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_BASESYSTEM_HPP
#define NDK_BASESYSTEM_HPP

#include <Nazara/Core/Bitset.hpp>
#include <NDK/Entity.hpp>
#include <vector>

namespace Ndk
{
	class World;

	class NDK_API BaseSystem
	{
		friend class Sdk;
		friend Entity;
		friend World;

		public:
			inline BaseSystem(SystemIndex systemId);
			inline BaseSystem(const BaseSystem&);
			BaseSystem(BaseSystem&&) noexcept = default;
			virtual ~BaseSystem();

			inline void Enable(bool enable = true);

			virtual std::unique_ptr<BaseSystem> Clone() const = 0;

			bool Filters(const Entity* entity) const;

			inline const std::vector<EntityHandle>& GetEntities() const;
			inline SystemIndex GetIndex() const;
			inline float GetUpdateRate() const;
			inline World& GetWorld() const;

			inline bool IsEnabled() const;

			inline bool HasEntity(const Entity* entity) const;

			inline void SetUpdateRate(float updatePerSecond);

			inline void Update(float elapsedTime);

			BaseSystem& operator=(const BaseSystem&) = delete;
			BaseSystem& operator=(BaseSystem&&) noexcept = default;

		protected:
			template<typename ComponentType> void Excludes();
			template<typename ComponentType1, typename ComponentType2, typename... Rest> void Excludes();
			inline void ExcludesComponent(ComponentIndex index);

			static SystemIndex GetNextIndex();

			template<typename ComponentType> void Requires();
			template<typename ComponentType1, typename ComponentType2, typename... Rest> void Requires();
			inline void RequiresComponent(ComponentIndex index);

			template<typename ComponentType> void RequiresAny();
			template<typename ComponentType1, typename ComponentType2, typename... Rest> void RequiresAny();
			inline void RequiresAnyComponent(ComponentIndex index);

			virtual void OnUpdate(float elapsedTime) = 0;

		private:
			inline void AddEntity(Entity* entity);

			virtual void OnEntityAdded(Entity* entity);
			virtual void OnEntityRemoved(Entity* entity);
			virtual void OnEntityValidation(Entity* entity, bool justAdded);

			inline void RemoveEntity(Entity* entity);

			inline void SetWorld(World* world) noexcept;

			inline void ValidateEntity(Entity* entity, bool justAdded);

			static inline bool Initialize();
			static inline void Uninitialize();

			std::vector<EntityHandle> m_entities;
			Nz::Bitset<Nz::UInt64> m_entityBits;
			Nz::Bitset<> m_excludedComponents;
			mutable Nz::Bitset<> m_filterResult;
			Nz::Bitset<> m_requiredAnyComponents;
			Nz::Bitset<> m_requiredComponents;
			SystemIndex m_systemIndex;
			World* m_world;
			bool m_updateEnabled;
			float m_updateCounter;
			float m_updateRate;

			static SystemIndex s_nextIndex;
	};
}

#include <NDK/BaseSystem.inl>

#endif // NDK_BASESYSTEM_HPP
