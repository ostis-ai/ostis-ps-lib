#include "not_filter_template.hpp"

#include <sc-memory/sc_agent_context.hpp>

NotFilterTemplate::NotFilterTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr)
  : NotSearchTemplate(context, logger, templateAddr)
{
}

NotFilterTemplate::~NotFilterTemplate() = default;

bool NotFilterTemplate::ApplyImpl(
    ScTemplateParams const & params,
    TemplateArguments const & arguments,
    TemplateResults & results,
    std::list<FilterCallback> const & callbacks) const
{
  if (NotSearchTemplate::ApplyImpl(params, arguments, results, callbacks))
  {
    m_logger.Debug("Searching by not filter template ", m_templateAddr, " succeeded");
    return true;
  }
  m_logger.Debug("Searching by not filter template ", m_templateAddr, " failed");
  return false;
}
