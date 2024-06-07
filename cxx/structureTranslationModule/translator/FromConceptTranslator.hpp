#pragma once

#include "StructureTranslator.hpp"

namespace structureTranslationModule
{
class FromConceptTranslator : public StructureTranslator
{
public:
  explicit FromConceptTranslator(ScMemoryContext * context);

  std::vector<std::string> translate(ScAddr const & structAddr)
      const override;
};

}  // namespace structureTranslationModule
