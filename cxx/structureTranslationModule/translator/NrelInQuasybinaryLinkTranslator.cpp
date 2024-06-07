#include "NrelInQuasybinaryLinkTranslator.hpp"

namespace structureTranslationModule
{
NrelInQuasybinaryLinkTranslator::NrelInQuasybinaryLinkTranslator(
    ScMemoryContext * context)
  : StructureTranslator(context)
{
}

std::vector<std::string> NrelInQuasybinaryLinkTranslator::translate(ScAddr const & structAddr) const
{
  std::vector<std::string> translations;
  std::string const TUPLE_ALIAS = "_tuple";
  std::string const NODE_ALIAS = "_node";
  std::string const NREL_ALIAS = "_nrel";
  std::string const EDGE_ALIAS = "_edge";
  ScAddr tupleNode, node, nrelNode;

  ScTemplate scTemplate;
  scTemplate.Triple(structAddr, ScType::EdgeAccessVarPosPerm, ScType::EdgeDCommonVar >> EDGE_ALIAS);
  scTemplate.Quintuple(
      ScType::NodeVar >> NODE_ALIAS, 
      EDGE_ALIAS, 
      ScType::NodeVarTuple >> TUPLE_ALIAS,
      ScType::EdgeAccessVarPosPerm, 
      ScType::NodeVarNoRole >> NREL_ALIAS);
  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplate(scTemplate, searchResult);

  for (size_t i = 0; i < searchResult.Size(); i++)
  {
    auto const & result = searchResult[i];
    node = result[NODE_ALIAS];
    tupleNode = result[TUPLE_ALIAS];
    nrelNode = result[NREL_ALIAS];
    std::string const & nodeMainIdtf = getEnglishMainIdtf(node);
    if (nodeMainIdtf.empty())
      continue;
    std::string const & nrelMainIdtf = getEnglishMainIdtf(nrelNode);
    if (nrelMainIdtf.empty())
      continue;
    auto const & linkIterator = context->Iterator3(tupleNode, ScType::EdgeAccessConstPosPerm, ScType::LinkConst);
    while (linkIterator->Next())
    {
      ScAddr const & linkNode = linkIterator->Get(2);
      std::string const & linkContent = getEnglishContent(linkNode);
      if (linkContent.empty())
        continue;
      translations.push_back(nodeMainIdtf + " " + nrelMainIdtf + " " + linkContent);
    }
  }
  return translations;
}
}  // namespace structureTranslationModule
