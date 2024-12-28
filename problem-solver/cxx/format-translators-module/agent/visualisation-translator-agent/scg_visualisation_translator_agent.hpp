#pragma once

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
  static float const Y_INCREMENT;
  ScAddr structureToTranslate;
  ScAddr identifiersLanguage;
  ScAddrList keyElementsOrder;
  ScAddrUnorderedSet rootElements;
  ScAddrUnorderedSet walkedConnectors;
  std::vector<std::list<std::shared_ptr<Node>>> parsedStructure;
  void parseKeyElementsOrder(ScAddr const & keyElementsOrderTuple);

  std::shared_ptr<Node> walkBFS(ScAddr const & root, size_t currentLevel);

  std::shared_ptr<Node> createNode(ScAddr const & nodeAddr, ScType const & nodeType) const;

  std::shared_ptr<Connector> createConnector(
      ScAddr const & connector,
      std::shared_ptr<Element> const & baseElement,
      std::shared_ptr<Node> const & otherElement,
      bool const isReversed) const;

  void addIncomingVerticalConnectors(std::shared_ptr<Connector> const & connectorElement) const;

  void addOutgoingVerticalConnectors(std::shared_ptr<Connector> const & connectorElement) const;

  void addVerticalConnector(
      ScAddr const & otherAddr,
      ScAddr const & connectorAddr,
      std::shared_ptr<Connector> const & baseConnector,
      bool const isReversed) const;

  void setSystemIdentifierIfExists(ScAddr const & elementAddr, std::shared_ptr<Element> const & element) const;

  void assignXCoordinates() const;

  float increaseSubtreeYCoordinate(std::shared_ptr<Node> const & treeRoot, float const increment) const;

  void processTriple(
      std::shared_ptr<Node> & rootElement,
      std::shared_ptr<Connector> const & connectorElement,
      std::shared_ptr<Node> const & subtreeRoot);

  template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
  void processIncomingIterator(
      std::shared_ptr<Node> & rootElement,
      std::shared_ptr<ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>> const & incomingIterator,
      size_t const currentLevel,
      std::list<size_t> const & indicesForStructureCheck);

  template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
  void processOutgoingIterator(
      std::shared_ptr<Node> & rootElement,
      std::shared_ptr<ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>> const & outgoingIterator,
      size_t const currentLevel,
      std::list<size_t> const & indicesForStructureCheck);

  template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
  void processIterator(
      std::shared_ptr<Node> & rootElement,
      std::shared_ptr<ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>> const & iterator,
      size_t const currentLevel,
      std::list<size_t> const & indicesForStructureCheck,
      bool const isOutgoing);
};

}  // namespace formatTranslators
