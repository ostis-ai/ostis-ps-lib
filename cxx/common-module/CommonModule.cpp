
#include "CommonModule.hpp"

#include "sc-memory/sc_memory.hpp"

#include "keynodes/Keynodes.hpp"

using namespace commonModule;

SC_IMPLEMENT_MODULE(CommonModule)

sc_result CommonModule::InitializeImpl()
{
  if (!Keynodes::InitGlobal())
    return SC_RESULT_ERROR;

  return SC_RESULT_OK;
}

sc_result CommonModule::ShutdownImpl()
{
  return SC_RESULT_OK;
}
