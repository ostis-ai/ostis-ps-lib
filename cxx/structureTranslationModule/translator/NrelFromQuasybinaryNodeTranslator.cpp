#include "NrelFromQuasybinaryNodeTranslator.hpp"

namespace structureTranslationModule
{
NrelFromQuasybinaryNodeTranslator::NrelFromQuasybinaryNodeTranslator(ScMemoryContext * context)
  : StructureTranslator(context)
{
}

std::vector<std::string> NrelFromQuasybinaryNodeTranslator::translate(ScAddr const & structAddr) const
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
      ScType::NodeVarTuple >> TUPLE_ALIAS,
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
    tupleNode = result[TUPLE_ALIAS];
    nrelNode = result[NREL_ALIAS];
    std::string const & nodeMainIdtf = getEnglishMainIdtf(node);
    if (nodeMainIdtf.empty())
      continue;
    std::string const & nrelMainIdtf = getEnglishMainIdtf(nrelNode);
    if (nrelMainIdtf.empty())
      continue;

    auto const & tupleNodeIterator = context->Iterator3(tupleNode, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);

    while (tupleNodeIterator->Next())
    {
      ScAddr const & tupleNode = tupleNodeIterator->Get(2);
      std::string const & tupleNodeMainIdtf = getEnglishMainIdtf(tupleNode);
      if (tupleNodeMainIdtf.empty())
        continue;
      translations.push_back(nodeMainIdtf + " " + nrelMainIdtf + " " + tupleNodeMainIdtf);
    }
  }
  return translations;
}
}  // namespace structureTranslationModule
