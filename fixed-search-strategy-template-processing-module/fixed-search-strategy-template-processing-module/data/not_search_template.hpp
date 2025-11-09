#pragma once

/*!
 * @file not_search_template.hpp
 * @brief Negation wrapper for SearchTemplate providing inverse search semantics.
 *
 * This module provides the NotSearchTemplate class, which implements negated pattern
 * matching against the sc-memory knowledge base. It extends SearchTemplate to invert
 * search semantics: the template succeeds when the base pattern is NOT found in the
 * knowledge base, and fails when the pattern IS found.
 *
 * NotSearchTemplate is primarily used in constraint validation and filtering scenarios
 * where the absence of certain patterns must be verified. It enables expressing negative
 * constraints in template-based queries, supporting use cases such as:
 * - Verifying that certain patterns do NOT exist before performing operations
 * - Implementing exclusion criteria in complex multi-template queries
 * - Validating that knowledge base states satisfy negative constraints
 * - Supporting "must not match" filtering in result refinement pipelines
 *
 * The class maintains all search capabilities of SearchTemplate but reverses the
 * success/failure semantics, making it suitable for constraint checking and validation
 * workflows where non-existence of patterns is the desired condition.
 *
 * @see search_template.hpp
 * @see SearchTemplate
 * @see ParameterizedTemplate
 * @see ScAgentContext
 */

#include "search_template.hpp"

class ScAgentContext;

/*!
 * @class NotSearchTemplate
 * @brief Negated search template that succeeds when patterns are NOT found.
 *
 * NotSearchTemplate provides a specialized implementation of SearchTemplate that inverts
 * the standard search semantics. Where SearchTemplate returns success when a pattern is
 * found in the knowledge base, NotSearchTemplate returns success when the pattern is
 * NOT found. This inversion is achieved by delegating to the parent SearchTemplate's
 * ApplyImpl method and negating its return value.
 *
 * @inherits SearchTemplate
 * @see ParameterizedTemplateBuilder
 * @see SearchTemplate
 * @see ParameterizedTemplate::NotFilter
 *
 * @thread_safety Not thread-safe; external synchronization required for concurrent use.
 */
class NotSearchTemplate : public SearchTemplate
{
  friend class ParameterizedTemplateBuilder;

public:
  /*!
   * @brief Destructor for NotSearchTemplate.
   *
   * Cleans up resources allocated by the NotSearchTemplate instance. Since this class
   * delegates all search logic to SearchTemplate and does not maintain additional state,
   * the destructor performs minimal cleanup beyond base class destruction.
   */
  ~NotSearchTemplate() override;

protected:
  /*!
   * @brief Executes negated search logic by inverting SearchTemplate results.
   *
   * This method implements the core negation logic for NotSearchTemplate by delegating
   * to the parent SearchTemplate::ApplyImpl method and inverting its return value.
   *
   * The negation semantics make this template suitable for constraint validation where
   * the absence of a pattern is the desired condition. Note that when this method returns
   * true (success), the results container may be populated by the base SearchTemplate but
   * those results represent the "forbidden" pattern that was fortunately not found.
   *
   *
   * @param params      [in] Reference to template parameters extracted from the knowledge
   *                         base, passed through to SearchTemplate::ApplyImpl for pattern
   *                         construction and matching.
   *
   * @param arguments   [in] Reference to runtime arguments providing variable bindings
   *                         for the search operation, passed through to the base template.
   *
   * @param results     [in,out] Reference to the results container. The base SearchTemplate
   *                             may populate this container during its search, but the
   *                             populated results are only meaningful if this method returns
   *                             false (indicating the unwanted pattern was found). When this
   *                             method returns true (success), results should be disregarded.
   *
   * @param callbacks   [in] Reference to filter callback functions passed through to the
   *                         base SearchTemplate for constraint application during the
   *                         underlying search operation.
   *
   * @return @c true if the search pattern was NOT found in the knowledge base (negated
   *         success, representing absence of the pattern); @c false if the search pattern
   *         WAS found (negated failure, representing presence of the pattern).
   *
   * @note The results parameter is not reliably useful with NotSearchTemplate. The primary
   *       value of this template is its boolean return indicating pattern absence/presence,
   *       not the collection of matched constructions.
   *
   * @see SearchTemplate::ApplyImpl
   * @see ParameterizedTemplate::NotFilter
   */
  bool ApplyImpl(
      ScTemplateParams const & params,
      TemplateArguments const & arguments,
      TemplateResults & results,
      std::list<FilterCallback> const & callbacks) const override;

protected:
  /*!
   * @brief Protected constructor for initialization by ParameterizedTemplateBuilder.
   *
   * Constructs a NotSearchTemplate instance by delegating to the SearchTemplate base
   * class constructor. This constructor is protected to ensure that NotSearchTemplate
   * instances are created only through the builder pattern, maintaining proper
   * initialization and configuration.
   *
   * The constructor establishes the same context, logging, and template address
   * configuration as SearchTemplate, as the negation logic is applied only at the
   * ApplyImpl level through result inversion.
   *
   * @param context       [in] Reference to the ScAgentContext providing knowledge base
   *                           access and agent-specific operations for template search.
   *
   * @param logger        [in] Reference to the ScLogger instance used for logging negated
   *                           search operations, debugging information, and result reporting.
   *
   * @param templateAddr  [in] Constant reference to the ScAddr of the template node
   *                           in the knowledge base that defines the search pattern
   *                           whose absence will be verified.
   *
   * @see SearchTemplate::SearchTemplate
   * @see ParameterizedTemplateBuilder
   */
  NotSearchTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr);
};