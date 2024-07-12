/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */
 
#include "NrelFromNodeTranslator.hpp"
#include "constants/TranslationConstants.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

namespace structureTranslationModule
{
NrelFromNodeTranslator::NrelFromNodeTranslator(ScMemoryContext * context)
  : StructureTranslator(context)
{
}


std::stringstream NrelFromNodeTranslator::translate(ScAddr const & structAddr) const
{
  std::stringstream translations;
  ScAddr sourceNode;
  ScAddr node;
  ScAddr nrelNode;

  ScTemplate scTemplate;
  scTemplate.Triple(structAddr, ScType::EdgeAccessVarPosPerm, ScType::EdgeDCommonVar >> TranslationConstants::EDGE_ALIAS);
  scTemplate.Quintuple(
      ScType::NodeVar >> TranslationConstants::SOURCE_ALIAS,
      TranslationConstants::EDGE_ALIAS,
      ScType::NodeVar >> TranslationConstants::NODE_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      ScType::NodeVarNoRole >> TranslationConstants::NREL_ALIAS);
  context->HelperSmartSearchTemplate(scTemplate,
  [&](ScTemplateResultItem const & searchResult)
  {
    node = searchResult[TranslationConstants::NODE_ALIAS];
    sourceNode = searchResult[TranslationConstants::SOURCE_ALIAS];
    nrelNode = searchResult[TranslationConstants::NREL_ALIAS];
    std::string const & nodeMainIdtf = utils::CommonUtils::getMainIdtf(context, node, {scAgentsCommon::CoreKeynodes::lang_ru});
    if (nodeMainIdtf.empty())
      return ScTemplateSearchRequest::CONTINUE;
    std::string const & sourceMainIdtf = utils::CommonUtils::getMainIdtf(context, sourceNode, {scAgentsCommon::CoreKeynodes::lang_ru});
    if (sourceMainIdtf.empty())
      return ScTemplateSearchRequest::CONTINUE;
    std::string const & nrelMainIdtf = utils::CommonUtils::getMainIdtf(context, nrelNode, {scAgentsCommon::CoreKeynodes::lang_ru});
    if (nrelMainIdtf.empty())
      return ScTemplateSearchRequest::CONTINUE;
    translations << sourceMainIdtf << " "  << nrelMainIdtf << " "  << nodeMainIdtf << ". ";
    return ScTemplateSearchRequest::CONTINUE;
  },
  [&](ScAddr const & element)
  {
    return isInStructure(structAddr, element);
  });

  return translations;
}
}  // namespace structureTranslationModule
