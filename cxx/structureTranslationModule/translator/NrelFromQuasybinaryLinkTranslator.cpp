/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */
 
#include "NrelFromQuasybinaryLinkTranslator.hpp"
#include "constants/TranslationConstants.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

namespace structureTranslationModule
{
NrelFromQuasybinaryLinkTranslator::NrelFromQuasybinaryLinkTranslator(ScMemoryContext * context)
  : StructureTranslator(context)
{
}

std::stringstream NrelFromQuasybinaryLinkTranslator::translate(ScAddr const & structAddr) const
{
  std::stringstream translations;
  ScAddr tupleNode;
  ScAddr node;
  ScAddr nrelNode;

  ScTemplate scTemplate;
  scTemplate.Quintuple(
      ScType::NodeVarTuple >> TranslationConstants::TUPLE_ALIAS,
      ScType::EdgeDCommonVar >> TranslationConstants::EDGE_ALIAS, 
      ScType::NodeVar >> TranslationConstants::NODE_ALIAS, 
      ScType::EdgeAccessVarPosPerm, 
      ScType::NodeVarNoRole >> TranslationConstants::NREL_ALIAS);
  scTemplate.Triple(structAddr, ScType::EdgeAccessVarPosPerm, TranslationConstants::EDGE_ALIAS);
  context->HelperSmartSearchTemplate(scTemplate,
  [&](ScTemplateResultItem const & searchResult)
  {
    node = searchResult[TranslationConstants::NODE_ALIAS];
    tupleNode = searchResult[TranslationConstants::TUPLE_ALIAS];
    nrelNode = searchResult[TranslationConstants::NREL_ALIAS];
    std::string const & nodeMainIdtf = utils::CommonUtils::getMainIdtf(context, node, {scAgentsCommon::CoreKeynodes::lang_ru});
    if (nodeMainIdtf.empty())
      return ScTemplateSearchRequest::CONTINUE;
    std::string const & nrelMainIdtf = utils::CommonUtils::getMainIdtf(context, nrelNode, {scAgentsCommon::CoreKeynodes::lang_ru});
    if (nrelMainIdtf.empty())
      return ScTemplateSearchRequest::CONTINUE;
    
    auto const & linkIterator = context->Iterator3(tupleNode, ScType::EdgeAccessConstPosPerm, ScType::LinkConst);
    while (linkIterator->Next())
    {
      ScAddr const & linkNode = linkIterator->Get(2);
      std::string linkContent;
      context->GetLinkContent(linkNode, linkContent);
      if (linkContent.empty())
        continue;
      translations << nodeMainIdtf << " " << nrelMainIdtf << " " << linkContent << ". ";
    }
    return ScTemplateSearchRequest::CONTINUE;
  },
  [&](ScAddr const & element)
  {
    return isInStructure(structAddr, element);
  });

  return translations;
}
}  // namespace structureTranslationModule
