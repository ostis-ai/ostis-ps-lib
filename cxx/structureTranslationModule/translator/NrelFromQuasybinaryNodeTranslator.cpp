/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */
 
#include "NrelFromQuasybinaryNodeTranslator.hpp"
#include "constants/TranslationConstants.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"
#include "keynodes/TranslationKeynodes.hpp"

namespace structureTranslationModule
{
NrelFromQuasybinaryNodeTranslator::NrelFromQuasybinaryNodeTranslator(ScMemoryContext * context)
  : StructureTranslator(context)
{
}

std::string NrelFromQuasybinaryNodeTranslator::translate(ScAddr const & structAddr) const
{
  SC_LOG_INFO("NrelFromQuasybinaryNodeTranslator started");
  std::string translations;
  ScAddr tupleNode, node, nrelNode;

  ScTemplate scTemplate;
  scTemplate.Triple(structAddr, ScType::EdgeAccessVarPosPerm, ScType::EdgeDCommonVar >> TranslationConstants::EDGE_ALIAS);
  scTemplate.Quintuple(
      ScType::NodeVarTuple >> TranslationConstants::TUPLE_ALIAS,
      TranslationConstants::EDGE_ALIAS, 
      ScType::NodeVar >> TranslationConstants::NODE_ALIAS, 
      ScType::EdgeAccessVarPosPerm, 
      ScType::NodeVarNoRole >> TranslationConstants::NREL_ALIAS);
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

    auto const & tupleNodeIterator = context->Iterator3(tupleNode, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);

    while (tupleNodeIterator->Next())
    {
      ScAddr const & tupleNode = tupleNodeIterator->Get(2);
      std::string const & tupleNodeMainIdtf = utils::CommonUtils::getMainIdtf(context, tupleNode, {TranslationKeynodes::lang_en});
      if (tupleNodeMainIdtf.empty())
        continue;
      translations += nodeMainIdtf + " " + nrelMainIdtf + " " + tupleNodeMainIdtf + ". ";
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
