#pragma once

/*!
 * @file wait_template.hpp
 * @brief Time-bounded polling wrapper for SearchTemplate with configurable timeout.
 *
 * This module provides the WaitTemplate class, which implements time-bounded polling
 * search operations against the sc-memory knowledge base. It extends SearchTemplate to
 * repeatedly attempt pattern matching until either the pattern is found or a timeout
 * expires, making it suitable for asynchronous workflows where knowledge base elements
 * may appear after some delay.
 *
 * WaitTemplate is designed for scenarios where the expected pattern may not immediately
 * exist in the knowledge base but is anticipated to appear through concurrent agent
 * actions, event processing, or external data ingestion. Rather than immediately failing
 * when a pattern is not found, WaitTemplate polls the knowledge base at regular intervals
 * until the timeout is reached.
 *
 * @see search_template.hpp
 * @see SearchTemplate
 * @see ParameterizedTemplate
 * @see ScAgentContext
 */

#include <chrono>

#include "search_template.hpp"

class ScAgentContext;

/*!
 * @class WaitTemplate
 * @brief Polling search template with timeout for waiting on knowledge base changes.
 *
 * WaitTemplate provides a specialized implementation of SearchTemplate that adds temporal
 * semantics to pattern matching. Instead of performing a single search operation, it
 * repeatedly polls the knowledge base until the pattern appears or a timeout expires.
 * This makes it suitable for synchronization and coordination tasks in multi-agent systems
 * where patterns may appear asynchronously.
 *
 * The class implements a simple but effective polling loop:
 * 1. Retrieve wait timeout from knowledge base (or use default)
 * 2. Repeatedly search for the pattern at fixed intervals
 * 3. Return success immediately when pattern is found
 * 4. Return failure if timeout expires without finding the pattern
 *
 * @inherits SearchTemplate
 * @see ParameterizedTemplateBuilder
 * @see SearchTemplate
 * @see ParameterizedTemplate::m_waitTimeMsAddr
 *
 * @thread_safety Not thread-safe; blocks the calling thread during wait period.
 * @warning Avoid using WaitTemplate with very large timeouts as it blocks the calling thread.
 */
class WaitTemplate : public SearchTemplate
{
  friend class ParameterizedTemplateBuilder;

public:
  /*!
   * @brief Destructor for WaitTemplate.
   *
   * Cleans up resources allocated by the WaitTemplate instance. Since this class
   * uses standard library timing facilities and delegates search logic to SearchTemplate,
   * the destructor performs minimal cleanup beyond base class destruction.
   */
  ~WaitTemplate() override;

protected:
  /*!
   * @brief Default polling interval for wait template pattern checking.
   *
   * This constant defines the sleep duration between successive pattern search attempts
   * during the polling loop. The value of 200 milliseconds provides a balance between:
   * - detecting pattern appearance within a reasonable timeframe
   * - avoiding excessive knowledge base query load
   *
   * This interval is also used as the default timeout duration when no timeout is
   * configured in the knowledge base via m_waitTimeMsAddr.
   *
   * The interval is fixed at compile time and cannot be customized at runtime. To change
   * polling behavior, the timeout duration (via m_waitTimeMsAddr) should be adjusted
   * rather than this constant.
   */
  static constexpr std::chrono::milliseconds DEFAULT_WAIT_TEMPLATE_INTERVAL_MS{200};

  /*!
   * @brief Executes time-bounded polling search until pattern found or timeout expires.
   *
   * This method implements the core polling logic for WaitTemplate. It performs repeated
   * search attempts at fixed intervals until either the pattern is successfully found or
   * the configured timeout expires.
   *
   * @param params      [in] Reference to template parameters extracted from the knowledge
   *                         base, passed through to SearchTemplate::ApplyImpl for pattern
   *                         construction and matching during each polling attempt.
   *
   * @param arguments   [in] Reference to runtime arguments providing variable bindings
   *                         for the search operation, passed through to SearchTemplate
   *                         on each polling iteration.
   *
   * @param results     [in,out] Reference to the results container. If the pattern is found
   *                             before timeout, this is populated by SearchTemplate::ApplyImpl
   *                             with all matched constructions. If timeout expires, the
   *                             container remains unmodified (or in its last search state).
   *
   * @param callbacks   [in] Reference to filter callback functions passed through to
   *                         SearchTemplate::ApplyImpl for constraint application during
   *                         each search attempt.
   *
   * @return @c true if the pattern was found within the timeout period (search succeeded
   *         before or at timeout expiration); @c false if the timeout expired without
   *         finding the pattern.
   *
   * @note The results container is only reliably populated when this method returns true.
   *       On timeout (returns false), results may be empty or contain partial data from
   *       the last search attempt.
   *
   * @warning This method blocks the calling thread. Avoid using in contexts where blocking
   *          is unacceptable (event loops, UI threads, real-time systems).
   *
   * @see SearchTemplate::ApplyImpl
   * @see ParameterizedTemplate::m_waitTimeMsAddr
   * @see DEFAULT_WAIT_TEMPLATE_INTERVAL_MS
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
   * Constructs a WaitTemplate instance by delegating to the SearchTemplate base class
   * constructor. This constructor is protected to ensure that WaitTemplate instances
   * are created only through the builder pattern, maintaining proper initialization
   * and configuration.
   *
   * The constructor establishes the same context, logging, and template address
   * configuration as SearchTemplate. The polling and timeout behavior is implemented
   * entirely in the ApplyImpl method override, requiring no additional member variables
   * or initialization beyond the base class.
   *
   * @param context       [in] Reference to the ScAgentContext providing knowledge base
   *                           access and agent-specific operations for repeated template
   *                           searches during the polling period.
   *
   * @param logger        [in] Reference to the ScLogger instance used for logging wait
   *                           operations, timeout events, polling attempts, and search
   *                           results.
   *
   * @param templateAddr  [in] Constant reference to the ScAddr of the template node
   *                           in the knowledge base that defines the search pattern
   *                           to wait for. The template should also have associated
   *                           m_waitTimeMsAddr configuration specifying the timeout.
   *
   * @see SearchTemplate::SearchTemplate
   * @see ParameterizedTemplateBuilder
   */
  WaitTemplate(ScAgentContext & context, utils::ScLogger & logger, ScAddr const & templateAddr);
};
