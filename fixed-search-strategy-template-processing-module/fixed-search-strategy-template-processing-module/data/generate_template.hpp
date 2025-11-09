#pragma once

/*!
 * @file generate_template.hpp
 * @brief Concrete implementation of ParameterizedTemplate for semantic memory construction operations.
 *
 * This module provides the GenerateTemplate class, which implements pattern-based generation
 * functionality for creating new structures in the sc-memory knowledge base. It extends
 * ParameterizedTemplate to support template-driven construction operations where new semantic
 * network elements (nodes, edges, and their relationships) are instantiated based on
 * predefined patterns.
 *
 * GenerateTemplate is designed for write operations that modify the knowledge base by
 * creating new constructions. Unlike SearchTemplate which performs read-only queries,
 * GenerateTemplate actively constructs new knowledge base elements according to template
 * specifications. It supports:
 * - Construction of new semantic network structures from template patterns
 * - Variable substitution with parameter binding for flexible generation
 * - Automatic creation of nodes, edges, and relationships as specified in templates
 * - Result collection of all generated elements for further processing
 * - Integration with the sc-memory template generation API
 *
 * @see parameterized_template.hpp
 * @see ParameterizedTemplate
 * @see SearchTemplate
 * @see ScAgentContext
 */

#include "parameterized_template.hpp"

class ScAgentContext;

/*!
 * @class GenerateTemplate
 * @brief Parameterized template implementation for generating new structures in the knowledge base.
 *
 * GenerateTemplate provides a specialized implementation of ParameterizedTemplate focused
 * on executing construction operations against the sc-memory semantic network. It translates
 * high-level template specifications into concrete generation operations that create new
 * knowledge base elements, handling parameter substitution, construction execution, and
 * result collection.
 *
 * The class performs destructive write operations, making it suitable for knowledge base
 * construction, data insertion, and semantic network augmentation tasks in sc-agent workflows.
 * Generation operations are executed through the sc-memory template API, which efficiently
 * instantiates structural patterns defined by the template into the knowledge base graph.
 *
 * @inherits ParameterizedTemplate
 * @see ParameterizedTemplateBuilder
 * @see SearchTemplate
 * @see TemplateArguments
 * @see TemplateResults
 *
 * @thread_safety Not thread-safe; external synchronization required for concurrent use.
 * @warning Generation operations modify the knowledge base and cannot be undone. Ensure
 *          proper validation before applying generation templates.
 */
class GenerateTemplate : public ParameterizedTemplate
{
  friend class ParameterizedTemplateBuilder;

public:
  /*!
   * @brief Destructor for GenerateTemplate.
   *
   * Cleans up resources allocated by the GenerateTemplate instance. Since this class
   * primarily works with sc-memory references rather than owned resources, the
   * destructor performs minimal cleanup beyond base class destruction.
   *
   * @note Generation operations that have already been performed are permanent changes
   *       to the knowledge base and are not rolled back during destruction.
   */
  ~GenerateTemplate() override;

protected:
  /*!
   * @brief Executes the generation template to create new structures in the knowledge base.
   *
   * This method implements the core generation logic for the GenerateTemplate. It performs
   * the following operations:
   *
   * 1. Builds and executes a ScTemplate generation operation with parameter substitutions
   * 2. Creates new semantic network constructions (nodes, edges, relationships) according
   *    to the template specification
   * 3. Initializes a new TemplateResults object with template configuration addresses
   *    (template, sort parameter, erase parameters, result parameters)
   * 4. Collects all generated elements into the results container for further processing
   * 5. Returns success status based on generation outcome
   *
   * The generation operation is destructive and permanently modifies the knowledge base
   * by creating new elements. Variables in the template that are not bound by parameters
   * are instantiated as new elements, while bound variables reference existing elements.
   *
   * Unlike SearchTemplate's ApplyImpl which may reuse an existing TemplateResults object,
   * GenerateTemplate always creates a fresh TemplateResults instance to ensure clean
   * state for the newly generated constructions.
   *
   * @param params      [in] Reference to template parameters extracted from the knowledge
   *                         base, containing variable replacements and template-specific
   *                         configuration used for ScTemplate construction and generation.
   *
   * @param arguments   [in] Reference to runtime arguments providing variable bindings
   *                         and constraints for this specific generation operation. Bound
   *                         variables reference existing elements; unbound variables will
   *                         be created as new elements.
   *
   * @param results     [out] Reference to the results container. A new TemplateResults
   *                          object is always created and assigned, populated with template
   *                          configuration addresses and all generated element addresses.
   *
   * @param callbacks   [in] Reference to a list of filter callback functions. Note that
   *                         for generation operations, callbacks are typically not used
   *                         during construction but may be applied during result processing.
   *
   * @return @c true if the generation was executed successfully and new structures were
   *         created in the knowledge base; @c false if generation failed due to errors
   *         during template building or construction execution.
   *
   * @warning This method modifies the knowledge base. Ensure proper validation and
   *          authorization before calling.
   *
   * @see TryGenerateByTemplate
   * @see TemplateResults::CollectFromGenResult
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
   * Constructs a GenerateTemplate instance with the necessary context and configuration
   * for executing generation operations against the knowledge base. This constructor is
   * protected to ensure that GenerateTemplate instances are created only through the
   * builder pattern, maintaining proper initialization and configuration.
   *
   * The constructor delegates to the ParameterizedTemplate base class constructor
   * to establish agent context, logging, and template address references.
   *
   * @param context       [in] Reference to the ScAgentContext providing knowledge base
   *                           access and agent-specific operations for template generation.
   *                           The context must have write permissions to the knowledge base.
   *
   * @param logger        [in] Reference to the ScLogger instance used for logging generation
   *                           operations, debugging information, and error reporting.
   *
   * @param templateAddr  [in] Constant reference to the ScAddr of the template node
   *                           in the knowledge base that defines the generation pattern
   *                           structure and configuration specifying what to create.
   *
   * @see ParameterizedTemplate::ParameterizedTemplate
   * @see ParameterizedTemplateBuilder
   */
  GenerateTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr);

  /*!
   * @brief Builds and executes a template generation, creating new structures in the knowledge base.
   *
   * This helper method encapsulates the low-level template building and generation
   * execution against the sc-memory knowledge base.
   *
   * The method uses the sc-memory template API to perform efficient construction operations
   * that atomically create multiple interconnected elements in the semantic network.
   *
   * @param params      [in] Reference to template parameters containing variable bindings
   *                         and substitutions to apply during template generation. These
   *                         parameters specify which template variables should reference
   *                         existing elements vs. being created as new elements.
   *
   * @param genResult   [out] Reference to a ScTemplateGenResult object that will be
   *                          populated with addresses of all generated elements. The result
   *                          provides a mapping from template variable names to the actual
   *                          sc-memory addresses of created or referenced elements.
   *
   * @return @c true always, as generation operations are considered successful once template
   *         building completes. The GenerateByTemplate API does not report failures through
   *         boolean return values; any errors would be raised as exceptions or logged.
   *
   * @note Unlike TrySearchByTemplate which may return false if no matches are found,
   *       TryGenerateByTemplate always returns true after completing generation, as the
   *       operation always produces a result (the newly created structure).
   *
   * @warning This method creates permanent structures in the knowledge base. There is no
   *          built-in rollback mechanism.
   *
   * @see ScAgentContext::BuildTemplate
   * @see ScAgentContext::GenerateByTemplate
   */
  bool TryGenerateByTemplate(ScTemplateParams const & params, ScTemplateGenResult & genResult) const;
};