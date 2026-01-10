#include "wait_template.hpp"

#include <chrono>
#include <thread>

#include <sc-memory/sc_agent_context.hpp>

WaitTemplate::WaitTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr)
  : SearchTemplate(context, logger, templateAddr)
{
}

WaitTemplate::~WaitTemplate() = default;

bool WaitTemplate::ApplyImpl(
    ScTemplateParams const & params,
    TemplateArguments const & arguments,
    TemplateResults & results,
    std::list<FilterCallback> const & callbacks) const
{
  size_t waitTimeMs = 0;
  if (m_replyContext.GetLinkContent(m_waitTimeMsAddr, waitTimeMs))
    m_logger.Debug("Wait time is ", waitTimeMs, " milliseconds");
  else
  {
    waitTimeMs = DEFAULT_WAIT_TEMPLATE_INTERVAL_MS.count();
    m_logger.Debug("Wait time is not specified, using default value of ", waitTimeMs, " milliseconds");
  }

  auto start = std::chrono::steady_clock::now();
  std::chrono::milliseconds timeout{waitTimeMs};

  while (std::chrono::steady_clock::now() - start < timeout)
  {
    if (SearchTemplate::ApplyImpl(params, arguments, results, callbacks))
    {
      m_logger.Debug("Searching by wait template ", *this, " succeeded");
      return true;
    }

    std::this_thread::sleep_for(DEFAULT_WAIT_TEMPLATE_INTERVAL_MS);
  }

  m_logger.Debug("Searching by wait template ", *this, " timed out");
  return false;
}
