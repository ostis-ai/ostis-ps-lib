#pragma once

#include <sc-memory/sc_agent.hpp>

class FixedSearchStrategyTemplateProcessingAgent : public ScActionInitiatedAgent
{
public:
  FixedSearchStrategyTemplateProcessingAgent();

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;
};
