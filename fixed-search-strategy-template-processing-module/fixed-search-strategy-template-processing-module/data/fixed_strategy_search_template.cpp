#include "fixed_strategy_search_template.hpp"

#include <sc-memory/sc_oriented_set.hpp>
#include <sc-memory/sc_agent_context.hpp>

#include "keynodes/keynodes.hpp"

#include "parameterized_template_builder.hpp"

FixedStrategySearchTemplate::FixedStrategySearchTemplate(
    ScAgentContext & context,
    utils::ScLogger & logger,
    ScAddr const & templateAddr)
  : ParameterizedTemplate(context, logger, templateAddr)
{
  Load();
}

FixedStrategySearchTemplate::~FixedStrategySearchTemplate() = default;

void FixedStrategySearchTemplate::Load()
{
  ScAddrToValueUnorderedMap<ScAddr> elements;
  GetElementsByRoles({Keynodes::rrel_init_template, Keynodes::rrel_next_template}, elements);

  if (elements.count(Keynodes::rrel_init_template))
  {
    m_initTemplateAddr = elements[Keynodes::rrel_init_template];
    m_logger.Debug("Initial template found in template ", *this);
  }
  else
    m_logger.Debug("Initial template not found in template ", *this);

  if (elements.count(Keynodes::rrel_next_template))
  {
    m_nextTemplateAddr = elements[Keynodes::rrel_next_template];
    m_logger.Debug("Next template found in template ", *this);
  }
  else
    m_logger.Debug("Next template not found in template ", *this);
}

bool FixedStrategySearchTemplate::ApplyImpl(
    ScTemplateParams const & params,
    TemplateArguments const & arguments,
    TemplateResults & results,
    std::list<FilterCallback> const & callbacks) const
{
  results = TemplateResults{m_replyContext, m_logger, m_templateAddr, ScAddr::Empty, ScAddr::Empty, m_resultParamsAddr};
  bool status = true;

  m_logger.Debug("Process init template ", m_initTemplateAddr);
  auto const & initTemplate = ParameterizedTemplateBuilder::BuildTemplate(m_replyContext, m_logger, m_initTemplateAddr);

  TemplateResults initTemplateResults;
  if (!initTemplate->Apply(arguments, initTemplateResults))
  {
    m_logger.Debug("Init template ", m_initTemplateAddr, " not applied");
    return false;
  }
  m_logger.Debug("Init template ", m_initTemplateAddr, " applied");

  if (results.Size() == 0)
    results.AddTemplateResults(initTemplateResults);

  if (m_nextTemplateAddr.IsValid())
    status = ProcessNextTemplates(arguments, initTemplateResults, results);

  if (status)
    m_logger.Debug("Complex search template applied");
  else
    m_logger.Debug("Complex search template not applied");

  return status;
}

bool FixedStrategySearchTemplate::ProcessNextTemplates(
    TemplateArguments const & arguments,
    TemplateResults & initTemplateResults,
    TemplateResults & results) const
{
  m_logger.Debug("Process next template for init template ", m_initTemplateAddr);

  bool const isInitSearchSetTemplate =
      m_context->CheckConnector(Keynodes::nrel_search_set_template, m_initTemplateAddr, ScType::ConstPosArc);

  m_logger.Debug("Process next template for ", m_initTemplateAddr);
  bool const status = initTemplateResults.AllOf(
      [&](TemplateResult const & initTemplateResult) -> bool
      {
        m_logger.Debug("Process init template result ", initTemplateResult.GetIndex(), " for next template");

        bool status = true;

        TemplateArguments nextTemplateArguments{m_replyContext, m_logger};
        nextTemplateArguments.Add(arguments);
        nextTemplateArguments.Add(initTemplateResult);

        TemplateResults nextTemplateResults;
        auto const & nextTemplate =
            ParameterizedTemplateBuilder::BuildTemplate(m_replyContext, m_logger, m_nextTemplateAddr);

        if (!nextTemplate->Apply(nextTemplateArguments, nextTemplateResults))
        {
          m_logger.Debug("Next template ", m_nextTemplateAddr, " not applied");
          status = false;
          return status;
        }

        m_logger.Debug("Next template ", m_nextTemplateAddr, " applied");

        m_logger.Debug("Process next template results (count: ", nextTemplateResults.Size(), ")");

        m_logger.Debug("Next templates are ", (isInitSearchSetTemplate ? "set" : "not set"), " templates");
        if (isInitSearchSetTemplate)
        {
          m_logger.Debug("Connect next template results with init template result");
          results.ConnectTemplateResults(initTemplateResult, nextTemplateResults, !isInitSearchSetTemplate);
        }
        else
        {
          m_logger.Debug("Merge next template results with init template result");
          results.MergeTemplateResults(initTemplateResult, nextTemplateResults, !isInitSearchSetTemplate);
        }
        m_logger.Debug("Next template results processed");

        return status;
      });

  m_logger.Debug("Next templates for init template ", m_initTemplateAddr, " processed");

  return status;
}
