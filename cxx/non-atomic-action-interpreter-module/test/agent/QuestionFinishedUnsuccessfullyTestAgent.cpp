#include "QuestionFinishedUnsuccessfullyTestAgent.hpp"

#include "test/keynodes/TestKeynodes.hpp"

using namespace nonAtomicActionInterpreterModuleTest;

ScResult QuestionFinishedUnsuccessfullyTestAgent::DoProgram(ScAction & action)
{
  return action.FinishUnsuccessfully();
}

ScAddr QuestionFinishedUnsuccessfullyTestAgent::GetActionClass() const
{
  return TestKeynodes::unsuccessfully_finished_test_action;
}
