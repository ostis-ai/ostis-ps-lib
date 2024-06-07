#include "sc-agents-common/utils/CommonUtils.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "keynodes/TranslationKeynodes.hpp"

#include "StructureTranslator.hpp"

namespace structureTranslationModule
{
size_t StructureTranslator::hashCode() const
{
  return std::hash<std::string>()(typeid(*this).name());
}

bool StructureTranslator::operator<(StructureTranslator const & other) const
{
  return this->hashCode() < other.hashCode();
}

std::string StructureTranslator::getEnglishContent(ScAddr const & linkNode) const
{
  std::string content;
  if (isEnglish(linkNode))
    context->GetLinkContent(linkNode, content);
  return content;
}

ScIterator5Ptr StructureTranslator::getNrelMainIdtfIterator(ScAddr const & node) const
{
  return context->Iterator5(
      node,
      ScType::EdgeDCommonConst,
      ScType::LinkConst,
      ScType::EdgeAccessConstPosPerm,
      scAgentsCommon::CoreKeynodes::nrel_main_idtf);
}

bool StructureTranslator::isEnglish(ScAddr const & node) const
{
  return context->HelperCheckEdge(TranslationKeynodes::lang_en, node, ScType::EdgeAccessConstPosPerm);
}

std::string StructureTranslator::getEnglishMainIdtf(ScAddr const & node) const
{
  std::string mainIdtf;
  auto const & iterator = getNrelMainIdtfIterator(node);
  while (iterator->Next())
  {
    mainIdtf = getEnglishContent(iterator->Get(2));
    if (mainIdtf.empty() == SC_FALSE)
      break;
  }
  return mainIdtf;
}
}  // namespace structureTranslationModule
