#pragma once

#include <sc-memory/sc_addr.hpp>

namespace formatTranslators
{
class FormatTranslatorsConstants
{
public:
  static sc_uint32 const KEY_ELEMENTS_ORDERING_WAIT_TIME;
  static sc_uint32 const SCG_VISUALISATION_WAIT_TIME;

  static ScAddr GetDefaultLanguage();

};
}  // namespace formatTranslators
