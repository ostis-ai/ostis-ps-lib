#pragma once

#include <sc-memory/sc_keynodes.hpp>

class Keynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_process_fixed_search_strategy_template{
      "action_process_fixed_search_strategy_template",
      ScType::ConstNodeClass};
  static inline ScKeynode const concept_template_type{"concept_template_type", ScType::ConstNodeClass};

  static inline ScKeynode const nrel_search_template{"nrel_search_template", ScType::ConstNodeClass};
  static inline ScKeynode const nrel_not_search_template{"nrel_not_search_template", ScType::ConstNodeClass};
  static inline ScKeynode const nrel_search_set_template{"nrel_search_set_template", ScType::ConstNodeClass};
  static inline ScKeynode const nrel_generate_template{"nrel_generate_template", ScType::ConstNodeClass};
  static inline ScKeynode const nrel_wait_template{"nrel_wait_template", ScType::ConstNodeClass};
  static inline ScKeynode const nrel_fixed_search_strategy_template{
      "nrel_fixed_search_strategy_template",
      ScType::ConstNodeClass};

  static inline ScKeynode const rrel_template{"rrel_template", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_filter_templates{"rrel_filter_templates", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_not_filter_templates{"rrel_not_filter_templates", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_init_template{"rrel_init_template", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_next_template{"rrel_next_template", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_wait_time{"rrel_wait_time", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_template_sort_param{"rrel_template_sort_param", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_template_input_params{"rrel_template_input_params", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_template_output_params{"rrel_template_output_params", ScType::ConstNodeRole};
  static inline ScKeynode const rrel_template_erase_params{"rrel_template_erase_params", ScType::ConstNodeRole};
};
