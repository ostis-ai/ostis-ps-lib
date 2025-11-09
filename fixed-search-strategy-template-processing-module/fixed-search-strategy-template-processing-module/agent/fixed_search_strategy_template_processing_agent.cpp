#include "fixed_search_strategy_template_processing_agent.hpp"

#include "keynodes/keynodes.hpp"

#include "data/parameterized_template_builder.hpp"

FixedSearchStrategyTemplateProcessingAgent::FixedSearchStrategyTemplateProcessingAgent()
{
  m_logger = utils::ScLogger(
      utils::ScLogger::ScLogType::File,
      "logs/fixed_search_strategy_template_processing_agent.log",
      utils::ScLogLevel::Debug,
      true);
}

ScAddr FixedSearchStrategyTemplateProcessingAgent::GetActionClass() const
{
  return Keynodes::action_process_fixed_search_strategy_template;
}

ScResult FixedSearchStrategyTemplateProcessingAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  auto [templateAddr, argumentsAddr] = action.GetArguments<2>();
  if (!templateAddr.IsValid())
  {
    m_logger.Error("Template is not specified");
    return action.FinishWithError();
  }

  TemplateArguments arguments{m_context, m_logger};
  if (argumentsAddr.IsValid())
    arguments.CollectFromSet(m_context.ConvertToSet(argumentsAddr));

  auto const & templ = ParameterizedTemplateBuilder::BuildTemplate(m_context, m_logger, templateAddr);
  TemplateResults results;
  bool const status = templ->Apply(arguments, results);

  ScStructure result = m_context.GenerateStructure();
  results.IterateAll(
      [&](ScAddr const & addr)
      {
        if (!m_context.CheckConnector(result, addr, ScType::ConstPermPosArc))
          m_context.GenerateConnector(ScType::ConstPermPosArc, result, addr);
      });

  action.SetResult(result);
  return status ? action.FinishSuccessfully() : action.FinishUnsuccessfully();
}
