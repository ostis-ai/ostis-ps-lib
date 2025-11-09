#pragma once

#include "parameterized_template.hpp"

class ScAgentContext;

class FixedStrategySearchTemplate : public ParameterizedTemplate
{
  friend class ParameterizedTemplateBuilder;

public:
  ~FixedStrategySearchTemplate();

protected:
  ScAddr m_initTemplateAddr;
  ScAddr m_nextTemplateAddr;

  bool ApplyImpl(
      ScTemplateParams const & params,
      TemplateArguments const & arguments,
      TemplateResults & results,
      std::list<FilterCallback> const & callbacks) const override;

  bool ProcessNextTemplates(
      TemplateArguments const & arguments,
      TemplateResults & startTemplateResults,
      TemplateResults & results) const;

  FixedStrategySearchTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr);

  void Load();
};
