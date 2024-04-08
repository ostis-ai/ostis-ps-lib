#pragma once

#if PERF_MEASURE_BUILD

#  include <chrono>
#  include <sc-memory/utils/sc_log.hpp>

#  define START_TIMER() auto perf_start_time = std::chrono::high_resolution_clock::now()

#  define STOP_TIMER(name) \
    auto perf_stop_time = std::chrono::high_resolution_clock::now(); \
    SC_LOG_DEBUG( \
        name << ": " \
             << std::chrono::duration_cast<std::chrono::milliseconds>(perf_stop_time - perf_start_time).count() \
             << "ms")

#else

#  define START_TIMER()
#  define STOP_TIMER(name)

#endif
