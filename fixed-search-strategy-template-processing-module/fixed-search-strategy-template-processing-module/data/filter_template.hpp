#pragma once

#include "search_template.hpp"

class ScAgentContext;

class FilterTemplate : public SearchTemplate
{
  friend class ParameterizedTemplate;

public:
  /*!
   * @brief Destructor for FilterTemplate.
   */
  ~FilterTemplate() override;

protected:
  bool ApplyImpl(
      ScTemplateParams const & params,
      TemplateArguments const & arguments,
      TemplateResults & results,
      std::list<FilterCallback> const & callbacks) const override;

  FilterTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr);
};
