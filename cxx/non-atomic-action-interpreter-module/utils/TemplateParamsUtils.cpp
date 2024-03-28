#include "TemplateParamsUtils.hpp"

using namespace nonAtomicActionInterpreterModule;

ScTemplateParams TemplateParamsUtils::createTemplateParamsFromReplacements(
    std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements,
    std::map<ScAddr, std::string, ScAddrLessFunc> const & resolvedVariableIdentifiers)
{
  ScTemplateParams templateParams;
  for (auto const & variableReplacementPair : replacements)
  {
    std::string const & variableIdentifier = resolvedVariableIdentifiers.at(variableReplacementPair.first);
    templateParams.Add(variableIdentifier, variableReplacementPair.second);
  }

  return templateParams;
}

ScTemplateParams TemplateParamsUtils::createTemplateParamsFromReplacements(
    ScMemoryContext * context,
    std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements,
    std::map<ScAddr, std::string, ScAddrLessFunc> const & resolvedVariableIdentifiers,
    ScAddr const & structure)
{
  ScTemplateParams templateParams;
  for (auto const & variableReplacementPair : replacements)
  {
    if (context->HelperCheckEdge(structure, variableReplacementPair.first, ScType::EdgeAccessConstPosPerm))
    {
      std::string const & variableIdentifier = resolvedVariableIdentifiers.at(variableReplacementPair.first);
      templateParams.Add(variableIdentifier, variableReplacementPair.second);
    }
  }

  return templateParams;
}
