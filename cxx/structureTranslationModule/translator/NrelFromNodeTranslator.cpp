#include "NrelFromNodeTranslator.hpp"

namespace structureTranslationModule
{
NrelFromNodeTranslator::NrelFromNodeTranslator(
    ScMemoryContext * context)
  : StructureTranslator(context)
{
}

std::vector<std::string> NrelFromNodeTranslator::translate(ScAddr const & structAddr) const
{
  std::vector<std::string> translations;
  std::string const NODE_ALIAS = "_node";
  std::string const SOURCE_ALIAS = "_source";
  std::string const NREL_ALIAS = "nrel";
  std::string const EDGE_ALIAS = "_edge";
  ScAddr sourceNode, node, nrelNode;

  ScTemplate scTemplate;
  scTemplate.Triple(structAddr, ScType::EdgeAccessVarPosPerm, ScType::EdgeDCommonVar >> EDGE_ALIAS);
  scTemplate.Quintuple(
      ScType::NodeVar >> SOURCE_ALIAS,
      EDGE_ALIAS,
      ScType::NodeVar >> NODE_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      ScType::NodeVarNoRole >> NREL_ALIAS);
  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplate(scTemplate, searchResult);

  for (size_t i = 0; i < searchResult.Size(); i++)
  {
    auto const & result = searchResult[i];
    node = result[NODE_ALIAS];
    sourceNode = result[SOURCE_ALIAS];
    nrelNode = result[NREL_ALIAS];
    std::string const & nodeMainIdtf = getEnglishMainIdtf(node);
    if (nodeMainIdtf.empty())
      continue;
    std::string const & sourceMainIdtf = getEnglishMainIdtf(sourceNode);
    if (sourceMainIdtf.empty())
      continue;
    std::string const & nrelMainIdtf = getEnglishMainIdtf(nrelNode);
    if (nrelMainIdtf.empty())
      continue;
    translations.push_back(sourceMainIdtf + " "  + nrelMainIdtf + " "  + nodeMainIdtf);
  }
  return translations;
}
}  // namespace structureTranslationModule
