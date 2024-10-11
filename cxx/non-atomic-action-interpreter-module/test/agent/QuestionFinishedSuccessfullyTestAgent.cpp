#include "QuestionFinishedSuccessfullyTestAgent.hpp"

#include "test/keynodes/TestKeynodes.hpp"

using namespace nonAtomicActionInterpreterModuleTest;

ScResult QuestionFinishedSuccessfullyTestAgent::DoProgram(ScAction & action)
{
  return action.FinishSuccessfully();
}

ScAddr QuestionFinishedSuccessfullyTestAgent::GetActionClass() const
{
  return TestKeynodes::successfully_finished_test_action;
}
