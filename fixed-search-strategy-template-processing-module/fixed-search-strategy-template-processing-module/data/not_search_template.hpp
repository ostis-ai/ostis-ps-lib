#pragma once

#include "search_template.hpp"

class ScAgentContext;

class NotSearchTemplate : public SearchTemplate
{
  friend class ParameterizedTemplateBuilder;

public:
  /*!
   * @brief Destructor for NotSearchTemplate.
   */
  ~NotSearchTemplate() override;

protected:
  bool ApplyImpl(
      ScTemplateParams const & params,
      TemplateArguments const & arguments,
      TemplateResults & results,
      std::list<FilterCallback> const & callbacks) const override;

  NotSearchTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr);
};
