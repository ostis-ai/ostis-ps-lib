#pragma once

/*!
 * @file search_template.hpp
 * @brief Concrete implementation of ParameterizedTemplate for semantic memory search operations.
 *
 * This module provides the SearchTemplate class, which implements pattern-based search
 * functionality against the sc-memory knowledge base. It extends ParameterizedTemplate
 * to support template-driven queries where patterns are matched against the semantic
 * network structure, collecting all matching constructions.
 *
 * SearchTemplate is designed for read-only query operations that retrieve existing
 * knowledge base elements without modifying the memory structure. It supports:
 * - Variable substitution and parameter binding for flexible queries
 * - Filter-based result refinement through callback mechanisms
 * - Result sorting and parameter erasure according to template configuration
 * - Integration with the sc-memory template search API
 *
 * @see parameterized_template.hpp
 * @see ParameterizedTemplate
 * @see ScAgentContext
 */

#include "parameterized_template.hpp"

class ScAgentContext;

/*!
 * @class SearchTemplate
 * @brief Parameterized template implementation for searching patterns in the knowledge base.
 *
 * SearchTemplate provides a specialized implementation of ParameterizedTemplate focused
 * on executing search queries against the sc-memory semantic network. It translates
 * high-level template specifications into concrete pattern matching operations,
 * handling parameter substitution, result collection, and filter application.
 *
 * The class performs non-destructive read operations, making it suitable for query
 * and retrieval tasks in sc-agent workflows. Search operations are executed through
 * the sc-memory template API, which efficiently matches structural patterns defined
 * by the template against the knowledge base graph.
 *
 * @inherits ParameterizedTemplate
 * @see ParameterizedTemplateBuilder
 * @see TemplateArguments
 * @see TemplateResults
 *
 * @thread_safety Not thread-safe; external synchronization required for concurrent use.
 */
class SearchTemplate : public ParameterizedTemplate
{
  friend class ParameterizedTemplateBuilder;

public:
  /*!
   * @brief Destructor for SearchTemplate.
   *
   * Cleans up resources allocated by the SearchTemplate instance. Since this class
   * primarily works with sc-memory references rather than owned resources, the
   * destructor performs minimal cleanup beyond base class destruction.
   */
  ~SearchTemplate() override;

protected:
  /*!
   * @brief Executes the search template against the knowledge base and collects results.
   *
   * This method implements the core search logic for the SearchTemplate. It performs
   * the following operations:
   *
   * 1. Initializes or updates the TemplateResults object with template configuration
   *    addresses (template, sort parameter, erase parameters, result parameters)
   * 2. Builds and executes a ScTemplate search query with parameter substitutions
   * 3. Collects matching results, applying filter callbacks for constraint validation
   * 4. Returns success/failure status based on search outcome
   *
   * The search operation is non-destructive and does not modify the knowledge base.
   * If all template variables are replaced by concrete parameter values, the search
   * is considered successful with an empty result set (the template is fully grounded).
   * Otherwise, the method searches for all constructions matching the template pattern.
   *
   * @param params      [in] Reference to template parameters extracted from the knowledge
   *                         base, containing variable replacements and template-specific
   *                         configuration used for ScTemplate construction.
   *
   * @param arguments   [in] Reference to runtime arguments providing variable bindings
   *                         and constraints for this specific search operation.
   *
   * @param results     [in,out] Reference to the results container. If valid, it is updated
   *                             with template configuration addresses; if invalid, it is
   *                             initialized with these addresses. Matched patterns are
   *                             appended to this container.
   *
   * @param callbacks   [in] Reference to a list of filter callback functions that define
   *                         additional constraints applied during result collection. Each
   *                         callback can reject results that don't satisfy specific criteria.
   *
   * @return @c true if the search was executed successfully and at least one result was
   *         found (or all variables were fully replaced, making the template concrete);
   *         @c false if the search failed to find any matches or encountered an error.
   *
   * @see TrySearchByTemplate
   * @see TemplateResults::CollectFromSearchResult
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
   * Constructs a SearchTemplate instance with the necessary context and configuration
   * for executing search operations against the knowledge base. This constructor is
   * protected to ensure that SearchTemplate instances are created only through the
   * builder pattern, maintaining proper initialization and configuration.
   *
   * The constructor delegates to the ParameterizedTemplate base class constructor
   * to establish agent context, logging, and template address references.
   *
   * @param context       [in] Reference to the ScAgentContext providing knowledge base
   *                           access and agent-specific operations for template search.
   *
   * @param logger        [in] Reference to the ScLogger instance used for logging search
   *                           operations, debugging information, and error reporting.
   *
   * @param templateAddr  [in] Constant reference to the ScAddr of the template node
   *                           in the knowledge base that defines the search pattern
   *                           structure and configuration.
   *
   * @see ParameterizedTemplate::ParameterizedTemplate
   * @see ParameterizedTemplateBuilder
   */
  SearchTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr);

  /*!
   * @brief Builds and executes a template search, populating the search result.
   *
   * This helper method encapsulates the low-level template building and search
   * execution against the sc-memory knowledge base.
   *
   * The method uses the sc-memory template API to perform efficient graph pattern matching
   * against the semantic network structure.
   *
   * @param params         [in] Reference to template parameters containing variable bindings
   *                            and substitutions to apply during template construction.
   *
   * @param searchResult   [out] Reference to a ScTemplateSearchResult object that will be
   *                             populated with all matching constructions found in the
   *                             knowledge base. Each match represents a complete variable
   *                             binding that satisfies the template pattern.
   *
   * @return @c true if the search was successful (either found matches or determined that
   *         all variables are replaced); @c false if the search failed to find any matches
   *         or encountered an error during template building or search execution.
   *
   * @note The method returns @c true when all variables are replaced even without searching,
   *       as this indicates the template is fully concrete and represents a valid state.
   *
   * @see ScAgentContext::BuildTemplate
   * @see ScAgentContext::SearchByTemplate
   */
  bool TrySearchByTemplate(ScTemplateParams const & params, ScTemplateSearchResult & searchResult) const;
};
