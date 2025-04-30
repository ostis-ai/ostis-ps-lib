#pragma once

#include <sc-memory/sc_agent.hpp>

#include "model/element.hpp"

#include "parser/types.hpp"

namespace formatTranslators
{
class Node;
class Connector;

class SCgVisualisationTranslatorAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;

  bool CheckInitiationCondition(ScActionInitiatedEvent const & event) override;

  ScResult DoProgram(ScAction & action) override;

private:
  float lastAssignedY = 0;
  ScAddrToValueUnorderedMap<uint32_t> keyElementsOrder;

  ScAddrUnorderedSet rootElements;
  ScAddrUnorderedSet walkedConnectors;
  void ParseKeyElementsOrder(ScAddr const & keyElementsOrderTuple);

  std::list<std::shared_ptr<Node>> AssignXCoordinates(std::shared_ptr<Node> const & rootElement);

  void AssignXCoordinates(
      std::list<std::shared_ptr<Node>> & roots,
      std::shared_ptr<Node> const & treeRoot,
      float rootElementIndent);

  void AssignYCoordinates(std::shared_ptr<Node> const & rootElement);
};

}  // namespace formatTranslators
