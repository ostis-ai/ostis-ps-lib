#include "AssignDynamicArgumentTestAgent.hpp"

#include "keynodes/TestKeynodes.hpp"

using namespace nonAtomicActionInterpreterModuleTest;

ScResult AssignDynamicArgumentTestAgent::DoProgram(ScAction & action)
{
  ScAddr dynamicArgument = action.GetArgument(1);

  m_context.GenerateConnector(ScType::ConstTempPosArc, dynamicArgument, TestKeynodes::test_node);

  return action.FinishSuccessfully();
}

ScAddr AssignDynamicArgumentTestAgent::GetActionClass() const
{
  return TestKeynodes::assign_dynamic_argument_test_action;
}
