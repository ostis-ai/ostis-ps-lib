#include "sc-agents-common/utils/CommonUtils.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include <iterator>
#include "constants/TranslationConstants.hpp"

#include "keynodes/TranslationKeynodes.hpp"

#include "StructureTranslationAgent.hpp"

#include<string>

namespace structureTranslationModule
{

SC_AGENT_IMPLEMENTATION(StructureTranslationAgent)
{
  ScAddr const & actionNode = otherAddr;
  try
  {
    if (checkActionClass(actionNode) == SC_FALSE)
      return SC_RESULT_OK;
    SC_LOG_DEBUG("structureTranslationAgent started");

    ScAddr const & nodeAddr = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, actionNode, scAgentsCommon::CoreKeynodes::rrel_1);

    std::vector<std::string> translations;

    auto const & structIterator = m_memoryCtx.Iterator3(nodeAddr, ScType::EdgeAccessConstPosPerm, ScType::NodeStruct);
    while (structIterator->Next())
    {
      ScAddr const & structAddr = structIterator->Get(2);
      translations.push_back(translateStructure(structAddr, &m_memoryCtx));
    }

    std::string const & translation = join(translations.cbegin(), translations.cend(), TranslationConstants::STRUCTURES_DELIMITER);

    ScAddr const & translationLink = m_memoryCtx.CreateLink();
    if (translationLink.IsValid() == SC_FALSE)
      SC_THROW_EXCEPTION(utils::ScException, "structureTranslationAgent: cannot create answer link");
    if (m_memoryCtx.SetLinkContent(translationLink, translation) == SC_FALSE)
      SC_THROW_EXCEPTION(utils::ScException, "structureTranslationAgent: cannot set link content");

    ScAddrVector const & answerElements = {translationLink};

    if (answerElements.empty())
      SC_THROW_EXCEPTION(utils::ScException, "structureTranslationAgent: answer is empty");

    utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionNode, answerElements, true);
    SC_LOG_DEBUG("structureTranslationAgent finished");
    return SC_RESULT_OK;
  }
  catch (utils::ScException const & exception)
  {
    SC_LOG_ERROR(exception.Description());
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionNode, false);
    SC_LOG_DEBUG("structureTranslationAgent finished with error");
    return SC_RESULT_ERROR;
  }
}

bool StructureTranslationAgent::checkActionClass(ScAddr const & actionNode)
{
  return m_memoryCtx.HelperCheckEdge(
      TranslationKeynodes::action_translate_structures, actionNode, ScType::EdgeAccessConstPosPerm);
}

std::string StructureTranslationAgent::join(
    std::vector<std::string>::const_iterator const & cbegin,
    std::vector<std::string>::const_iterator const & cend,
    std::string const & delimiter)
{
  std::ostringstream os;
  std::copy(cbegin, cend, std::ostream_iterator<std::string>(os, delimiter.c_str()));
  std::string joined = os.str();
  if (joined.size() > delimiter.size())
    joined.erase(joined.size() - delimiter.size());
  return joined;
}

std::string StructureTranslationAgent::translateStructure(ScAddr const & structAddr, ScMemoryContext * context)
{
  auto TranslatorSet = std::make_unique<StructureTranslatorSet>(context);
  auto translations = TranslatorSet->translate(structAddr);
  std::string const & translation = join(translations.cbegin(), translations.cend(), TranslationConstants::TRANSLATIONS_DELIMITER);
  return translation;
}
}  // namespace structureTranslationModule
