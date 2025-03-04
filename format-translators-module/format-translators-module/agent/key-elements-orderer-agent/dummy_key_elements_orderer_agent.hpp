#pragma once

#include <sc-memory/sc_agent.hpp>

namespace formatTranslators
{
class DummyKeyElementsOrdererAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;

  bool CheckInitiationCondition(ScActionInitiatedEvent const & event) override;

  ScResult DoProgram(ScAction & action) override;

private:
  void AddNodesToOrder(
      ScAddr const & structure,
      ScAddr const & elementsOrder,
      ScAddrUnorderedSet & elementsInOrder,
      ScAddr & lastMembershipArc,
      ScType const & nodesType) const;
};

}  // namespace formatTranslators
