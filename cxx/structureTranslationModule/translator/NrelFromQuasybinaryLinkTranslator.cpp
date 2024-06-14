/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */
 
#include "NrelFromQuasybinaryLinkTranslator.hpp"
#include "constants/TranslationConstants.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"
#include "keynodes/TranslationKeynodes.hpp"

namespace structureTranslationModule
{
NrelFromQuasybinaryLinkTranslator::NrelFromQuasybinaryLinkTranslator(ScMemoryContext * context)
  : StructureTranslator(context)
{
}

std::string NrelFromQuasybinaryLinkTranslator::translate(ScAddr const & structAddr) const
{
  SC_LOG_INFO("NrelFromQuasybinaryLinkTranslator started");
  std::string translations;
  ScAddr tupleNode, node, nrelNode;

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
    std::string const & nodeMainIdtf = utils::CommonUtils::getMainIdtf(context, node, {TranslationKeynodes::lang_en});
    if (nodeMainIdtf.empty())
      return ScTemplateSearchRequest::CONTINUE;
    std::string const & nrelMainIdtf = utils::CommonUtils::getMainIdtf(context, nrelNode, {TranslationKeynodes::lang_en});
    if (nrelMainIdtf.empty())
      return ScTemplateSearchRequest::CONTINUE;
    
    auto const & linkIterator = context->Iterator3(tupleNode, ScType::EdgeAccessConstPosPerm, ScType::LinkConst);
    while (linkIterator->Next())
    {
      ScAddr const & linkNode = linkIterator->Get(2);
      std::string const & linkContent = getEnglishContent(linkNode);
      if (linkContent.empty())
        continue;
      translations += nodeMainIdtf + " " + nrelMainIdtf + " " + linkContent + ". ";
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
