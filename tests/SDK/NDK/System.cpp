#include <NDK/System.hpp>
#include <Catch/catch.hpp>

namespace
{
	class TestSystem : public Ndk::System<TestSystem>
	{
		public:
			TestSystem(int value) :
			m_value(value)
			{
			}

			int GetValue() const
			{
				return m_value;
			}

			~TestSystem() = default;

			static Ndk::SystemIndex systemIndex;

		private:
			int m_value;

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
		TestSystem testSystem(666);

		WHEN("We clone it")
		{
			std::unique_ptr<Ndk::BaseSystem> clone = testSystem.Clone();

			THEN("We should get a copy")
			{
				REQUIRE(static_cast<TestSystem*>(clone.get())->GetValue() == 42);
			}
		}
	}
}