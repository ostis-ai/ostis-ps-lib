#pragma once

#include <sc-memory/sc_keynodes.hpp>

namespace formatTranslators
{
class FormatTranslatorsKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const nrel_key_elements_order{"nrel_key_elements_order", ScType::ConstNodeNonRole};
  static inline ScKeynode const action_visualise_to_scg{"action_visualise_to_scg", ScType::ConstNodeClass};
  static inline ScKeynode const lang_en{"lang_en", ScType::ConstNodeClass};
};

}  // namespace formatTranslators
