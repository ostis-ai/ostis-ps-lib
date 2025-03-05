#include "common/utils/template_params_utils.hpp"

using namespace common;

ScTemplateParams TemplateParamsUtils::CreateTemplateParamsFromReplacements(
    std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements)
{
  ScTemplateParams templateParams;
  for (auto const & [varAddr, value] : replacements)
    templateParams.Add(varAddr, value);

  return templateParams;
}

ScTemplateParams TemplateParamsUtils::CreateTemplateParamsFromReplacements(
    ScMemoryContext * context,
    std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements,
    ScAddr const & structure)
{
  ScTemplateParams templateParams;
  for (auto const & [varAddr, value] : replacements)
  {
    if (context->CheckConnector(structure, varAddr, ScType::ConstPermPosArc))
      templateParams.Add(varAddr, value);
  }

  return templateParams;
}
