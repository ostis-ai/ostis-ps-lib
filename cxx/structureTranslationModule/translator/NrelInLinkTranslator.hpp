#pragma once

#include "StructureTranslator.hpp"

namespace structureTranslationModule
{
class NrelInLinkTranslator : public StructureTranslator
{
public:
  explicit NrelInLinkTranslator(ScMemoryContext * context);

std::vector<std::string> translate(ScAddr const & structAddr)
      const override;

private:
};

}  // namespace structureTranslationModule
