/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

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
  SC_PROPERTY(Keynode("action_translate_structures_into_natural_language"), ForceCreate(ScType::NodeConstClass))
  static ScAddr action_translate_structures_into_natural_language;

  SC_PROPERTY(Keynode("lang_en"), ForceCreate(ScType::NodeConstClass))
  static ScAddr lang_en;

  SC_PROPERTY(Keynode("translation_ignored_keynodes"), ForceCreate(ScType::NodeConstClass))
  static ScAddr translation_ignored_keynodes;
};

}  // namespace structureTranslationModule
