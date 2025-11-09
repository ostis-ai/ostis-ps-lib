#pragma once

#include "parameterized_template.hpp"

class ScAgentContext;

class GenerateTemplate : public ParameterizedTemplate
{
  friend class ParameterizedTemplateBuilder;

public:
  /*!
   * @brief Destructor for GenerateTemplate.
   */
  ~GenerateTemplate() override;

protected:
  bool ApplyImpl(
      ScTemplateParams const & params,
      TemplateArguments const & arguments,
      TemplateResults & results,
      std::list<FilterCallback> const & callbacks) const override;

  GenerateTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr);

  bool TryGenerateByTemplate(ScTemplateParams const & params, ScTemplateGenResult & genResult) const;
};
