#include "TemplateParamsUtils.hpp"

using namespace nonAtomicActionInterpreterModule;

ScTemplateParams TemplateParamsUtils::createTemplateParamsFromReplacements(
    std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements)
{
  ScTemplateParams templateParams;
  for (auto const & [varAddr, value] : replacements)
  {
    templateParams.Add(varAddr, value);
  }

  return templateParams;
}

ScTemplateParams TemplateParamsUtils::createTemplateParamsFromReplacements(
    ScMemoryContext * context,
    std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements,
    ScAddr const & structure)
{
  ScTemplateParams templateParams;
  for (auto const & [varAddr, value] : replacements)
  {
    if (context->HelperCheckEdge(structure, varAddr, ScType::EdgeAccessConstPosPerm))
    {
      templateParams.Add(varAddr, value);
    }
  }

  return templateParams;
}
