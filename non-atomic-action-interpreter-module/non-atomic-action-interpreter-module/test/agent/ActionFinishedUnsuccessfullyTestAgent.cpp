#include "ActionFinishedUnsuccessfullyTestAgent.hpp"

#include "keynodes/TestKeynodes.hpp"

using namespace nonAtomicActionInterpreterModuleTest;

ScResult ActionFinishedUnsuccessfullyTestAgent::DoProgram(ScAction & action)
{
  return action.FinishUnsuccessfully();
}

ScAddr ActionFinishedUnsuccessfullyTestAgent::GetActionClass() const
{
  return TestKeynodes::unsuccessfully_finished_test_action;
}
