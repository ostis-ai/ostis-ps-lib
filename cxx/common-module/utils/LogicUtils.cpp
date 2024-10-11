#include "LogicUtils.hpp"

#include "sc-agents-common/utils/CommonUtils.hpp"
#include "keynodes/Keynodes.hpp"
#include "utils/TemplateParamsUtils.hpp"

using namespace commonModule;

bool LogicUtils::checkLogicalFormula(
    ScMemoryContext * context,
    ScAddr const & logicFormula,
    std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements)
{
  bool result = false;

  if (utils::CommonUtils::checkType(context, logicFormula, ScType::ConstNodeStructure) &&
      context->CheckConnector(Keynodes::atomic_logical_formula, logicFormula, ScType::ConstPermPosArc))
  {
    ScTemplateParams formulaTemplateParams =
        TemplateParamsUtils::createTemplateParamsFromReplacements(context, replacements, logicFormula);

    ScTemplate formulaTemplate;
    context->BuildTemplate(formulaTemplate, logicFormula, formulaTemplateParams);
    ScTemplateSearchResult formulaSearchResult;
    context->SearchByTemplate(formulaTemplate, formulaSearchResult);

    result = !formulaSearchResult.IsEmpty();
  }
  else
    SC_LOG_DEBUG("LogicUtils: the statement cannot be checked - only atomic logical formulas are currently supported.");

  return result;
}
