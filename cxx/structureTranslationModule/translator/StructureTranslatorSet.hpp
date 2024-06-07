#pragma once

#include "translator/StructureTranslator.hpp"

namespace structureTranslationModule
{
class StructureTranslatorSet
{
public:
  explicit StructureTranslatorSet(ScMemoryContext * context);
  
  std::vector<std::string> translate(ScAddr structAddr)
      const;

  ~StructureTranslatorSet();

private:
  std::set<StructureTranslator *> const handlers;
};

}  // namespace structureTranslationModule
