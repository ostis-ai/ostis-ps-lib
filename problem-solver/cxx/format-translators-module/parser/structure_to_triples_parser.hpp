#pragma once

#include "parser/types.hpp"

#include <sc-memory/sc_memory.hpp>

namespace formatTranslators
{
class StructureToTriplesParser
{
public:
  void ParseStructure(
      ScMemoryContext * memoryContext,
      ScAddr const & structureToTranslateAddr,
      ScAddrToValueUnorderedMap<size_t> const & keyElementsOrderMap,
      ScAddr const & identifiersLanguageAddr,
      Triples & structureTriplesMap);

private:
  ScMemoryContext * context = nullptr;
  ScAddr structureToTranslate;
  ScAddr identifiersLanguage;
  ScAddrToValueUnorderedMap<size_t> keyElementsOrder;

  void ParseStructure();

  Triples structureTriples;

  void ParseTriple(
      ScAddr const & baseAddr,
      ScAddr const & connectorAddr,
      ScAddr const & otherElementAddr,
      bool const isReversed,
      ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> & triplesWithConnectorNotInParsedStructure,
      ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> & triplesWithConnectorInParsedStructure);

  void AddTripleToParsedTriplesIfOtherIsNode(
      ScAddr const & baseAddr,
      ScAddr const & connectorAddr,
      ScAddr const & otherElementAddr,
      bool const isReversed);
};

}  // namespace formatTranslators
