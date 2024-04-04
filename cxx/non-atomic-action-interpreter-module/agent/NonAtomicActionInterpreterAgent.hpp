#pragma once

#include "sc-memory/kpm/sc_agent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "interpreter/NonAtomicActionInterpreter.hpp"

#include "NonAtomicActionInterpreterAgent.generated.hpp"

namespace nonAtomicActionInterpreterModule
{
class NonAtomicActionInterpreterAgent : public ScAgent
{
  SC_CLASS(Agent, Event(scAgentsCommon::CoreKeynodes::question_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

private:
  std::unique_ptr<NonAtomicActionInterpreter> nonAtomicActionInterpreter;

  bool checkActionClass(ScAddr const & actionAddr);

  void generateNonAtomicActionTemplate(
      ScAddr const & nonAtomicActionTemplateAddr,
      ScTemplateParams const & templateParams);

  std::map<ScAddr, ScAddr, ScAddrLessFunc> createReplacements(
      ScAddr const & nonAtomicActionTemplate,
      ScAddr const & argumentsSet);

  void validateVariableIdentifiers(std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements);

  ScAddr getTemplateKeyElement(ScAddr const & templateAddr);

  ScAddr replaceNonAtomicAction(ScAddr const & templateAddr, ScTemplateParams & templateParams);

  void initFields();
};

}  // namespace nonAtomicActionInterpreterModule
