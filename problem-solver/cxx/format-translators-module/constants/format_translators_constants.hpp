#pragma once

#include <string>

#include <sc-memory/sc_addr.hpp>

namespace formatTranslators
{
class FormatTranslatorsConstants
{
public:
  static sc_uint32 const KEY_ELEMENTS_ORDERING_WAIT_TIME;
  static sc_uint32 const SCG_VISUALISATION_WAIT_TIME;

  static std::string const GWF_CONTENT_BEFORE_STATIC_SECTOR_CONTENT;
  static std::string const GWF_CONTENT_AFTER_STATIC_SECTOR_CONTENT;
  // CONNECTOR_IDENTIFIER_SCALE was set after a couple of experiments because it makes connector long enough
  // for identifier to not overlap with other element
  static float const CONNECTOR_IDENTIFIER_SCALE;
  static float const CONNECTOR_INCIDENT_POINT_PERCENT;
  static float const REVERSED_CONNECTOR_INCIDENT_POINT_PERCENT;
  static float const Y_INCREMENT;
  static float const HALF_Y_INCREMENT;
  static float const MIN_X;
  static float const MAX_Y;
  static float const MAX_LINK_WIDTH;
  static float const EMPTY_SPACE_AFTER_IDENTIFIER;

  static ScAddr GetDefaultLanguage();
};
}  // namespace formatTranslators
