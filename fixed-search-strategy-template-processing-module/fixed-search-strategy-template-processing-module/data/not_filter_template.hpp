#pragma once

#include "not_search_template.hpp"

class ScAgentContext;

class NotFilterTemplate : public NotSearchTemplate
{
  friend class ParameterizedTemplate;

public:
  ~NotFilterTemplate() override;

protected:
  bool ApplyImpl(
      ScTemplateParams const & params,
      TemplateArguments const & arguments,
      TemplateResults & results,
      std::list<FilterCallback> const & callbacks) const override;

  NotFilterTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr);
};
