#include "not_search_template.hpp"

#include <sc-memory/sc_agent_context.hpp>

NotSearchTemplate::NotSearchTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr)
  : SearchTemplate(context, logger, templateAddr)
{
}

NotSearchTemplate::~NotSearchTemplate() = default;

bool NotSearchTemplate::ApplyImpl(
    ScTemplateParams const & params,
    TemplateArguments const & arguments,
    TemplateResults & results,
    std::list<FilterCallback> const & callbacks) const
{
  if (SearchTemplate::ApplyImpl(params, arguments, results, callbacks))
  {
    m_logger.Debug("Searching by not search template ", *this, " failed");
    return false;
  }
  m_logger.Debug("Searching by not search template ", *this, " succeeded");
  return true;
}
