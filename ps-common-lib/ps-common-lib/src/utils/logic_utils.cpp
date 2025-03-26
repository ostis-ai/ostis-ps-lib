#include "ps-common-lib/utils/logic_utils.hpp"

#include <sc-agents-common/utils/CommonUtils.hpp>

#include "ps-common-lib/keynodes.hpp"
#include "ps-common-lib/utils/template_params_utils.hpp"

using namespace common;

bool LogicUtils::CheckLogicalFormula(
    ScMemoryContext * context,
    ScAddr const & logicFormula,
    std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements)
{
  bool result = false;

  if (utils::CommonUtils::checkType(context, logicFormula, ScType::ConstNodeStructure)
      && context->CheckConnector(Keynodes::atomic_logical_formula, logicFormula, ScType::ConstPermPosArc))
  {
    ScTemplateParams formulaTemplateParams =
        TemplateParamsUtils::CreateTemplateParamsFromReplacements(context, replacements, logicFormula);

    ScTemplate formulaTemplate;
    context->BuildTemplate(formulaTemplate, logicFormula, formulaTemplateParams);
    ScTemplateSearchResult formulaSearchResult;
    context->SearchByTemplate(formulaTemplate, formulaSearchResult);

    result = !formulaSearchResult.IsEmpty();
  }
  else
    SC_THROW_EXCEPTION(
        utils::ExceptionNotImplemented,
        "LogicUtils: the statement cannot be checked - only atomic logical formulas are currently supported.");

  return result;
}
