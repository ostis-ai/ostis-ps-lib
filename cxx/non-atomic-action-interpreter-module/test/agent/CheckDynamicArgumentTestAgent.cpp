#include "CheckDynamicArgumentTestAgent.hpp"

#include "test/keynodes/TestKeynodes.hpp"

using namespace nonAtomicActionInterpreterModuleTest;

ScResult CheckDynamicArgumentTestAgent::DoProgram(ScAction & action)
{
  ScTemplate scTemplate;
  scTemplate.Quintuple(
      action, ScType::VarPermPosArc, ScType::VarNode >> "_dynamic_argument", ScType::VarPermPosArc, ScKeynodes::rrel_1);
  scTemplate.Triple("_dynamic_argument", ScType::VarTempPosArc, TestKeynodes::test_node);
  ScTemplateSearchResult results;
  m_context.SearchByTemplate(scTemplate, results);
  if (results.Size() == 1)
  {
    return action.FinishSuccessfully();
  }
  else
  {
    return action.FinishUnsuccessfully();
  }
}

ScAddr CheckDynamicArgumentTestAgent::GetActionClass() const
{
  return TestKeynodes::check_dynamic_argument_test_action;
}
