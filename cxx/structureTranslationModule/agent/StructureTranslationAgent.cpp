/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "keynodes/TranslationKeynodes.hpp"

#include "StructureTranslationAgent.hpp"

#include <string>
#include <sstream>

namespace structureTranslationModule
{

SC_AGENT_IMPLEMENTATION(StructureTranslationAgent)
{
  ScAddr const & actionNode = otherAddr;
  try
  {
    if (checkActionClass(actionNode) == SC_FALSE)
      return SC_RESULT_OK;
    SC_LOG_INFO("StructureTranslationAgent started");

    ScAddr const & structuresSet =
        utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, actionNode, scAgentsCommon::CoreKeynodes::rrel_1);
    ScAddr const & answerAddr =
        utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, actionNode, scAgentsCommon::CoreKeynodes::rrel_2);

    std::stringstream translation;

    ScIterator3Ptr const & structIterator =
        m_memoryCtx.Iterator3(structuresSet, ScType::EdgeAccessConstPosPerm, ScType::NodeConstStruct);
    while (structIterator->Next())
    {
      ScAddr const & structAddr = structIterator->Get(2);
      translation << translateStructure(structAddr, &m_memoryCtx);
      ScAddr const & structuresSet =
          utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, actionNode, scAgentsCommon::CoreKeynodes::rrel_1);
    }

    SC_LOG_DEBUG("StructureTranslationAgent: translation result is " << translation.str());

    ScAddr const & translationLink = m_memoryCtx.CreateLink();
    if (m_memoryCtx.IsElement(translationLink) == SC_FALSE)
      SC_THROW_EXCEPTION(utils::ScException, "StructureTranslationAgent: cannot create answer link");
    if (m_memoryCtx.SetLinkContent(translationLink, translation.str()) == SC_FALSE)
      SC_THROW_EXCEPTION(utils::ScException, "StructureTranslationAgent: cannot set link content");

    m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, answerAddr, translationLink);

    utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionNode, true);
    SC_LOG_INFO("StructureTranslationAgent finished");
    return SC_RESULT_OK;
  }
  catch (utils::ScException const & exception)
  {
    SC_LOG_ERROR(exception.Description());
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionNode, false);
    SC_LOG_INFO("StructureTranslationAgent finished with error");
    return SC_RESULT_ERROR;
  }
}

bool StructureTranslationAgent::checkActionClass(ScAddr const & actionNode)
{
  return m_memoryCtx.HelperCheckEdge(
      TranslationKeynodes::action_translate_structures_into_natural_language,
      actionNode,
      ScType::EdgeAccessConstPosPerm);
}

std::string StructureTranslationAgent::translateStructure(ScAddr const & structAddr, ScMemoryContext * context)
{
  auto TranslatorSet = std::make_unique<StructureTranslatorSet>(context);
  auto translation = TranslatorSet->translate(structAddr);
  return translation;
}
}  // namespace structureTranslationModule
