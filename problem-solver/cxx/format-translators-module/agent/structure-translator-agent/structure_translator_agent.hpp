#pragma once

#include <sc-memory/sc_agent.hpp>

namespace formatTranslators
{
class StructureTranslatorAgent : public ScAgent<ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>>
{
public:
  ScAddr GetActionClass() const noexcept(false) override;

  bool CheckInitiationCondition(ScActionInitiatedEvent const & event) override;

  ScResult DoProgram(ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const & event, ScAction & action)
      override;

  ScAddr GetEventSubscriptionElement() const noexcept(false) override;

private:
  ScAddr structureToTranslate;
};

}  // namespace formatTranslators
