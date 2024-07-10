#include "SemanticNeighbourhoodTranslator.hpp"
#include "keynodes/TranslationKeynodes.hpp"

#include "NodeDefinitionTranslator.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "sc-memory/sc_addr.hpp"
#include "sc-memory/utils/sc_log.hpp"


namespace naturalLanguageProcessingModule
{
NodeDefinitionTranslator::NodeDefinitionTranslator(ScMemoryContext * context)
  : SemanticNeighbourhoodTranslator(context)
{
}

std::vector<std::string> NodeDefinitionTranslator::getSemanticNeighbourhoods(

    ScAddr const & node,
    size_t const & maxTranslations,
    ScAddrSet const & structure,
    ScAddrSet const & atLeastOneNodeFromConstruction) const
{
  std::vector<std::string> translations;
  ScAddr definitionNode;
  std::string TRANSLATION_ALIAS = "_text_translation_node";
  std::string NODE_ALIAS = "_node";
  std::string TEXT_TRANSLATION_ALIAS = "_text_translation_node";
  std::string EXAMPLE_ALIAS = "_example_link";

  ScTemplate scTemplate;
  scTemplate.Quintuple(
      ScType::NodeVar >> TEXT_TRANSLATION_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      node,
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::rrel_key_sc_element);
  scTemplate.Quintuple(
      ScType::NodeVar >> NODE_ALIAS,
      ScType::EdgeDCommonVar,
      TEXT_TRANSLATION_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::nrel_sc_text_translation);
  scTemplate.Quintuple(
      NODE_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      ScType::LinkVar >> EXAMPLE_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      ScType::NodeVarRole);
  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplate(scTemplate, searchResult);
  for (size_t i = 0; i < searchResult.Size(); i++)
  {
    auto const & result = searchResult[i];
    definitionNode = result[EXAMPLE_ALIAS];

    std::string definitionContent;
    context->GetLinkContent(definitionNode, definitionContent);
      if (definitionContent.empty())
        continue;

    translations.push_back(definitionContent);
  }
  return translations;
}

bool NodeDefinitionTranslator::isParameter(ScAddr const & node) const
{
  std::string const & NODE_ALIAS = "_node;";
  ScTemplate scTemplate;
  scTemplate.Triple(TranslationKeynodes::parameter, ScType::EdgeAccessVarPosPerm, ScType::NodeVarClass >> NODE_ALIAS);
  scTemplate.Triple(NODE_ALIAS, ScType::EdgeAccessVarPosPerm, node);
  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplate(scTemplate, searchResult);
  return searchResult.IsEmpty() == SC_FALSE;
}

std::list<ScAddrVector> NodeDefinitionTranslator::getSemanticNeighbourhoodsElements(
    ScAddr const & node,
    ScAddrSet const & structure) const
{
  std::list<ScAddrVector> answer;
  ScAddr definitionNode;
  std::string TRANSLATION_ALIAS = "_text_translation_node";
  std::string NODE_ALIAS = "_node";
  std::string TEXT_TRANSLATION_ALIAS = "_text_translation_node";
  std::string EXAMPLE_ALIAS = "_example_link";

  ScTemplate scTemplate;
  scTemplate.Quintuple(
      ScType::NodeVar >> TEXT_TRANSLATION_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      node,
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::rrel_key_sc_element);
  scTemplate.Quintuple(
      ScType::NodeVar >> NODE_ALIAS,
      ScType::EdgeDCommonVar,
      TEXT_TRANSLATION_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::nrel_sc_text_translation);
  scTemplate.Quintuple(
      NODE_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      ScType::LinkVar >> EXAMPLE_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      ScType::NodeVarRole);
  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplate(scTemplate, searchResult);
  for (size_t i = 0; i < searchResult.Size(); i++)
  {
    auto const & result = searchResult[i];
    answer.push_back({result[EXAMPLE_ALIAS]});
  }
  return answer;
}
}  // namespace naturalLanguageProcessingModule