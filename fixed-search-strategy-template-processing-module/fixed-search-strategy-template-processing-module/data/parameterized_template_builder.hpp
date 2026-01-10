#pragma once

#include <sc-memory/utils/sc_logger.hpp>

#include "parameterized_template.hpp"

class ScAgentContext;

class ParameterizedTemplateBuilder
{
public:
  static std::unique_ptr<ParameterizedTemplate> BuildTemplate(
      ScAgentContext & context,
      utils::ScLogger & logger,
      ScAddr const & templateAddr);
};
