#pragma once

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_object.hpp"

#include "TranslationKeynodes.generated.hpp"

namespace structureTranslationModule
{

class TranslationKeynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("action_translate_structures"), ForceCreate(ScType::NodeConstClass))
  static ScAddr action_translate_structures;

  SC_PROPERTY(Keynode("lang_en"), ForceCreate(ScType::NodeConstClass))
  static ScAddr lang_en;
};

}  // namespace structureTranslationModule
