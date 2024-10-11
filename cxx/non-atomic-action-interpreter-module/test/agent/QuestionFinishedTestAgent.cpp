#include "QuestionFinishedTestAgent.hpp"

#include "test/keynodes/TestKeynodes.hpp"

using namespace nonAtomicActionInterpreterModuleTest;

ScResult QuestionFinishedTestAgent::DoProgram(ScAction & action)
{
  m_context.GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::action_finished, action);
  ScAction fakeActionToNotFinishOriginalActionAgain =
      m_context.ConvertToAction(m_context.GenerateNode(ScType::ConstNode));
  fakeActionToNotFinishOriginalActionAgain.Initiate();
  return fakeActionToNotFinishOriginalActionAgain.FinishSuccessfully();
}

ScAddr QuestionFinishedTestAgent::GetActionClass() const
{
  return TestKeynodes::finished_test_action;
}
