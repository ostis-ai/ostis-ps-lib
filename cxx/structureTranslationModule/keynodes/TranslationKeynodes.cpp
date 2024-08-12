/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_memory.hpp"

#include "TranslationKeynodes.hpp"

namespace structureTranslationModule
{
ScAddr TranslationKeynodes::action_translate_structures_into_natural_language;
ScAddr TranslationKeynodes::lang_en;
ScAddr TranslationKeynodes::translation_ignored_keynodes;
}  // namespace structureTranslationModule
