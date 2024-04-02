#include "NonAtomicActionInterpreterModule.hpp"

using namespace nonAtomicActionInterpreterModule;

SC_IMPLEMENT_MODULE(NonAtomicActionInterpreterModule)

sc_result NonAtomicActionInterpreterModule::InitializeImpl()
{
  if (nonAtomicActionInterpreterModule::Keynodes::InitGlobal() == SC_FALSE)
    return SC_RESULT_ERROR;

  ScMemoryContext ctx;

  if (ActionUtils::isActionDeactivated(&ctx, Keynodes::action_interpret_non_atomic_action))
  {
    SC_LOG_WARNING("action_interpret_non_atomic_action is deactivated");
  }
  else
  {
    SC_AGENT_REGISTER(NonAtomicActionInterpreterAgent)
  }

  return SC_RESULT_OK;
}

sc_result NonAtomicActionInterpreterModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(NonAtomicActionInterpreterAgent)

  return SC_RESULT_OK;
}
