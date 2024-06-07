#include "FromConceptTranslator.hpp"

namespace structureTranslationModule
{
FromConceptTranslator::FromConceptTranslator(ScMemoryContext * context)
  : StructureTranslator(context)
{
}

std::vector<std::string> FromConceptTranslator::translate(ScAddr const & structAddr) const
{
  std::vector<std::string> translations;
  std::string const NODE_ALIAS = "_node";
  std::string const CLASS_ALIAS = "_class";
  std::string const EDGE_ALIAS = "_edge";
  ScAddr classNode, node;

  ScTemplate scTemplate;
  scTemplate.Triple(ScType::NodeVarClass >> CLASS_ALIAS, ScType::EdgeAccessVarPosPerm >> EDGE_ALIAS, ScType::NodeVar >> NODE_ALIAS);
  scTemplate.Triple(structAddr, ScType::EdgeAccessVarPosPerm, EDGE_ALIAS);
  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplate(scTemplate, searchResult);

  for (size_t i = 0; i < searchResult.Size(); i++)
  {
    auto const & result = searchResult[i];
    classNode = result[CLASS_ALIAS];
    node = result[NODE_ALIAS];
    std::string const & classMainIdtf = getEnglishMainIdtf(classNode);
    if (classMainIdtf.empty())
      continue;
    std::string const & nodeMainIdtf = getEnglishMainIdtf(node);
    if (nodeMainIdtf.empty())
      continue;
    translations.push_back(nodeMainIdtf + " is " + classMainIdtf);
  }
  return translations;
}
}  // namespace structureTranslationModule
