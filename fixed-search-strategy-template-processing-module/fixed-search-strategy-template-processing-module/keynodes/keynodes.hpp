#pragma once

/*!
 * @file keynodes.hpp
 * @brief System keynodes definitions for the parameterized template framework.
 *
 * This module defines the Keynodes class, which provides statically-accessible sc-memory
 * keynode identifiers used throughout the parameterized template system. Keynodes are
 * well-known semantic entities in the knowledge base that have fixed system identifiers,
 * allowing consistent reference across different components.
 *
 * The keynodes defined here establish the semantic vocabulary for:
 * - Template type classification (search, generate, wait, etc.)
 * - Template composition and strategy specification
 * - Role relations for template configuration
 * - Parameter and result management
 * - Action processing triggers
 *
 * @see ScKeynodes
 * @see ParameterizedTemplate
 * @see ParameterizedTemplateBuilder
 */

#include <sc-memory/sc_keynodes.hpp>

/*!
 * @class Keynodes
 * @brief Container for parameterized template system keynode definitions.
 *
 * The Keynodes class extends ScKeynodes to provide static access to semantic identifiers
 * used throughout the parameterized template framework. All keynodes are defined as
 * static inline members, making them globally accessible without requiring instantiation.
 *
 * Keynodes are accessed directly as static members:
 *
 * @code
 * // Check if a template is a search template
 * bool isSearchTemplate = context.CheckConnector(
 *     Keynodes::nrel_search_template,
 *     templateAddr,
 *     ScType::ConstPosArc
 * );
 *
 * // Get the init template from a fixed strategy template
 * ScAddrToValueUnorderedMap<ScAddr> elements;
 * template.GetElementsByRoles({Keynodes::rrel_init_template}, elements);
 * @endcode
 *
 * @inherits ScKeynodes
 * @see ScKeynode
 * @see ScKeynodes
 *
 * @thread_safety All keynodes are const and thread-safe for read access after initialization.
 */
class Keynodes : public ScKeynodes
{
public:
  /*!
   * @name Action Keynodes
   * @brief Action identifiers for template processing operations.
   * @{
   */

  /*!
   * @brief Action for processing fixed search strategy templates.
   *
   * This action triggers the execution of FixedStrategySearchTemplate instances,
   * which orchestrate multi-stage template processing with init-next execution patterns.
   * Agents subscribe to this action class to handle fixed strategy template requests.
   *
   * System identifier: "action_process_fixed_search_strategy_template"
   *
   * @see FixedStrategySearchTemplate
   */
  static inline ScKeynode const action_process_fixed_search_strategy_template{
      "action_process_fixed_search_strategy_template",
      ScType::ConstNodeClass};

  /*!
   * @}
   * @name Concept Keynodes
   * @brief Classification concepts for template system entities.
   * @{
   */

  /*!
   * @brief Concept identifying template type classification.
   *
   * This concept serves as a superclass for all template type concepts, providing
   * a common semantic category for template classification purposes.
   *
   * System identifier: "concept_template_type"
   *
   * @see nrel_search_template
   * @see nrel_generate_template
   */
  static inline ScKeynode const concept_template_type{"concept_template_type", ScType::ConstNodeClass};

  /*!
   * @}
   * @name Non-Role Relations (nrel_*)
   * @brief Semantic relations defining template types and characteristics.
   * @{
   */

  /*!
   * @brief Relation identifying search templates.
   *
   * Templates marked with this relation are processed as SearchTemplate instances,
   * performing read-only pattern matching queries against the knowledge base.
   * The relation connects a template node to indicate its search template type.
   *
   * System identifier: "nrel_search_template"
   *
   * @see SearchTemplate
   * @see ParameterizedTemplateBuilder::BuildTemplate
   */
  static inline ScKeynode const nrel_search_template{"nrel_search_template", ScType::ConstNodeClass};

  /*!
   * @brief Relation identifying not-search templates.
   *
   * Templates marked with this relation are processed as NotSearchTemplate instances,
   * performing negated pattern matching where success indicates pattern absence.
   * Used for negative constraints and exclusion criteria.
   *
   * System identifier: "nrel_not_search_template"
   *
   * @see NotSearchTemplate
   * @see ParameterizedTemplate::NotFilter
   */
  static inline ScKeynode const nrel_not_search_template{"nrel_not_search_template", ScType::ConstNodeClass};

