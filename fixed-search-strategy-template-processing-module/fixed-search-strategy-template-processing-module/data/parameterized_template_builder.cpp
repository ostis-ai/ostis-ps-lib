#include "parameterized_template_builder.hpp"

#include <sc-memory/sc_agent_context.hpp>

#include "keynodes/keynodes.hpp"

#include "search_template.hpp"
#include "not_search_template.hpp"
#include "wait_template.hpp"
#include "generate_template.hpp"
#include "fixed_strategy_search_template.hpp"

std::unique_ptr<ParameterizedTemplate> ParameterizedTemplateBuilder::BuildTemplate(
    ScAgentContext & context,
    utils::ScLogger & logger,
    ScAddr const & templateAddr)
{
  if (!templateAddr.IsValid())
  {
    logger.Error("Template not found in parameterized template ", templateAddr);
    SC_THROW_EXCEPTION(
        utils::ExceptionItemNotFound, "Action template not found in parameterized template " << templateAddr);
  }

  ScTemplate templ;
  templ.Triple(Keynodes::concept_template_type, ScType::VarPermPosArc, ScType::VarNode >> "_template_type");
  templ.Triple("_template_type", ScType::VarPosArc, templateAddr);

  ScAddr templateTypeAddr;
  ScTemplateSearchResult result;
  if (context.SearchByTemplate(templ, result))
    templateTypeAddr = result[0]["_template_type"];

  if (templateTypeAddr == Keynodes::nrel_search_template || templateTypeAddr == Keynodes::nrel_search_set_template)
    return std::unique_ptr<SearchTemplate>(new SearchTemplate(context, logger, templateAddr));
  else if (templateTypeAddr == Keynodes::nrel_not_search_template)
    return std::unique_ptr<NotSearchTemplate>(new NotSearchTemplate(context, logger, templateAddr));
  else if (templateTypeAddr == Keynodes::nrel_wait_template)
    return std::unique_ptr<WaitTemplate>(new WaitTemplate(context, logger, templateAddr));
  else if (templateTypeAddr == Keynodes::nrel_generate_template)
    return std::unique_ptr<GenerateTemplate>(new GenerateTemplate(context, logger, templateAddr));
  else if (templateTypeAddr == Keynodes::nrel_fixed_search_strategy_template)
    return std::unique_ptr<FixedStrategySearchTemplate>(new FixedStrategySearchTemplate(context, logger, templateAddr));
  else
  {
    logger.Error("Unknown action template type ", templateTypeAddr);
    SC_THROW_EXCEPTION(utils::ExceptionInvalidType, "Unknown action template type " << templateTypeAddr);
  }
}
