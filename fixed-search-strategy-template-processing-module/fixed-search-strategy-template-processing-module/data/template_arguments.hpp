#pragma once

#include <optional>

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/utils/sc_logger.hpp>
#include <sc-memory/sc_set.hpp>

class TemplateResult;
class ScAgentContext;

using ArgumentType = ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>>;

class TemplateArguments
{
public:
  using Arguments = std::vector<ArgumentType>;

  TemplateArguments();

  TemplateArguments(ScAgentContext & context, utils::ScLogger & logger);

  std::optional<ScAddr> Get(ScAddr const & setAddr) const;

  void CollectFromSet(ScSet const & set);

  void Add(TemplateArguments const & arguments);

  void Add(ScAddr const & setAddr, ScAddr const & arcAddr, ScAddr const & elementAddr);

  void Add(TemplateResult const & result);

  bool GetTemplateParams(ScAddr const & templateAddr, ScAddr const & inputParamsAddr, ScTemplateParams & params) const;

private:
  ScAgentContext * m_context = nullptr;  ///< Pointer to the message reply context for knowledge base access
  utils::ScLogger * m_logger = nullptr;  ///< Pointer to the system logger for debugging and error reporting
  ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>>
      m_arguments;  ///< Collection of argument mappings from entity classes to arc-element pairs
};

using FilterCallback = std::function<bool(TemplateArguments)>;
