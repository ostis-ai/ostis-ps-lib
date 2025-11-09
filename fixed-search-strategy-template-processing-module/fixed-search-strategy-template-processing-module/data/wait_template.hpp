#pragma once

#include <chrono>

#include "search_template.hpp"

class ScAgentContext;

class WaitTemplate : public SearchTemplate
{
  friend class ParameterizedTemplateBuilder;

public:
  /*!
   * @brief Destructor for WaitTemplate.
   */
  ~WaitTemplate() override;

protected:
  static constexpr std::chrono::milliseconds DEFAULT_WAIT_TEMPLATE_INTERVAL_MS{200};

  bool ApplyImpl(
      ScTemplateParams const & params,
      TemplateArguments const & arguments,
      TemplateResults & results,
      std::list<FilterCallback> const & callbacks) const override;

  WaitTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr);
};
