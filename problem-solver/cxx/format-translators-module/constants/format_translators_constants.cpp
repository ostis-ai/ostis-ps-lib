#include "format_translators_constants.hpp"

#include "keynodes/format_translators_keynodes.hpp"

namespace formatTranslators
{


sc_uint32 const FormatTranslatorsConstants::KEY_ELEMENTS_ORDERING_WAIT_TIME = 1000;
sc_uint32 const FormatTranslatorsConstants::SCG_VISUALISATION_WAIT_TIME = 300000;

ScAddr FormatTranslatorsConstants::GetDefaultLanguage()
{
  return FormatTranslatorsKeynodes::lang_en;
}
}  // namespace formatTranslators
