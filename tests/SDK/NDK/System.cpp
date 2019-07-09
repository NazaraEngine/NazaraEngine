#include <NDK/System.hpp>
#include <Catch/catch.hpp>

namespace
{
	class TestSystem : public Ndk::System<TestSystem>
	{
		public:
			TestSystem() :
			m_updateCounter(0),
			m_elapsedTime(0.f)
			{
			}

			~TestSystem() = default;

			float GetElapsedTime() const
			{
				return m_elapsedTime;
			}

			std::size_t GetLoopCount() const
			{
				return m_updateCounter;
			}

			static Ndk::SystemIndex systemIndex;

		private:
			std::size_t m_updateCounter;
			float m_elapsedTime;

			void OnUpdate(float elapsedTime) override
			{
				++m_updateCounter;

				m_elapsedTime += elapsedTime;
			}
	};

	Ndk::SystemIndex TestSystem::systemIndex;
}

SCENARIO("System", "[NDK][SYSTEM]")
{
	GIVEN("Our TestSystem")
	{
		TestSystem testSystem;
		testSystem.SetMaximumUpdateRate(30.f);

		float maxTimePerFrame = 1 / 30.f;

		WHEN("We update it with a higher framerate")
		{
			float timePerFrame = maxTimePerFrame / 2.f;
			float elapsedTime = 2.f;

			std::size_t loopCount = static_cast<std::size_t>(std::round(elapsedTime / timePerFrame));

			for (std::size_t i = 0; i < loopCount; ++i)
				testSystem.Update(timePerFrame);

			CHECK(testSystem.GetLoopCount() == loopCount / 2);
			CHECK(testSystem.GetElapsedTime() == Approx(elapsedTime).epsilon(timePerFrame));
		}

		WHEN("We update it with a lower framerate")
		{
			float timePerFrame = maxTimePerFrame * 2.f;
			float elapsedTime = 10.f;

			std::size_t loopCount = static_cast<std::size_t>(std::round(elapsedTime / timePerFrame));

			for (std::size_t i = 0; i < loopCount; ++i)
				testSystem.Update(timePerFrame);

			CHECK(testSystem.GetLoopCount() == loopCount);
			CHECK(testSystem.GetElapsedTime() == Approx(elapsedTime).epsilon(timePerFrame));

			AND_WHEN("We suddenly increase framerate")
			{
				float newTimePerFrame = 1 / 300.f;
				float newElapsedTime = 100.f;

				std::size_t newLoopCount = static_cast<std::size_t>(std::round(newElapsedTime / newTimePerFrame));

				for (std::size_t i = 0; i < newLoopCount; ++i)
					testSystem.Update(newTimePerFrame);

				CHECK(testSystem.GetLoopCount() == loopCount + newLoopCount / 10);
				CHECK(testSystem.GetElapsedTime() == Approx(elapsedTime + newElapsedTime).epsilon(newTimePerFrame));
			}
		}


		WHEN("We update it with a very low framerate")
		{
			float timePerFrame = 0.5f;

			for (std::size_t i = 0; i < 10; ++i)
				testSystem.Update(timePerFrame);

			CHECK(testSystem.GetLoopCount() == 10);
			CHECK(testSystem.GetElapsedTime() == Approx(5.f));
		}
	}
}
