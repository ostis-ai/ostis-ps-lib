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
  ScAddr GetActionClass() const noexcept(false) override;

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
    size_t getValue(std::pair<ScAddr, ScAddr> const & key) const;
  };

  ScAddrToValueUnorderedMap<
      std::map<std::pair<ScAddr, ScAddr>, std::list<std::tuple<ScAddr, ScAddr, bool>>, ScAddrComparator>>
      structureTriples;
  ScAddrUnorderedSet rootElements;
  ScAddrUnorderedSet walkedConnectors;
  std::vector<std::list<std::shared_ptr<Node>>> parsedStructure;
  void parseKeyElementsOrder(ScAddr const & keyElementsOrderTuple);

  void parseStructureIntoTriples();

  void addTripleToParsedTriples(
      ScAddr const & baseAddr,
      ScAddr const & connectorAddr,
      ScAddr const & otherElementAddr,
      bool const isReversed);

  std::shared_ptr<Node> walkBFS(ScAddr const & root, size_t currentLevel);

  std::shared_ptr<Node> createNode(ScAddr const & nodeAddr, ScType const & nodeType) const;

  std::shared_ptr<Connector> createConnector(
      ScAddr const & connector,
      std::shared_ptr<Element> const & baseElement,
      std::shared_ptr<Node> const & otherElement,
      bool const isReversed) const;

  void addVerticalConnector(
      ScAddr const & otherAddr,
      ScAddr const & connectorAddr,
      std::shared_ptr<Connector> const & baseConnector,
      bool const isReversed) const;

  static float calculateIndentCausedByLinkContent(
      std::list<std::shared_ptr<Connector>> const & connectorsToNextLevelRoots,
      float & previousLevelMaxLinkContentSize);

  static void calculateNextLevelElementsAndIdentifiersLengths(
      std::list<std::shared_ptr<Node>> const & currentLevelRoots,
      std::list<std::shared_ptr<Connector>> & connectorsToNextLevelRoots,
      std::list<std::shared_ptr<Node>> & nextLevelRoots,
      float & maxRelationIdentifierLength,
      float & maxLevelElementIdentifierLength);

  static void updateNextLevelRoots(
      std::list<std::shared_ptr<Node>> & treeRoots,
      std::list<std::shared_ptr<Connector>> & connectorsToNextLevelRoots,
      std::list<std::shared_ptr<Node>> & nextLevelRoots);

  static void setRelationIndent(std::list<std::shared_ptr<Node>> const & currentLevelRoots, float relationIndent);

  void setSystemIdentifierIfExists(ScAddr const & elementAddr, std::shared_ptr<Element> const & element) const;

  static std::list<std::shared_ptr<Node>> assignXCoordinates(std::shared_ptr<Node> const & rootElement);

  void assignYCoordinates(std::shared_ptr<Node> const & rootElement);

  void processTriple(std::shared_ptr<Node> const & rootElement, std::shared_ptr<Connector> const & connectorElement);
};

}  // namespace formatTranslators
