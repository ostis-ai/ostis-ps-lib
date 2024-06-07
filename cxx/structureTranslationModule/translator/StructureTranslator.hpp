#pragma once

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_addr.hpp"

namespace structureTranslationModule
{
using ScAddrSet = std::unordered_set<ScAddr, ScAddrHashFunc<uint32_t>>;

class StructureTranslator
{
public:
  explicit StructureTranslator(ScMemoryContext * context)
    : context(context)
  {
  }

  virtual std::vector<std::string> translate(ScAddr const & structAddr)
      const = 0;

  size_t hashCode() const;

  bool operator<(StructureTranslator const & other) const;

protected:
  ScMemoryContext * context;

  std::string getEnglishContent(ScAddr const & linkNode) const;

  std::string getEnglishMainIdtf(ScAddr const & node) const;

  bool isInIgnoredKeynodes(ScAddr const & node) const;

  static bool isInStructure(ScAddr const & elementAddr, ScAddrSet const & structure);

  static bool anyIsInStructure(ScAddrVector const & elements, ScAddrSet const & structure);

private:
  ScIterator5Ptr getNrelMainIdtfIterator(ScAddr const & node) const;

  bool isEnglish(ScAddr const & node) const;
};

struct StructureTranslatorCmp
{
  bool operator()(StructureTranslator * first, StructureTranslator * second) const
  {
    return *first < *second;
  }
};

}  // namespace structureTranslationModule
