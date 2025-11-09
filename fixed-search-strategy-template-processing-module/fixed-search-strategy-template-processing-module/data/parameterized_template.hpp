#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_set.hpp>
#include <sc-memory/utils/sc_logger.hpp>

#include "template_arguments.hpp"
#include "template_results.hpp"

class ScAgentContext;
class FilterTemplate;
class NotFilterTemplate;

class ParameterizedTemplate : public ScSet
{
public:
  virtual ~ParameterizedTemplate();

  bool Apply(TemplateArguments const & arguments, TemplateResults & results) const;

protected:
  virtual bool ApplyImpl(
      ScTemplateParams const & params,
      TemplateArguments const & arguments,
      TemplateResults & results,
      std::list<FilterCallback> const & callbacks) const = 0;

protected:
  ScAgentContext & m_replyContext;  ///< Reference to message reply context for knowledge base access
  utils::ScLogger & m_logger;       ///< Reference to system logger for debugging and error reporting
  ScAddr m_templateAddr;            ///< Address of the primary action template node
  ScAddr m_templateTypeAddr;        ///< Address of the action template type identifier
  ScAddr m_waitTimeMsAddr;          ///< Address of the wait time configuration for time-based operations
  ScAddr m_sortParamAddr;           ///< Address of the sorting criteria entity class for result ordering
  ScAddr m_inputParamsAddr;         ///< Address of the input parameters set defining required entities
  ScAddr m_eraseParamsAddr;         ///< Address of parameters to erase after template processing
  ScAddr m_resultParamsAddr;        ///< Address of result parameters set for output configuration

  ScAddrUnorderedSet m_filterTemplateAddrs;     ///< Collection of filter template addresses for positive filtering
  ScAddrUnorderedSet m_notFilterTemplateAddrs;  ///< Collection of not-filter template addresses for negative filtering

  ParameterizedTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & setAddr);

  void Load();

  bool Filter(ScAddr const & filterTemplateAddr, TemplateArguments const & arguments) const;

  bool NotFilter(ScAddr const & notFilterTemplateAddr, TemplateArguments const & arguments) const;
};
