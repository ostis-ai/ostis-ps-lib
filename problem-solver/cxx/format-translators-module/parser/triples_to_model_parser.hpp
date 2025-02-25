#pragma once

#include <sc-memory/sc_memory.hpp>

#include "model/element.hpp"

#include "parser/types.hpp"

namespace formatTranslators
{
class TriplesToModelParser
{
public:
  std::shared_ptr<Node> ParseTriples(
      ScMemoryContext * memoryContext,
      ScAddr const & root,
      ScAddrToValueUnorderedMap<size_t> const & keyElementsOrderMap,
      ScAddr const & identifiersLanguageAddr,
      Triples const & structureTriplesMap);

private:
  ScMemoryContext * context = nullptr;
  ScAddrToValueUnorderedMap<size_t> keyElementsOrder;
  Triples structureTriples;
  ScAddr identifiersLanguage;

  ScAddrUnorderedSet rootElements;
  ScAddrUnorderedSet walkedConnectors;

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

  void ProcessTriple(std::shared_ptr<Node> const & rootElement, std::shared_ptr<Connector> const & connectorElement);

  void SetIdentifierIfExists(ScAddr const & elementAddr, std::shared_ptr<Element> const & element) const;
};

}  // namespace formatTranslators