  /*!
   * @brief Relation identifying search set templates.
   *
   * Templates marked with this relation produce results that should be organized
   * as sets with explicit connection relationships. This affects result aggregation
   * strategy in multi-stage template processing (ConnectTemplateResults vs MergeTemplateResults).
   *
   * System identifier: "nrel_search_set_template"
   *
   * @see FixedStrategySearchTemplate::ProcessNextTemplates
   * @see TemplateResults::ConnectTemplateResults
   */
  static inline ScKeynode const nrel_search_set_template{"nrel_search_set_template", ScType::ConstNodeClass};

  /*!
   * @brief Relation identifying generate templates.
   *
   * Templates marked with this relation are processed as GenerateTemplate instances,
   * performing write operations that create new structures in the knowledge base.
   *
   * System identifier: "nrel_generate_template"
   *
   * @see GenerateTemplate
   * @see ParameterizedTemplateBuilder::BuildTemplate
   */
  static inline ScKeynode const nrel_generate_template{"nrel_generate_template", ScType::ConstNodeClass};

  /*!
   * @brief Relation identifying wait templates.
   *
   * Templates marked with this relation are processed as WaitTemplate instances,
   * performing time-bounded polling searches that wait for patterns to appear
   * in the knowledge base.
   *
   * System identifier: "nrel_wait_template"
   *
   * @see WaitTemplate
   * @see ParameterizedTemplateBuilder::BuildTemplate
   */
  static inline ScKeynode const nrel_wait_template{"nrel_wait_template", ScType::ConstNodeClass};

  /*!
   * @brief Relation identifying fixed search strategy templates.
   *
   * Templates marked with this relation are processed as FixedStrategySearchTemplate
   * instances, orchestrating multi-stage template execution with init-next patterns.
   *
   * System identifier: "nrel_fixed_search_strategy_template"
   *
   * @see FixedStrategySearchTemplate
   * @see ParameterizedTemplateBuilder::BuildTemplate
   */
  static inline ScKeynode const nrel_fixed_search_strategy_template{
      "nrel_fixed_search_strategy_template",
      ScType::ConstNodeClass};

  /*!
   * @}
   * @name Role Relations (rrel_*)
   * @brief Role relations for template component and parameter specification.
   * @{
   */

  /*!
   * @brief Role identifying the primary template entity.
   *
   * This role relation marks the main template entity within a template specification
   * or template-related structure, distinguishing it from other related entities.
   *
   * System identifier: "rrel_template"
   *
   * @see ParameterizedTemplate
   */
  static inline ScKeynode const rrel_template{"rrel_template", ScType::ConstNodeRole};

  /*!
   * @brief Role identifying the set of filter templates.
   *
   * This role marks the entity containing positive filter templates (FilterTemplate
   * instances) that define inclusion constraints. Elements connected via this role
   * are collected into ParameterizedTemplate::m_filterTemplateAddrs.
   *
   * System identifier: "rrel_filter_templates"
   *
   * @see FilterTemplate
   * @see ParameterizedTemplate::m_filterTemplateAddrs
   * @see ParameterizedTemplate::Filter
   */
  static inline ScKeynode const rrel_filter_templates{"rrel_filter_templates", ScType::ConstNodeRole};

  /*!
   * @brief Role identifying the set of not-filter templates.
   *
   * This role marks the entity containing negative filter templates (NotFilterTemplate
   * instances) that define exclusion constraints. Elements connected via this role
   * are collected into ParameterizedTemplate::m_notFilterTemplateAddrs.
   *
   * System identifier: "rrel_not_filter_templates"
   *
   * @see NotFilterTemplate
   * @see ParameterizedTemplate::m_notFilterTemplateAddrs
   * @see ParameterizedTemplate::NotFilter
   */
  static inline ScKeynode const rrel_not_filter_templates{"rrel_not_filter_templates", ScType::ConstNodeRole};

