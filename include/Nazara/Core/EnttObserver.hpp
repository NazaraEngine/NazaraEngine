// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_ENTTOBSERVER_HPP
#define NAZARA_CORE_ENTTOBSERVER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <NazaraUtils/Signal.hpp>
#include <NazaraUtils/TypeList.hpp>
#include <entt/entt.hpp>

namespace Nz
{
	template<typename IncludeList, typename ExcludeList = TypeList<>, typename Data = void>
	class EnttObserver
	{
		public:
			EnttObserver(entt::registry& registry);
			EnttObserver(const EnttObserver&) = delete;
			EnttObserver(EnttObserver&&) = delete;
			~EnttObserver() = default;

			[[nodiscard]] bool Contains(entt::entity entity) const noexcept;

			[[nodiscard]] auto& Get(entt::entity entity) noexcept;
			[[nodiscard]] const auto& Get(entt::entity entity) const noexcept;

			[[nodiscard]] std::size_t GetCapacity() const noexcept;
			[[nodiscard]] std::size_t GetSize() const noexcept;

			[[nodiscard]] bool IsEmpty() const noexcept;

			bool Remove(entt::entity entity) noexcept;

			void SignalExisting();

			[[nodiscard]] auto* TryGet(entt::entity entity) noexcept;
			[[nodiscard]] const auto* TryGet(entt::entity entity) const noexcept;

			[[nodiscard]] auto begin() noexcept;
			[[nodiscard]] auto begin() const noexcept;
			[[nodiscard]] auto each() noexcept;
			[[nodiscard]] auto each() const noexcept;
			[[nodiscard]] auto end() noexcept;
			[[nodiscard]] auto end() const noexcept;

			EnttObserver& operator=(const EnttObserver&) = delete;
			EnttObserver& operator=(EnttObserver&&) = delete;

			NazaraSignal(OnEntityAdded, entt::entity /*entity*/);
			NazaraSignal(OnEntityRemove, entt::entity /*entity*/);

		private:
			using EnttStorage = entt::storage<Data>;

			class CustomStorage : public EnttStorage
			{
				friend EnttObserver;

				public:
					using EnttStorage::EnttStorage;

					void TriggerAddedCallback(entt::entity entity);
					void TriggerRemoveCallback(entt::entity entity);

				private:
					EnttObserver* m_enttObserver;
			};

			friend CustomStorage;

			using ReactiveStorage = entt::reactive_mixin<CustomStorage>;

			ReactiveStorage m_storage;
	};
}

#include <Nazara/Core/EnttObserver.inl>

#endif // NAZARA_CORE_ENTTOBSERVER_HPP
