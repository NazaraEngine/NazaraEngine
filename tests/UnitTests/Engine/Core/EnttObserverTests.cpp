#include <Nazara/Core/EnttObserver.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <entt/entt.hpp>

class FooComponent {};
class BarComponent {};
class BazComponent {};

SCENARIO("EnttObserver", "[Core][EnttObserver]")
{
	GIVEN("An entity created and destroyed")
	{
		entt::registry registry;
		Nz::EnttObserver<Nz::TypeList<FooComponent, BarComponent>> observer(registry);

		entt::entity e1 = registry.create();

		unsigned int addSignalCounter = 0;
		observer.OnEntityAdded.Connect([&](entt::entity entity)
		{
			CHECK(entity == e1);
			addSignalCounter++;
		});

		unsigned int removeSignalCounter = 0;
		observer.OnEntityRemove.Connect([&](entt::entity entity)
		{
			CHECK(entity == e1);
			removeSignalCounter++;
		});

		registry.emplace<FooComponent>(e1);
		CHECK(addSignalCounter == 0);
		CHECK(removeSignalCounter == 0);

		registry.emplace<BarComponent>(e1);
		CHECK(addSignalCounter == 1);
		CHECK(removeSignalCounter == 0);

		registry.destroy(e1);
		CHECK(addSignalCounter == 1);
		CHECK(removeSignalCounter == 1);
	}

	GIVEN("An entity created and a component removed")
	{
		entt::registry registry;
		Nz::EnttObserver<Nz::TypeList<FooComponent, BarComponent>> observer(registry);

		entt::entity e1 = registry.create();

		unsigned int addSignalCounter = 0;
		observer.OnEntityAdded.Connect([&](entt::entity entity)
		{
			CHECK(entity == e1);
			addSignalCounter++;
		});

		unsigned int removeSignalCounter = 0;
		observer.OnEntityRemove.Connect([&](entt::entity entity)
		{
			CHECK(entity == e1);
			removeSignalCounter++;
		});

		registry.emplace<FooComponent>(e1);
		CHECK(addSignalCounter == 0);
		CHECK(removeSignalCounter == 0);

		registry.emplace<BarComponent>(e1);
		CHECK(addSignalCounter == 1);
		CHECK(removeSignalCounter == 0);

		registry.erase<BarComponent>(e1);
		CHECK(addSignalCounter == 1);
		CHECK(removeSignalCounter == 1);

		registry.destroy(e1);
		CHECK(addSignalCounter == 1);
		CHECK(removeSignalCounter == 1);
	}

	GIVEN("An entity created and an excluded component added")
	{
		entt::registry registry;
		Nz::EnttObserver<Nz::TypeList<FooComponent, BarComponent>, Nz::TypeList<BazComponent>> observer(registry);

		entt::entity e1 = registry.create();

		unsigned int addSignalCounter = 0;
		observer.OnEntityAdded.Connect([&](entt::entity entity)
		{
			CHECK(entity == e1);
			addSignalCounter++;
		});

		unsigned int removeSignalCounter = 0;
		observer.OnEntityRemove.Connect([&](entt::entity entity)
		{
			CHECK(entity == e1);
			removeSignalCounter++;
		});

		registry.emplace<BazComponent>(e1);
		CHECK(addSignalCounter == 0);
		CHECK(removeSignalCounter == 0);

		registry.emplace<FooComponent>(e1);
		CHECK(addSignalCounter == 0);
		CHECK(removeSignalCounter == 0);

		registry.emplace<BarComponent>(e1);
		CHECK(addSignalCounter == 0);
		CHECK(removeSignalCounter == 0);

		registry.erase<BazComponent>(e1);
		CHECK(addSignalCounter == 1);
		CHECK(removeSignalCounter == 0);

		registry.destroy(e1);
		CHECK(addSignalCounter == 1);
		CHECK(removeSignalCounter == 1);
	}
}
