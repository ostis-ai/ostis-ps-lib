#include "parameterized_template.hpp"

#include <sc-memory/sc_agent_context.hpp>

#include "keynodes/keynodes.hpp"

#include "template_arguments.hpp"
#include "template_results.hpp"

#include "filter_template.hpp"
#include "not_filter_template.hpp"

ParameterizedTemplate::ParameterizedTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & setAddr)
  : ScSet(&context, setAddr)
  , m_replyContext(context)
  , m_logger(logger)
{
  Load();
}

ParameterizedTemplate::~ParameterizedTemplate() = default;

void ParameterizedTemplate::Load()
{
  ForEach(
      [&](ScAddr const &, ScAddr const & elementAddr, ScAddr const &, ScAddr const & roleAddr)
      {
        if (roleAddr == Keynodes::rrel_template)
        {
          m_templateAddr = elementAddr;
          m_logger.Debug("Template ", m_templateAddr, " found in parameterized template ", *this);
        }
        else if (roleAddr == Keynodes::rrel_wait_time)
        {
          m_waitTimeMsAddr = elementAddr;
          m_logger.Debug("Wait time ", m_waitTimeMsAddr, " found in parameterized template ", *this);
        }
        else if (roleAddr == Keynodes::rrel_template_sort_param)
        {
          m_sortParamAddr = elementAddr;
          m_logger.Debug("Sort param ", m_sortParamAddr, " found in parameterized template ", *this);
        }
        else if (roleAddr == Keynodes::rrel_template_input_params)
        {
          m_inputParamsAddr = elementAddr;
          m_logger.Debug("Input params ", m_inputParamsAddr, " found in parameterized template ", *this);
        }
        else if (roleAddr == Keynodes::rrel_template_erase_params)
        {
          m_eraseParamsAddr = elementAddr;
          m_logger.Debug("Erase params ", m_eraseParamsAddr, " found in parameterized template ", *this);
        }
        else if (roleAddr == Keynodes::rrel_template_output_params)
        {
          m_resultParamsAddr = elementAddr;
          m_logger.Debug("Output params ", m_resultParamsAddr, " found in parameterized template ", *this);
        }
        else if (roleAddr == Keynodes::rrel_filter_templates)
        {
          m_replyContext.ConvertToSet(elementAddr).GetElements(m_filterTemplateAddrs);
          m_logger.Debug("Filter templates ", elementAddr, " found in parameterized template ", *this);
        }
        else if (roleAddr == Keynodes::rrel_not_filter_templates)
        {
          m_replyContext.ConvertToSet(elementAddr).GetElements(m_notFilterTemplateAddrs);
          m_logger.Debug("Not filter templates ", elementAddr, " found in parameterized template ", *this);
        }
      });

  if (!m_templateAddr.IsValid())
    m_logger.Debug("Template not found in parameterized template ", *this);

  if (!m_waitTimeMsAddr.IsValid())
    m_logger.Debug("Wait time not found in parameterized template ", *this);

  if (!m_sortParamAddr.IsValid())
    m_logger.Debug("Sort param not found in parameterized template ", *this);

  if (!m_inputParamsAddr.IsValid())
    m_logger.Debug("Input params not found in parameterized template ", *this);

  if (!m_eraseParamsAddr.IsValid())
    m_logger.Debug("Erase params not found in parameterized template ", *this);

  if (!m_resultParamsAddr.IsValid())
    m_logger.Debug("Output params not found in parameterized template ", *this);
}

bool ParameterizedTemplate::Apply(TemplateArguments const & arguments, TemplateResults & results) const
{
  std::list<FilterCallback> filterCallbacks;
  if (!m_filterTemplateAddrs.empty())
  {
    for (ScAddr const & filterTemplateAddr : m_filterTemplateAddrs)
    {
      auto const FilterCallback = [this, &filterTemplateAddr, &arguments](TemplateArguments templateArguments) -> bool
      {
        templateArguments.Add(arguments);
        return Filter(filterTemplateAddr, templateArguments);
      };
      filterCallbacks.push_back(FilterCallback);
    }
  }

  if (!m_notFilterTemplateAddrs.empty())
  {
    for (ScAddr const & notFilterTemplateAddr : m_notFilterTemplateAddrs)
    {
      auto NotFilterCallback = [this, &notFilterTemplateAddr, &arguments](TemplateArguments templateArguments) -> bool
      {
        templateArguments.Add(arguments);
        return NotFilter(notFilterTemplateAddr, templateArguments);
      };
      filterCallbacks.push_back(NotFilterCallback);
    }
  }

  ScTemplateParams params;
  if (!arguments.GetTemplateParams(m_templateAddr, m_inputParamsAddr, params))
    return false;
  bool const actionStatus = ApplyImpl(params, arguments, results, filterCallbacks);
  return actionStatus;
}

bool ParameterizedTemplate::Filter(ScAddr const & filterTemplateAddr, TemplateArguments const & arguments) const
{
  FilterTemplate const & filterTemplate = FilterTemplate(m_replyContext, m_logger, filterTemplateAddr);
  TemplateResults filterResults;
  bool const filterStatus = filterTemplate.ApplyImpl(ScTemplateParams(), arguments, filterResults, {});
  return filterStatus;
}

bool ParameterizedTemplate::NotFilter(ScAddr const & notFilterTemplateAddr, TemplateArguments const & arguments) const
{
  NotFilterTemplate const & filterTemplate = NotFilterTemplate(m_replyContext, m_logger, notFilterTemplateAddr);
  TemplateResults notFilterResults;
  bool const notFilterStatus = filterTemplate.ApplyImpl(ScTemplateParams(), arguments, notFilterResults, {});
  return notFilterStatus;
}
