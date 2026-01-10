#pragma once

/*!
 * @file not_filter_template.hpp
 * @brief Negative filtering wrapper providing exclusion constraint validation.
 *
 * This module provides the NotFilterTemplate class, which implements negative constraint
 * validation (exclusion filtering) against the sc-memory knowledge base. It extends
 * NotSearchTemplate and restores standard search semantics: the template succeeds when
 * the pattern IS found (exclusion constraint satisfied) and fails when the pattern is
 * NOT found (exclusion constraint violated).
 *
 * NotFilterTemplate serves as a negative filter in template processing pipelines, working
 * in conjunction with ParameterizedTemplate's filtering mechanism. By double-negating the
 * search semantics (NotSearchTemplate inverts SearchTemplate, then NotFilterTemplate inverts
 * NotSearchTemplate), it returns to standard search behavior but with filtering semantics:
 * success means "this exclusion pattern exists, so exclude the result."
 *
 * - Validation workflows where certain patterns indicate invalid or forbidden states
 *
 * @see not_search_template.hpp
 * @see NotSearchTemplate
 * @see FilterTemplate
 * @see SearchTemplate
 * @see ParameterizedTemplate::NotFilter
 * @see ScAgentContext
 */

#include "not_search_template.hpp"

class ScAgentContext;

/*!
 * @class NotFilterTemplate
 * @brief Negative filter template that validates exclusion constraints through pattern presence checking.
 *
 * NotFilterTemplate provides a specialized implementation of NotSearchTemplate designed
 * specifically for negative filtering (exclusion) use cases. Through double negation
 * (NotSearchTemplate inverts SearchTemplate, then NotFilterTemplate inverts NotSearchTemplate),
 * it restores standard search semantics: success when the pattern is found.
 *
 * However, in the filtering context, this has specific meaning: when the exclusion pattern
 * is found, it indicates that the result should be excluded from the final output. The
 * template returns true to signal "exclusion constraint satisfied - exclude this result."
 *
 * @inherits NotSearchTemplate
 * @see ParameterizedTemplate::NotFilter
 * @see ParameterizedTemplate::m_notFilterTemplateAddrs
 * @see FilterTemplate
 * @see NotSearchTemplate
 *
 * @thread_safety Not thread-safe; external synchronization required for concurrent use.
 */
class NotFilterTemplate : public NotSearchTemplate
{
  friend class ParameterizedTemplate;

public:
  /*!
   * @brief Destructor for NotFilterTemplate.
   *
   * Cleans up resources allocated by the NotFilterTemplate instance. Since this class
   * delegates all search logic to NotSearchTemplate and does not maintain additional state,
   * the destructor performs minimal cleanup beyond base class destruction.
   */
  ~NotFilterTemplate() override;

protected:
  /*!
   * @brief Executes exclusion filter validation by restoring standard search semantics.
   *
   * This method implements the negative filtering logic by delegating to the parent
   * NotSearchTemplate::ApplyImpl method and passing through its return value unchanged
   * (effectively inverting the inversion).
   *
   * @param params      [in] Reference to template parameters extracted from the knowledge
   *                         base, passed through to NotSearchTemplate::ApplyImpl for pattern
   *                         construction and exclusion pattern checking.
   *
   * @param arguments   [in] Reference to runtime arguments providing variable bindings
   *                         for the filter evaluation, representing the data being validated
   *                         against exclusion criteria.
   *
   * @param results     [in,out] Reference to the results container, passed through to
   *                             NotSearchTemplate. The results are populated if the exclusion
   *                             pattern is found (returns false).
   *
   * @param callbacks   [in] Reference to filter callback functions passed through to
   *                         NotSearchTemplate for additional constraint application.
   *
   * @return @c true if the exclusion pattern is NOT found (no exclusion needed, include data);
   *         @c false if the exclusion pattern IS found (exclusion needed, exclude data).
   *
   * @note The return semantics match NotSearchTemplate, not standard search. The "Not" in
   *       NotFilterTemplate refers to it being a negative/exclusion filter, not to inverting
   *       NotSearchTemplate's logic.
   *
   * @see NotSearchTemplate::ApplyImpl
   * @see ParameterizedTemplate::NotFilter
   */
  bool ApplyImpl(
      ScTemplateParams const & params,
      TemplateArguments const & arguments,
      TemplateResults & results,
      std::list<FilterCallback> const & callbacks) const override;

protected:
  /*!
   * @brief Protected constructor for initialization by ParameterizedTemplate.
   *
   * Constructs a NotFilterTemplate instance by delegating to the NotSearchTemplate base
   * class constructor. This constructor is protected and has ParameterizedTemplate as a
   * friend to ensure that NotFilterTemplate instances are created and managed properly
   * within the filtering framework.
   *
   * NotFilterTemplate instances are typically created and stored by ParameterizedTemplate
   * itself as part of its exclusion filtering mechanism (stored in m_notFilterTemplateAddrs).
   *
   * The constructor establishes the same context, logging, and template address
   * configuration as NotSearchTemplate, as the filtering logic is applied through
   * delegation to the parent class.
   *
   * @param context       [in] Reference to the ScAgentContext providing knowledge base
   *                           access and agent-specific operations for template search
   *                           used in exclusion constraint validation.
   *
   * @param logger        [in] Reference to the ScLogger instance used for logging negative
   *                           filter operations, debugging information, and exclusion
   *                           constraint validation results.
   *
   * @param templateAddr  [in] Constant reference to the ScAddr of the template node
   *                           in the knowledge base that defines the exclusion pattern
   *                           whose presence indicates results should be filtered out.
   *
   * @see NotSearchTemplate::NotSearchTemplate
   * @see ParameterizedTemplate
   * @see ParameterizedTemplate::m_notFilterTemplateAddrs
   */
  NotFilterTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr);
};