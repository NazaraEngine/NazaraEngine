// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	namespace Detail
	{
		template<typename ReactiveStorage, typename IncludeList, typename ExcludeList>
		struct EnttObserverCallback;

		template<typename ReactiveStorage, typename IncludeList, typename ExcludeList>
		struct EnttObserverBinder;

		template<typename ReactiveStorage, typename... IncludeList, typename... ExcludeList>
		struct EnttObserverCallback<ReactiveStorage, TypeList<IncludeList...>, TypeList<ExcludeList...>>
		{
			template<bool CanAdd>
			static void Callback(ReactiveStorage& storage, const entt::registry& registry, const entt::entity entity)
			{
				if constexpr (CanAdd)
				{
					if (registry.all_of<IncludeList...>(entity) && !registry.any_of<ExcludeList...>(entity))
					{
						if (!storage.contains(entity))
						{
							storage.emplace(entity);
							storage.TriggerAddedCallback(entity);
						}
						return;
					}
				}

				if (storage.contains(entity))
				{
					storage.TriggerRemoveCallback(entity); //< FIXME: This doesn't protect against double-signal if remove callback removes a component, but we can't remove it from storage before signaling
					storage.remove(entity);
				}
			}
		};

		template<typename ReactiveStorage, typename... IncludeList, typename... ExcludeList>
		struct EnttObserverBinder<ReactiveStorage, TypeList<IncludeList...>, TypeList<ExcludeList...>>
		{
			template<typename T, typename... Rest>
			static void BindConstructIncluded(ReactiveStorage& storage)
			{
				using CallbackHolder = EnttObserverCallback<ReactiveStorage, TypeList<IncludeList...>, TypeList<ExcludeList...>>;

				storage.template on_construct<T, &CallbackHolder::template Callback<true>>();
				if constexpr (sizeof...(Rest) > 0)
					BindConstructIncluded<Rest...>(storage);
			}

			template<typename T, typename... Rest>
			static void BindConstructExcluded(ReactiveStorage& storage)
			{
				using CallbackHolder = EnttObserverCallback<ReactiveStorage, TypeList<IncludeList...>, TypeList<ExcludeList...>>;

				storage.template on_construct<T, &CallbackHolder::template Callback<false>>();
				if constexpr (sizeof...(Rest) > 0)
					BindConstructExcluded<Rest...>(storage);
			}

			template<typename T, typename... Rest>
			static void BindDestroyIncluded(ReactiveStorage& storage)
			{
				using CallbackHolder = EnttObserverCallback<ReactiveStorage, TypeList<IncludeList...>, TypeList<ExcludeList...>>;

				storage.template on_destroy<T, &CallbackHolder::template Callback<false>>();
				if constexpr (sizeof...(Rest) > 0)
					BindDestroyIncluded<Rest...>(storage);
			}

			template<typename T, typename... Rest>
			static void BindDestroyExcluded(ReactiveStorage& storage)
			{
				using ExcludedList = TypeListRemove<TypeList<ExcludeList...>, T>;
				using CallbackHolder = EnttObserverCallback<ReactiveStorage, TypeList<IncludeList...>, ExcludedList>;

				storage.template on_destroy<T, &CallbackHolder::template Callback<true>>();
				if constexpr (sizeof...(Rest) > 0)
					BindDestroyIncluded<Rest...>(storage);
			}

			static void BindCallbacks(ReactiveStorage& storage)
			{
				if constexpr (sizeof...(IncludeList) > 0)
				{
					BindConstructIncluded<IncludeList...>(storage);
					BindDestroyIncluded<IncludeList...>(storage);
				}

				if constexpr (sizeof...(ExcludeList) > 0)
				{
					BindConstructExcluded<ExcludeList...>(storage);
					BindDestroyExcluded<ExcludeList...>(storage);
				}
			}
		};
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	EnttObserver<IncludeList, ExcludeList, Data>::EnttObserver(entt::registry& registry)
	{
		m_storage.m_enttObserver = this;
		m_storage.bind(registry);

		Detail::EnttObserverBinder<ReactiveStorage, IncludeList, ExcludeList>::BindCallbacks(m_storage);
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	[[nodiscard]] bool EnttObserver<IncludeList, ExcludeList, Data>::Contains(entt::entity entity) const noexcept
	{
		return m_storage.contains(entity);
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	[[nodiscard]] auto& EnttObserver<IncludeList, ExcludeList, Data>::Get(entt::entity entity) noexcept
	{
		return m_storage.get(entity);
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	[[nodiscard]] const auto& EnttObserver<IncludeList, ExcludeList, Data>::Get(entt::entity entity) const noexcept
	{
		return m_storage.get(entity);
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	[[nodiscard]] std::size_t EnttObserver<IncludeList, ExcludeList, Data>::GetCapacity() const noexcept
	{
		return m_storage.capacity();
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	[[nodiscard]] std::size_t EnttObserver<IncludeList, ExcludeList, Data>::GetSize() const noexcept
	{
		return m_storage.size();
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	[[nodiscard]] bool EnttObserver<IncludeList, ExcludeList, Data>::IsEmpty() const noexcept
	{
		return m_storage.empty();
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	bool EnttObserver<IncludeList, ExcludeList, Data>::Remove(entt::entity entity) noexcept
	{
		return m_storage.remove(entity);
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	void EnttObserver<IncludeList, ExcludeList, Data>::SignalExisting()
	{
		for (entt::entity entity : m_storage.template view<entt::entity>())
			OnEntityAdded(entity);
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	[[nodiscard]] auto* EnttObserver<IncludeList, ExcludeList, Data>::TryGet(entt::entity entity) noexcept
	{
		return m_storage.contains(entity) ? &m_storage.get(entity) : nullptr;
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	[[nodiscard]] const auto* EnttObserver<IncludeList, ExcludeList, Data>::TryGet(entt::entity entity) const noexcept
	{
		return m_storage.contains(entity) ? &m_storage.get(entity) : nullptr;
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	[[nodiscard]] auto EnttObserver<IncludeList, ExcludeList, Data>::begin() noexcept
	{
		return m_storage.begin();
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	[[nodiscard]] auto EnttObserver<IncludeList, ExcludeList, Data>::begin() const noexcept
	{
		return m_storage.begin();
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	[[nodiscard]] auto EnttObserver<IncludeList, ExcludeList, Data>::each() noexcept
	{
		return m_storage.each();
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	[[nodiscard]] auto EnttObserver<IncludeList, ExcludeList, Data>::each() const noexcept
	{
		return m_storage.each();
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	[[nodiscard]] auto EnttObserver<IncludeList, ExcludeList, Data>::end() noexcept
	{
		return m_storage.end();
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	[[nodiscard]] auto EnttObserver<IncludeList, ExcludeList, Data>::end() const noexcept
	{
		return m_storage.end();
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	void EnttObserver<IncludeList, ExcludeList, Data>::CustomStorage::TriggerAddedCallback(entt::entity entity)
	{
		m_enttObserver->OnEntityAdded(entity);
	}

	template<typename IncludeList, typename ExcludeList, typename Data>
	void EnttObserver<IncludeList, ExcludeList, Data>::CustomStorage::TriggerRemoveCallback(entt::entity entity)
	{
		m_enttObserver->OnEntityRemove(entity);
	}
}
