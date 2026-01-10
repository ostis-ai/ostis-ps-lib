#pragma once

/*!
 * @file filter_template.hpp
 * @brief Filtering wrapper for SearchTemplate providing positive constraint validation.
 *
 * This module provides the FilterTemplate class, which implements positive constraint
 * validation against the sc-memory knowledge base. It extends SearchTemplate with
 * inverted semantics specifically designed for filtering use cases: the template succeeds
 * when the pattern is NOT found (constraint satisfied) and fails when the pattern IS
 * found (constraint violated).
 *
 * FilterTemplate serves as a positive filter in template processing pipelines, working
 * in conjunction with ParameterizedTemplate's filtering mechanism. Despite its inverted
 * return semantics (similar to NotSearchTemplate), it is semantically a "positive filter"
 * because it defines conditions that must be satisfied (pattern absence) for results to
 * pass through the filter.
 *
 * @see search_template.hpp
 * @see SearchTemplate
 * @see NotSearchTemplate
 * @see ParameterizedTemplate::Filter
 * @see ScAgentContext
 */

#include "search_template.hpp"

class ScAgentContext;

/*!
 * @class FilterTemplate
 * @brief Positive filter template that validates constraints through pattern absence checking.
 *
 * FilterTemplate provides a specialized implementation of SearchTemplate designed specifically
 * for use in filtering pipelines. It inverts standard search semantics (similar to
 * NotSearchTemplate) to support constraint validation: success indicates the pattern was
 * NOT found, meaning the constraint is satisfied and the data being filtered should pass.
 *
 * The class is integrated with ParameterizedTemplate's filtering mechanism through the
 * friend relationship and is typically invoked via ParameterizedTemplate::Filter. When
 * used in this context, FilterTemplate acts as a positive constraint that must be satisfied
 * for template results to be included.
 *
 * @inherits SearchTemplate
 * @see ParameterizedTemplate::Filter
 * @see ParameterizedTemplate::m_filterTemplateAddrs
 * @see NotSearchTemplate
 *
 * @thread_safety Not thread-safe; external synchronization required for concurrent use.
 */
class FilterTemplate : public SearchTemplate
{
  friend class ParameterizedTemplate;

public:
  /*!
   * @brief Destructor for FilterTemplate.
   *
   * Cleans up resources allocated by the FilterTemplate instance. Since this class
   * delegates all search logic to SearchTemplate and does not maintain additional state,
   * the destructor performs minimal cleanup beyond base class destruction.
   */
  ~FilterTemplate() override;

protected:
  /*!
   * @brief Executes filter validation by inverting SearchTemplate results.
   *
   * This method implements the core filtering logic by delegating to the parent
   * SearchTemplate::ApplyImpl method and inverting its return value.
   *
   * @param params      [in] Reference to template parameters extracted from the knowledge
   *                         base, passed through to SearchTemplate::ApplyImpl for pattern
   *                         construction and constraint checking.
   *
   * @param arguments   [in] Reference to runtime arguments providing variable bindings
   *                         for the filter evaluation, representing the data being validated.
   *
   * @param results     [in,out] Reference to the results container. The base SearchTemplate
   *                             may populate this container during constraint checking, but
   *                             the populated results are only meaningful if this method
   *                             returns false (indicating the constraint-violating pattern
   *                             was found). When this method returns true (filter passes),
   *                             results should be disregarded.
   *
   * @param callbacks   [in] Reference to filter callback functions passed through to the
   *                         base SearchTemplate for additional constraint application during
   *                         the underlying search operation.
   *
   * @return @c true if the constraint is satisfied (pattern NOT found, data passes filter);
   *         @c false if the constraint is violated (pattern IS found, data rejected by filter).
   *
   * @note Similar to NotSearchTemplate, the results parameter is not reliably useful with
   *       FilterTemplate. The primary value is the boolean return indicating whether the
   *       filter constraint is satisfied, not the collection of matched constructions.
   *
   * @see SearchTemplate::ApplyImpl
   * @see ParameterizedTemplate::Filter
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
   * Constructs a FilterTemplate instance by delegating to the SearchTemplate base
   * class constructor. This constructor is protected and has ParameterizedTemplate
   * as a friend to ensure that FilterTemplate instances are created and managed
   * properly within the filtering framework.
   *
   * Unlike other template types which are created by ParameterizedTemplateBuilder,
   * FilterTemplate instances are typically created and stored by ParameterizedTemplate
   * itself as part of its filtering mechanism (stored in m_filterTemplateAddrs).
   *
   * The constructor establishes the same context, logging, and template address
   * configuration as SearchTemplate, as the filtering logic is applied only at the
   * ApplyImpl level through result inversion.
   *
   * @param context       [in] Reference to the ScAgentContext providing knowledge base
   *                           access and agent-specific operations for template search
   *                           used in constraint validation.
   *
   * @param logger        [in] Reference to the ScLogger instance used for logging filter
   *                           operations, debugging information, and constraint validation
   *                           results.
   *
   * @param templateAddr  [in] Constant reference to the ScAddr of the template node
   *                           in the knowledge base that defines the constraint pattern
   *                           whose absence indicates filter success.
   *
   * @see SearchTemplate::SearchTemplate
   * @see ParameterizedTemplate
   * @see ParameterizedTemplate::m_filterTemplateAddrs
   */
  FilterTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr);
};
