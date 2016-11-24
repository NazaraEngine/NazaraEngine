#include <NDK/Component.hpp>
#include <Catch/catch.hpp>

namespace
{
	class TestComponent : public Ndk::Component<TestComponent>
	{
		public:
			TestComponent(int value) :
			m_value(value)
			{
			}

			int GetValue() const
			{
				return m_value;
			}

			int m_value;

			static Ndk::ComponentIndex componentIndex;
	};

	Ndk::ComponentIndex TestComponent::componentIndex;
}

SCENARIO("Component", "[NDK][COMPONENT]")
{
	GIVEN("Our TestComponent")
	{
		TestComponent testComponent(42);

		WHEN("We clone it")
		{
			std::unique_ptr<Ndk::BaseComponent> clone = testComponent.Clone();

			THEN("We should get a copy")
			{
				REQUIRE(static_cast<TestComponent*>(clone.get())->GetValue() == 42);
			}
		}
	}
}