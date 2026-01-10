#include "search_template.hpp"

#include <sc-memory/sc_agent_context.hpp>

SearchTemplate::SearchTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr)
  : ParameterizedTemplate(context, logger, templateAddr)
{
}

SearchTemplate::~SearchTemplate() = default;

bool SearchTemplate::ApplyImpl(
    ScTemplateParams const & params,
    TemplateArguments const & arguments,
    TemplateResults & results,
    std::list<FilterCallback> const & callbacks) const
{
  if (results.IsValid())
  {
    results.m_templateAddr = m_templateAddr;
    results.m_sortParamAddr = m_sortParamAddr;
    results.m_eraseParamsAddr = m_eraseParamsAddr;
    results.m_resultParamsAddr = m_resultParamsAddr;
  }
  else
    results = TemplateResults{
        m_replyContext, m_logger, m_templateAddr, m_sortParamAddr, m_eraseParamsAddr, m_resultParamsAddr};

  ScTemplateSearchResult searchResult;
  if (TrySearchByTemplate(params, searchResult))
  {
    m_logger.Debug("Searching by search template ", *this, " succeeded");
    return results.CollectFromSearchResult(searchResult, callbacks);
  }
  m_logger.Debug("Searching by search template ", *this, " failed");
  return false;
}

bool SearchTemplate::TrySearchByTemplate(ScTemplateParams const & params, ScTemplateSearchResult & searchResult) const
{
  m_logger.Debug("Build template ", *this);
  ScTemplate searchTemplate;
  m_replyContext.BuildTemplate(searchTemplate, m_templateAddr, params);

  if (searchTemplate.Size() == (params.GetAll().size() / 2))
  {
    m_logger.Debug("All variables ", *this, " replaced in template for search");
    return true;
  }

  m_logger.Debug("Search by template ", *this);
  bool const found = m_replyContext.SearchByTemplate(searchTemplate, searchResult);
  if (!found)
  {
    m_logger.Debug("Search by template ", *this, " failed");
    return false;
  }
  m_logger.Debug("Search by template ", *this, " succeeded");

  return true;
}
