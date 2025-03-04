#pragma once

#include <sc-memory/sc_keynodes.hpp>

namespace formatTranslators
{
class FormatTranslatorsKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const nrel_key_elements_order{"nrel_key_elements_order", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_format{"nrel_format", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_translation{"nrel_translation", ScType::ConstNodeNonRole};
  static inline ScKeynode const action_visualise_to_scg{"action_visualise_to_scg", ScType::ConstNodeClass};
  static inline ScKeynode const action_translate_to_format{"action_translate_to_format", ScType::ConstNodeClass};
  static inline ScKeynode const action_divide_structure{"action_divide_structure", ScType::ConstNodeClass};
  static inline ScKeynode const action_select_main_key_element{
      "action_select_main_key_element",
      ScType::ConstNodeClass};
  static inline ScKeynode const action_classify_structure{"action_classify_structure", ScType::ConstNodeClass};
  static inline ScKeynode const action_order_key_elements{"action_order_key_elements", ScType::ConstNodeClass};
  static inline ScKeynode const ui_command_initiated{"ui_command_initiated", ScType::ConstNodeClass};
  static inline ScKeynode const ui_command_translate_from_sc{"ui_command_translate_from_sc", ScType::ConstNodeClass};
  static inline ScKeynode const ui_rrel_output_format{"ui_rrel_output_format", ScType::ConstNodeRole};
  static inline ScKeynode const ui_rrel_source_sc_construction{"ui_rrel_source_sc_construction", ScType::ConstNodeRole};
  static inline ScKeynode const ui_rrel_user_lang{"ui_rrel_user_lang", ScType::ConstNodeRole};
  static inline ScKeynode const format_scg_json{"format_scg_json", ScType::ConstNodeClass};
  static inline ScKeynode const lang_en{"lang_en", ScType::ConstNodeClass};
};

}  // namespace formatTranslators
