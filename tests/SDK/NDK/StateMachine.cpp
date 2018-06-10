#include <NDK/StateMachine.hpp>
#include <Catch/catch.hpp>

class TestState : public Ndk::State
{
	public:
		void Enter(Ndk::StateMachine& /*fsm*/) override
		{
			m_isUpdated = false;
		}

		bool IsUpdated() const
		{
			return m_isUpdated;
		}

		void Leave(Ndk::StateMachine& /*fsm*/) override
		{
		}

		bool Update(Ndk::StateMachine& /*fsm*/, float /*elapsedTime*/) override
		{
			m_isUpdated = true;
			return true;
		}

	private:
		bool m_isUpdated;
};

class SecondTestState : public Ndk::State
{
	public:
		void Enter(Ndk::StateMachine& /*fsm*/) override
		{
			m_isUpdated = false;
		}

		bool IsUpdated() const
		{
			return m_isUpdated;
		}

		void Leave(Ndk::StateMachine& /*fsm*/) override
		{
		}

		bool Update(Ndk::StateMachine& fsm, float /*elapsedTime*/) override
		{
			if (fsm.IsTopState(this))
				m_isUpdated = true;
			return true;
		}

	private:
		bool m_isUpdated;
};

SCENARIO("State & StateMachine", "[NDK][STATE]")
{
	GIVEN("A statemachine with our test states")
	{
		std::shared_ptr<TestState> testState = std::make_shared<TestState>();
		std::shared_ptr<SecondTestState> secondTestState = std::make_shared<SecondTestState>();
		Ndk::StateMachine stateMachine(secondTestState);
		stateMachine.PushState(testState);
		CHECK(!testState->IsUpdated());
		CHECK(!secondTestState->IsUpdated());

		WHEN("We update our machine")
		{
			stateMachine.Update(1.f);

			THEN("Our state on the top has been updated but not the bottom one")
			{
				CHECK(stateMachine.IsTopState(testState.get()));
				CHECK(!stateMachine.IsTopState(secondTestState.get()));

				CHECK(testState->IsUpdated());
				CHECK(!secondTestState->IsUpdated());
			}
		}

		WHEN("We exchange the states' positions while emptying the stack")
		{
			stateMachine.PopStatesUntil(secondTestState);
			stateMachine.Update(1.f);
			CHECK(stateMachine.IsTopState(secondTestState.get()));

			stateMachine.ResetState(testState);
			stateMachine.PushState(secondTestState);

			stateMachine.Update(1.f);

			THEN("Both states should be updated")
			{
				CHECK(!stateMachine.IsTopState(testState.get()));
				CHECK(stateMachine.IsTopState(secondTestState.get()));

				CHECK(testState->IsUpdated());
				CHECK(secondTestState->IsUpdated());
			}
		}
	}
}
