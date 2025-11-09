#pragma once

#include "parameterized_template.hpp"

class ScAgentContext;

class SearchTemplate : public ParameterizedTemplate
{
  friend class ParameterizedTemplateBuilder;

public:
  /*!
   * @brief Destructor for SearchTemplate.
   */
  ~SearchTemplate() override;

protected:
  bool ApplyImpl(
      ScTemplateParams const & params,
      TemplateArguments const & arguments,
      TemplateResults & results,
      std::list<FilterCallback> const & callbacks) const override;

  SearchTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr);

  bool TrySearchByTemplate(ScTemplateParams const & params, ScTemplateSearchResult & searchResult) const;
};
