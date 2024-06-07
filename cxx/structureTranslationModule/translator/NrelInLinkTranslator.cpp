#include "NrelInLinkTranslator.hpp"

namespace structureTranslationModule
{
NrelInLinkTranslator::NrelInLinkTranslator(ScMemoryContext * context)
  : StructureTranslator(context)
{
}

std::vector<std::string> NrelInLinkTranslator::translate(ScAddr const & structAddr) const
{
  std::vector<std::string> translations;
  std::string const NODE_ALIAS = "_node";
  std::string const LINK_ALIAS = "_link";
  std::string const NREL_ALIAS = "_nrel";
  std::string const EDGE_ALIAS = "_edge";
  ScAddr linkNode, node, nrelNode;

  ScTemplate scTemplate;
  scTemplate.Triple(structAddr, ScType::EdgeAccessVarPosPerm, ScType::EdgeDCommonVar >> EDGE_ALIAS);
  scTemplate.Quintuple(
      ScType::NodeVar >> NODE_ALIAS,
      EDGE_ALIAS,
      ScType::LinkVar >> LINK_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      ScType::NodeVarNoRole >> NREL_ALIAS);
  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplate(scTemplate, searchResult);

  for (size_t i = 0; i < searchResult.Size(); i++)
  {
    auto const & result = searchResult[i];
    node = result[NODE_ALIAS];
    linkNode = result[LINK_ALIAS];
    nrelNode = result[NREL_ALIAS];
    std::string const & nodeMainIdtf = getEnglishMainIdtf(node);
    if (nodeMainIdtf.empty())
      continue;
    std::string const & nrelMainIdtf = getEnglishMainIdtf(nrelNode);
    if (nrelMainIdtf.empty())
      continue;
    std::string const & linkContent = getEnglishContent(linkNode);
    if (linkContent.empty())
      continue;
    translations.push_back(nodeMainIdtf + " " + nrelMainIdtf + " " + linkContent);
  }
  return translations;
}
}  // namespace structureTranslationModule
