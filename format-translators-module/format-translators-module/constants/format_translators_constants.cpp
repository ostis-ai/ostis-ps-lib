#include "format_translators_constants.hpp"

#include "keynodes/format_translators_keynodes.hpp"

namespace formatTranslators
{

sc_uint32 const FormatTranslatorsConstants::KEY_ELEMENTS_ORDERING_WAIT_TIME = 1000;
sc_uint32 const FormatTranslatorsConstants::SCG_VISUALISATION_WAIT_TIME = 300000;
std::string const FormatTranslatorsConstants::GWF_CONTENT_BEFORE_STATIC_SECTOR_CONTENT =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<GWF version=\"2.0\">\n"
    "    <staticSector>\n";
std::string const FormatTranslatorsConstants::GWF_CONTENT_AFTER_STATIC_SECTOR_CONTENT =
    "    </staticSector>\n"
    "</GWF>\n";
float const FormatTranslatorsConstants::CONNECTOR_IDENTIFIER_SCALE = 0.4f;
float const FormatTranslatorsConstants::CONNECTOR_INCIDENT_POINT_PERCENT = 0.5f;
float const FormatTranslatorsConstants::REVERSED_CONNECTOR_INCIDENT_POINT_PERCENT =
    1 - CONNECTOR_INCIDENT_POINT_PERCENT;
float const FormatTranslatorsConstants::Y_INCREMENT = 100;
float const FormatTranslatorsConstants::HALF_Y_INCREMENT = Y_INCREMENT / 2;
float const FormatTranslatorsConstants::INDENT_FOR_NODE_WITHOUT_IDENTIFIER = 100;
float const FormatTranslatorsConstants::MIN_X = 200;
float const FormatTranslatorsConstants::MAX_X = 800;
float const FormatTranslatorsConstants::MAX_LINK_WIDTH = 60;
float const FormatTranslatorsConstants::EMPTY_SPACE_AFTER_IDENTIFIER = 8;
float const FormatTranslatorsConstants::IDENTIFIER_CHARACTER_WIDTH = 6;
int const FormatTranslatorsConstants::NEGATIVE_BALANCE = -1;

ScAddr FormatTranslatorsConstants::GetDefaultLanguage()
{
  return FormatTranslatorsKeynodes::lang_en;
}
}  // namespace formatTranslators