  /*!
   * @brief Role identifying the initial template in a fixed strategy.
   *
   * This role marks the template that should be executed first in a FixedStrategySearchTemplate,
   * producing initial results that feed into the next template stage.
   *
   * System identifier: "rrel_init_template"
   *
   * @see FixedStrategySearchTemplate::m_initTemplateAddr
   * @see FixedStrategySearchTemplate::Load
   */
  static inline ScKeynode const rrel_init_template{"rrel_init_template", ScType::ConstNodeRole};

  /*!
   * @brief Role identifying the next template in a fixed strategy.
   *
   * This role marks the template that should be executed for each result from the
   * initial template in a FixedStrategySearchTemplate, enabling multi-stage processing.
   *
   * System identifier: "rrel_next_template"
   *
   * @see FixedStrategySearchTemplate::m_nextTemplateAddr
   * @see FixedStrategySearchTemplate::ProcessNextTemplates
   */
  static inline ScKeynode const rrel_next_template{"rrel_next_template", ScType::ConstNodeRole};

  /*!
   * @brief Role identifying the wait time configuration.
   *
   * This role marks the entity specifying the timeout duration (in milliseconds)
   * for WaitTemplate polling operations. Referenced by ParameterizedTemplate::m_waitTimeMsAddr.
   *
   * System identifier: "rrel_wait_time"
   *
   * @see WaitTemplate
   * @see ParameterizedTemplate::m_waitTimeMsAddr
   * @see WaitTemplate::DEFAULT_WAIT_TEMPLATE_INTERVAL_MS
   */
  static inline ScKeynode const rrel_wait_time{"rrel_wait_time", ScType::ConstNodeRole};

  /*!
   * @brief Role identifying the template sorting parameter.
   *
   * This role marks the variable or entity that should be used as the sorting criterion
   * for template result ordering. Referenced by ParameterizedTemplate::m_sortParamAddr.
   *
   * System identifier: "rrel_template_sort_param"
   *
   * @see ParameterizedTemplate::m_sortParamAddr
   * @see TemplateResults::SortResultIndices
   */
  static inline ScKeynode const rrel_template_sort_param{"rrel_template_sort_param", ScType::ConstNodeRole};

  /*!
   * @brief Role identifying the template input parameters set.
   *
   * This role marks the set entity containing required input parameter specifications
   * for a template. The parameters define which variable bindings must be provided
   * via TemplateArguments for template execution. Referenced by ParameterizedTemplate::m_inputParamsAddr.
   *
   * System identifier: "rrel_template_input_params"
   *
   * @see ParameterizedTemplate::m_inputParamsAddr
   * @see TemplateArguments::GetTemplateParams
   */
  static inline ScKeynode const rrel_template_input_params{"rrel_template_input_params", ScType::ConstNodeRole};

  /*!
   * @brief Role identifying the template output parameters set.
   *
   * This role marks the set entity containing output parameter specifications for a
   * template. The parameters define which variable bindings from results should be
   * extracted and made available for template chaining. Referenced by ParameterizedTemplate::m_resultParamsAddr.
   *
   * System identifier: "rrel_template_output_params"
   *
   * @see ParameterizedTemplate::m_resultParamsAddr
   * @see TemplateResults::GetResultParams
   * @see TemplateResults::TryUpdateArguments
   */
  static inline ScKeynode const rrel_template_output_params{"rrel_template_output_params", ScType::ConstNodeRole};

  /*!
   * @brief Role identifying the template erase parameters set.
   *
   * This role marks the set entity containing erase parameter specifications for a
   * template. The parameters define which variable arcs should be removed from the
   * knowledge base during result processing. Referenced by ParameterizedTemplate::m_eraseParamsAddr.
   *
   * System identifier: "rrel_template_erase_params"
   *
   * @see ParameterizedTemplate::m_eraseParamsAddr
   * @see TemplateResults::GetEraseParams
   * @see TemplateResults::ProcessSingleResultItem
   */
  static inline ScKeynode const rrel_template_erase_params{"rrel_template_erase_params", ScType::ConstNodeRole};

  /*!
   * @}
   */
};
