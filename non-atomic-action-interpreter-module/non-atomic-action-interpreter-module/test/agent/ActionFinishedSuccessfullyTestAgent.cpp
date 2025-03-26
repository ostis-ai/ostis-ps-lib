#include "ActionFinishedSuccessfullyTestAgent.hpp"

#include "keynodes/TestKeynodes.hpp"

using namespace nonAtomicActionInterpreterModuleTest;

ScResult ActionFinishedSuccessfullyTestAgent::DoProgram(ScAction & action)
{
  return action.FinishSuccessfully();
}

ScAddr ActionFinishedSuccessfullyTestAgent::GetActionClass() const
{
  return TestKeynodes::successfully_finished_test_action;
}
