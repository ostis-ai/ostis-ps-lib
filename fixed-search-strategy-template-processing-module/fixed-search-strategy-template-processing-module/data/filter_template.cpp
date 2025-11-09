#include "filter_template.hpp"

#include <sc-memory/sc_agent_context.hpp>

FilterTemplate::FilterTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr)
  : SearchTemplate(context, logger, templateAddr)
{
}

FilterTemplate::~FilterTemplate() = default;

bool FilterTemplate::ApplyImpl(
    ScTemplateParams const & params,
    TemplateArguments const & arguments,
    TemplateResults & results,
    std::list<FilterCallback> const & callbacks) const
{
  if (SearchTemplate::ApplyImpl(params, arguments, results, callbacks))
  {
    m_logger.Debug("Searching by filter template ", *this, " failed");
    return false;
  }
  m_logger.Debug("Searching by filter template ", *this, " succeeded");
  return true;
}
