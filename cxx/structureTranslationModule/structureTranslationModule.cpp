/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "keynodes/TranslationKeynodes.hpp"

#include "agent/StructureTranslationAgent.hpp"

#include "constants/TranslationConstants.hpp"

#include "structureTranslationModule.hpp"

namespace structureTranslationModule
{
SC_IMPLEMENT_MODULE(structureTranslationModule)

sc_result structureTranslationModule::InitializeImpl()
{
  if (TranslationKeynodes::InitGlobal() == SC_FALSE)
    return SC_RESULT_ERROR;

  SC_AGENT_REGISTER(StructureTranslationAgent)

  return SC_RESULT_OK;
}

sc_result structureTranslationModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(StructureTranslationAgent)

  return SC_RESULT_OK;
}
}  // namespace structureTranslationModule
