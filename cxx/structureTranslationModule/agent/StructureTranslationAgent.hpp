#pragma once

#include "sc-memory/kpm/sc_agent.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "StructureTranslationAgent.generated.hpp"

#include "translator/StructureTranslatorSet.hpp"

namespace structureTranslationModule
{
class StructureTranslationAgent : public ScAgent
{
public:
  SC_CLASS(Agent, Event(scAgentsCommon::CoreKeynodes::question_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

private:
  bool checkActionClass(ScAddr const & actionNode);
  std::string translateStructure(ScAddr const & structAddr, ScMemoryContext * context);
  static std::string join(
      std::vector<std::string>::const_iterator const & cbegin,
      std::vector<std::string>::const_iterator const & cend,
      std::string const & delimiter = "");
};

}  // namespace structureTranslationModule
