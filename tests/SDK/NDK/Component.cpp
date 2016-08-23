#include <NDK/Component.hpp>
#include <Catch/catch.hpp>

namespace
{
	class TestComponent : public Ndk::Component<TestComponent>
	{
		public:
			static Ndk::ComponentIndex componentIndex;
	};

	Ndk::ComponentIndex TestComponent::componentIndex;
}

SCENARIO("Component", "[NDK][COMPONENT]")
{
	GIVEN("Our TestComponent")
	{
		TestComponent testComponent;

		WHEN("We clone it")
		{
			std::unique_ptr<Ndk::BaseComponent> clone = testComponent.Clone();

			THEN("We should get a copy")
			{
				REQUIRE(dynamic_cast<TestComponent*>(clone.get()) != nullptr);
			}
		}
	}
}