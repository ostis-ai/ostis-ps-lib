#pragma once

#include "StructureTranslator.hpp"

namespace structureTranslationModule
{
class NrelFromNodeTranslator : public StructureTranslator
{
public:
  explicit NrelFromNodeTranslator(ScMemoryContext * context);

  std::vector<std::string> translate(ScAddr const & structAddr)
      const override;

private:
};

}  // namespace structureTranslationModule
