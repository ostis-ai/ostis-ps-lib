#include "generate_template.hpp"

#include <sc-memory/sc_agent_context.hpp>

GenerateTemplate::GenerateTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr)
  : ParameterizedTemplate(context, logger, templateAddr)
{
}

GenerateTemplate::~GenerateTemplate() = default;

bool GenerateTemplate::ApplyImpl(
    ScTemplateParams const & params,
    TemplateArguments const & arguments,
    TemplateResults & results,
    std::list<FilterCallback> const & callbacks) const
{
  ScTemplateGenResult genResult;
  if (TryGenerateByTemplate(params, genResult))
  {
    m_logger.Debug("Generation by generate template ", *this, " succeeded");
    results = TemplateResults{
        m_replyContext, m_logger, m_templateAddr, m_sortParamAddr, m_eraseParamsAddr, m_resultParamsAddr};
    return results.CollectFromGenResult(genResult);
  }
  m_logger.Debug("Generation by generate template ", *this, " failed");
  return false;
}

bool GenerateTemplate::TryGenerateByTemplate(ScTemplateParams const & params, ScTemplateGenResult & genResult) const
{
  m_logger.Debug("Build template ", *this);
  ScTemplate genTemplate;
  m_replyContext.BuildTemplate(genTemplate, m_templateAddr);

  m_logger.Debug("Generate by template ", *this);
  m_replyContext.GenerateByTemplate(genTemplate, genResult, params);
  m_logger.Debug("Generation by template ", *this, " completed");

  m_logger.Debug("Generation results for ", *this, " formed");
  return true;
}
