#include "sc-agents-common/utils/CommonUtils.hpp"

#include "keynodes/Keynodes.hpp"
#include "utils/TemplateParamsUtils.hpp"

#include "LogicFormulaManager.hpp"

using namespace nonAtomicActionInterpreterModule;

LogicFormulaManager::LogicFormulaManager(ScMemoryContext * context)
  : context(context)
{
}

bool LogicFormulaManager::checkLogicalFormula(
    ScAddr const & logicFormula,
    std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements,
    std::map<ScAddr, std::string, ScAddrLessFunc> const & resolvedVariableIdentifiers)
{
  bool result = false;

  if (utils::CommonUtils::checkType(context, logicFormula, ScType::NodeConstStruct) &&
      context->HelperCheckEdge(Keynodes::atomic_logical_formula, logicFormula, ScType::EdgeAccessConstPosPerm))
  {
    ScTemplateParams formulaTemplateParams = TemplateParamsUtils::createTemplateParamsFromReplacements(
        context, replacements, resolvedVariableIdentifiers, logicFormula);

    ScTemplate formulaTemplate;
    if (!context->HelperBuildTemplate(formulaTemplate, logicFormula, formulaTemplateParams))
      throw std::runtime_error("LogicFormulaSearcher: the logic formula template cannot be built.");
    ScTemplateSearchResult formulaSearchResult;
    context->HelperSearchTemplate(formulaTemplate, formulaSearchResult);

    result = !formulaSearchResult.IsEmpty();
  }
  else
    SC_LOG_DEBUG(
        "LogicFormulaSearcher: the statement cannot be checked- "
        "only atomic logical formulas are currently supported.");

  return result;
}
