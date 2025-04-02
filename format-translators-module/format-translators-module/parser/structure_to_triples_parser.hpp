#pragma once

#include <iosfwd>
#include <list>
#include <unordered_map>

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
      ScAddrToValueUnorderedMap<uint32_t> const & keyElementsOrderMap,
      ScAddr const & identifiersLanguageAddr,
      std::shared_ptr<Triples> & structureTriplesMap);

private:
  ScMemoryContext * context = nullptr;
  ScAddr structureToTranslate;
  ScAddr identifiersLanguage;
  ScAddrToValueUnorderedMap<uint32_t> keyElementsOrder;

  void OrderTriples(UnorderedTriples const & unorderedTriples);

  void ParseStructure();

  std::shared_ptr<Triples> structureTriples;

  void ParseTriple(
      ScAddr const & baseAddr,
      ScAddr const & connectorAddr,
      ScAddr const & otherElementAddr,
      bool const isReversed,
      ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> & triplesWithConnectorNotInParsedStructure,
      ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> & triplesWithConnectorInParsedStructure);

  void AddTripleToParsedTriples(
      ScAddr const & baseAddr,
      ScAddr const & connectorAddr,
      ScAddr const & otherElementAddr,
      bool const isReversed);
};

}  // namespace formatTranslators
