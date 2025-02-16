#include "ActionFinishedTestAgent.hpp"

#include "keynodes/TestKeynodes.hpp"

using namespace nonAtomicActionInterpreterModuleTest;

ScResult ActionFinishedTestAgent::DoProgram(ScAction & action)
{
  m_context.GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::action_finished, action);
  ScAction fakeActionToNotFinishOriginalActionAgain =
      m_context.ConvertToAction(m_context.GenerateNode(ScType::ConstNode));
  fakeActionToNotFinishOriginalActionAgain.Initiate();
  return fakeActionToNotFinishOriginalActionAgain.FinishSuccessfully();
}

ScAddr ActionFinishedTestAgent::GetActionClass() const
{
  return TestKeynodes::finished_test_action;
}
