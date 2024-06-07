#pragma once

#include "StructureTranslator.hpp"

namespace structureTranslationModule
{
class NrelInQuasybinaryLinkTranslator : public StructureTranslator
{
public:
  explicit NrelInQuasybinaryLinkTranslator(ScMemoryContext * context);

  std::vector<std::string> translate(ScAddr const & structAddr)
      const override;

private:
};

}  // namespace structureTranslationModule
