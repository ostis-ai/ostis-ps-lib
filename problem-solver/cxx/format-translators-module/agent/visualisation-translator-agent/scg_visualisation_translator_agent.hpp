#pragma once

#include <map>

#include <sc-memory/sc_agent.hpp>

#include "model/element.hpp"

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
  static std::string const GWF_CONTENT_BEFORE_STATIC_SECTOR_CONTENT;
  static std::string const GWF_CONTENT_AFTER_STATIC_SECTOR_CONTENT;
  float lastAssignedY = 0;
  // CONNECTOR_IDENTIFIER_SCALE was set after a couple of experiments because it makes connector long enough
  // for identifier to not overlap with other element
  static float const CONNECTOR_IDENTIFIER_SCALE;
  static float const CONNECTOR_INCIDENT_POINT_PERCENT;
  static float const Y_INCREMENT;
  static float const MIN_X;
  static float const MAX_Y;
  static float const MAX_LINK_WIDTH;
  ScAddr structureToTranslate;
  ScAddr identifiersLanguage;
  ScAddrToValueUnorderedMap<size_t> keyElementsOrder;

  class ScAddrComparator
  {
  public:
    explicit ScAddrComparator(
        ScAddrToValueUnorderedMap<size_t> & order,
        ScAddrToValueUnorderedMap<
            std::map<std::pair<ScAddr, ScAddr>, std::list<std::tuple<ScAddr, ScAddr, bool>>, ScAddrComparator>> &
            structureTriples);
    bool operator()(std::pair<ScAddr, ScAddr> const & first, std::pair<ScAddr, ScAddr> const & second) const;

  private:
    ScAddrToValueUnorderedMap<size_t> & order;
    ScAddrToValueUnorderedMap<
        std::map<std::pair<ScAddr, ScAddr>, std::list<std::tuple<ScAddr, ScAddr, bool>>, ScAddrComparator>> &
        structureTriples;
    size_t GetValue(std::pair<ScAddr, ScAddr> const & key) const;
  };

  ScAddrToValueUnorderedMap<
      std::map<std::pair<ScAddr, ScAddr>, std::list<std::tuple<ScAddr, ScAddr, bool>>, ScAddrComparator>>
      structureTriples;
  ScAddrUnorderedSet rootElements;
  ScAddrUnorderedSet walkedConnectors;
  std::vector<std::list<std::shared_ptr<Node>>> parsedStructure;
  void ParseKeyElementsOrder(ScAddr const & keyElementsOrderTuple);

  void ParseStructureIntoTriples();

  void AddTripleToParsedTriples(
      ScAddr const & baseAddr,
      ScAddr const & connectorAddr,
      ScAddr const & otherElementAddr,
      bool const isReversed);

  std::shared_ptr<Node> WalkBFS(ScAddr const & root, size_t currentLevel);

  std::shared_ptr<Node> CreateNode(ScAddr const & nodeAddr, ScType const & nodeType) const;

  std::shared_ptr<Connector> CreateConnector(
      ScAddr const & connector,
      std::shared_ptr<Element> const & baseElement,
      std::shared_ptr<Node> const & otherElement,
      bool const isReversed) const;

  void AddVerticalConnector(
      ScAddr const & otherAddr,
      ScAddr const & connectorAddr,
      std::shared_ptr<Connector> const & baseConnector,
      bool const isReversed) const;

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

  void SetSystemIdentifierIfExists(ScAddr const & elementAddr, std::shared_ptr<Element> const & element) const;

  static std::list<std::shared_ptr<Node>> AssignXCoordinates(std::shared_ptr<Node> const & rootElement);

  void AssignYCoordinates(std::shared_ptr<Node> const & rootElement);

  void ProcessTriple(std::shared_ptr<Node> const & rootElement, std::shared_ptr<Connector> const & connectorElement);
};

}  // namespace formatTranslators
