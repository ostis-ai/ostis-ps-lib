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
  ScAddr structureToTranslate;
  ScAddr identifiersLanguage;
  ScAddrToValueUnorderedMap<size_t> keyElementsOrder;

  Triples structureTriples;
  ScAddrUnorderedSet rootElements;
  ScAddrUnorderedSet walkedConnectors;
  void ParseKeyElementsOrder(ScAddr const & keyElementsOrderTuple);

  static float CalculateIndentCausedByLinkContent(
      std::list<std::shared_ptr<Connector>> const & connectorsToNextLevelRoots,
      float & previousLevelMaxLinkContentSize);

  static void CalculateNextLevelElementsAndIdentifiersLengths(
      std::list<std::shared_ptr<Node>> const & currentLevelRoots,
      std::list<std::shared_ptr<Connector>> & connectorsToNextLevelRoots,
      std::list<std::shared_ptr<Node>> & nextLevelRoots,
      float & maxRelationIdentifierLength,
      float & maxLevelElementIdentifierLength);

  static void UpdateNextLevelRoots(
      std::list<std::shared_ptr<Node>> & treeRoots,
      std::list<std::shared_ptr<Connector>> & connectorsToNextLevelRoots,
      std::list<std::shared_ptr<Node>> & nextLevelRoots);

  static void SetRelationIndent(std::list<std::shared_ptr<Node>> const & currentLevelRoots, float relationIndent);

  static std::list<std::shared_ptr<Node>> AssignXCoordinates(std::shared_ptr<Node> const & rootElement);

  void AssignYCoordinates(std::shared_ptr<Node> const & rootElement);
};

}  // namespace formatTranslators
