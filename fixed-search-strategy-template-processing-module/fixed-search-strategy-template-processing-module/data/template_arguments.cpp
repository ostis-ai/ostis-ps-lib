#include "template_arguments.hpp"

#include <sc-memory/sc_agent_context.hpp>

#include "template_results.hpp"

TemplateArguments::TemplateArguments()
  : m_context(nullptr)
  , m_logger(nullptr)
{
}

TemplateArguments::TemplateArguments(ScAgentContext & context, utils::ScLogger & logger)
  : m_context(&context)
  , m_logger(&logger)
{
}

std::optional<ScAddr> TemplateArguments::Get(ScAddr const & setAddr) const
{
  return m_arguments.find(setAddr) == m_arguments.cend() ? std::nullopt
                                                         : std::optional<ScAddr>(m_arguments.at(setAddr).second);
}

void TemplateArguments::CollectFromSet(ScSet const & set)
{
  ScAddrUnorderedSet arcs;
  set.GetElements(arcs);

  for (ScAddr const arcAddr : arcs)
  {
    auto const [setAddr, elementAddr] = m_context->GetConnectorIncidentElements(arcAddr);
    m_arguments.insert({setAddr, {arcAddr, elementAddr}});
  }
}

void TemplateArguments::Add(TemplateArguments const & arguments)
{
  m_arguments.insert(arguments.m_arguments.cbegin(), arguments.m_arguments.cend());
}

void TemplateArguments::Add(ScAddr const & setAddr, ScAddr const & arcAddr, ScAddr const & elementAddr)
{
  m_arguments.insert({setAddr, {arcAddr, elementAddr}});
}

void TemplateArguments::Add(TemplateResult const & result)
{
  m_arguments.insert(result.m_result.cbegin(), result.m_result.cend());
}

bool TemplateArguments::GetTemplateParams(
    ScAddr const & templateAddr,
    ScAddr const & inputParamsAddr,
    ScTemplateParams & params) const
{
  if (!inputParamsAddr.IsValid())
    return true;

  bool status = true;
  ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> notFoundParams;
  m_context->ConvertToSet(inputParamsAddr)
      .ForEach(
          [&](ScAddr const &, ScAddr const & paramArcAddr, ScAddr const &, ScAddr const &)
          {
            auto const [setAddr, paramElementAddr] = m_context->GetConnectorIncidentElements(paramArcAddr);
            notFoundParams.insert({setAddr, {paramArcAddr, paramElementAddr}});
          });

  for (auto const & [setAddr, arcAndElement] : m_arguments)
  {
    ScAddr const arcAddr = arcAndElement.first;
    ScAddr const elementAddr = arcAndElement.second;

    std::string const setElementSystemIdentifier = m_context->GetElementSystemIdentifier(setAddr);
    auto const it = notFoundParams.find(setAddr);
    if (it == notFoundParams.cend())
      m_logger->Debug(
          "Element ", elementAddr, " of set ", setElementSystemIdentifier, " is not expected to substitute. Skip");
    else
    {
      ScAddr const paramArcAddr = it->second.first;
      ScAddr const paramElementAddr = it->second.second;

      notFoundParams.erase(setAddr);
      m_logger->Debug("Element ", elementAddr, " of set ", setElementSystemIdentifier, " is expected to substitute");

      params.Add(paramArcAddr, arcAddr);
      params.Add(paramElementAddr, elementAddr);
      m_logger->Debug("Element ", elementAddr, " of set ", setElementSystemIdentifier, " added to params");
    }
  }

  for (auto const & [setAddr, arcAndElement] : notFoundParams)
    m_logger->Warning(
        "Element ",
        arcAndElement.second,
        " of set ",
        m_context->GetElementSystemIdentifier(setAddr),
        " not found in arguments");

  return notFoundParams.empty();
}
