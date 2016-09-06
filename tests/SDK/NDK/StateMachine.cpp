#include <NDK/StateMachine.hpp>
#include <Catch/catch.hpp>

class TestState : public Ndk::State
{
	public:
		void Enter(Ndk::StateMachine& fsm) override
		{
			m_isUpdated = false;
		}

		bool IsUpdated() const
		{
			return m_isUpdated;
		}

		void Leave(Ndk::StateMachine& fsm) override
		{
		}

		bool Update(Ndk::StateMachine& fsm, float elapsedTime) override
		{
			m_isUpdated = true;
			return true;
		}

	private:
		bool m_isUpdated;
};

SCENARIO("State & StateMachine", "[NDK][STATE]")
{
	GIVEN("A statemachine with our TestState")
	{
		std::shared_ptr<TestState> testState = std::make_shared<TestState>();
		Ndk::StateMachine stateMachine(testState);
		REQUIRE(!testState->IsUpdated());

		WHEN("We update our machine")
		{
			stateMachine.Update(1.f);

			THEN("Our state has been updated")
			{
				REQUIRE(testState->IsUpdated());
			}
		}
	}
}
