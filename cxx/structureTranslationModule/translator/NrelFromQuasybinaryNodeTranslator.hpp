#pragma once

#include "StructureTranslator.hpp"

namespace structureTranslationModule
{
class NrelFromQuasybinaryNodeTranslator : public StructureTranslator
{
public:
  explicit NrelFromQuasybinaryNodeTranslator(ScMemoryContext * context);

std::vector<std::string> translate(ScAddr const & structAddr)
      const override;

private:
};

}  // namespace structureTranslationModule
