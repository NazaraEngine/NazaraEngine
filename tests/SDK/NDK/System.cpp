#include <NDK/System.hpp>
#include <Catch/catch.hpp>

namespace
{
	class TestSystem : public Ndk::System<TestSystem>
	{
		public:
			TestSystem()
			{
			}

			~TestSystem() = default;

			static Ndk::SystemIndex systemIndex;

		private:
			void OnUpdate(float elapsedTime) override
			{
			}
	};

	Ndk::SystemIndex TestSystem::systemIndex;
}

SCENARIO("System", "[NDK][SYSTEM]")
{
	GIVEN("Our TestSystem")
	{
		TestSystem testSystem;

		WHEN("We clone it")
		{
			std::unique_ptr<Ndk::BaseSystem> clone = testSystem.Clone();

			THEN("We should get a copy")
			{
				REQUIRE(dynamic_cast<TestSystem*>(clone.get()) != nullptr);
			}
		}
	}
}