/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "NrelInLinkTranslator.hpp"
#include "constants/TranslationConstants.hpp"
#include "keynodes/TranslationKeynodes.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

namespace structureTranslationModule
{
NrelInLinkTranslator::NrelInLinkTranslator(ScMemoryContext * context)
  : StructureTranslator(context)
{
}

std::stringstream NrelInLinkTranslator::translate(ScAddr const & structAddr) const
{
  std::stringstream translations;
  ScAddr linkNode;
  ScAddr node;
  ScAddr nrelNode;

  ScTemplate scTemplate;
  scTemplate.Triple(
      structAddr, ScType::EdgeAccessVarPosPerm, ScType::EdgeDCommonVar >> TranslationConstants::EDGE_ALIAS);
  scTemplate.Quintuple(
      ScType::NodeVar >> TranslationConstants::NODE_ALIAS,
      TranslationConstants::EDGE_ALIAS,
      ScType::LinkVar >> TranslationConstants::LINK_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      ScType::NodeVarNoRole >> TranslationConstants::NREL_ALIAS);
  context->HelperSmartSearchTemplate(
      scTemplate,
      [&](ScTemplateResultItem const & searchResult)
      {
        node = searchResult[TranslationConstants::NODE_ALIAS];
        linkNode = searchResult[TranslationConstants::LINK_ALIAS];
        nrelNode = searchResult[TranslationConstants::NREL_ALIAS];
        if (context->HelperCheckEdge(
                TranslationKeynodes::translation_ignored_keynodes, nrelNode, ScType::EdgeAccessConstPosPerm))
          return ScTemplateSearchRequest::CONTINUE;
        std::string const & nodeMainIdtf =
            utils::CommonUtils::getMainIdtf(context, node, {scAgentsCommon::CoreKeynodes::lang_ru});
        if (nodeMainIdtf.empty())
          return ScTemplateSearchRequest::CONTINUE;
        std::string const & nrelMainIdtf =
            utils::CommonUtils::getMainIdtf(context, nrelNode, {scAgentsCommon::CoreKeynodes::lang_ru});
        if (nrelMainIdtf.empty())
          return ScTemplateSearchRequest::CONTINUE;
        std::string linkContent;
        if (context->HelperCheckEdge(scAgentsCommon::CoreKeynodes::lang_ru, linkNode, ScType::EdgeAccessConstPosPerm))
          context->GetLinkContent(linkNode, linkContent);
        if (linkContent.empty())
          return ScTemplateSearchRequest::CONTINUE;
        translations << nodeMainIdtf << " " << nrelMainIdtf << " " << linkContent << ". ";
        return ScTemplateSearchRequest::CONTINUE;
      },
      [&](ScAddr const & element)
      {
        return isInStructure(structAddr, element);
      });
  return translations;
}
}  // namespace structureTranslationModule
