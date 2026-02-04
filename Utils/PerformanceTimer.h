#pragma once

#include <cstdint>
#include <chrono>

#ifndef __EMSCRIPTEN__
  #ifdef __APPLE__
    #include <OpenGL/gl.h>
  #else
    #include <GL/gl.h>
  #endif
#endif

/**
 * @file PerformanceTimer.h
 * @brief Simple CPU and GPU timing utilities with a common interface.
 *
 * Declares an abstract @ref PerformanceTimer plus two implementations:
 *  - @ref CPUTimer using std::chrono for wall-clock duration.
 *  - @ref GLTimer using OpenGL timer queries (GL_TIME_ELAPSED) to measure GPU time.
 *
 * @note @ref GLTimer requires OpenGL support for `GL_TIME_ELAPSED` queries
 *       (OpenGL 3.3+ core or ARB_timer_query). Always call @ref begin() and
 *       @ref end() in matched pairs before querying @ref elapsedNanoseconds().
 */
class PerformanceTimer {
public:
  /** @brief Polymorphic base must have a virtual destructor. */
  virtual ~PerformanceTimer() = default;

  /** @brief Start measuring a new interval. */
  virtual void begin() = 0;
  /** @brief End the current interval. */
  virtual void end() = 0;
  /**
   * @brief Elapsed time for the last interval in nanoseconds.
   * @return Duration in nanoseconds. Implementations may return the value for
   *         the most recent completed interval or, if still running, a
   *         best-effort partial value.
   */
  virtual std::uint64_t elapsedNanoseconds() = 0;

  /** @brief Convenience accessor: elapsed time in milliseconds. */
  double elapsedMilliseconds() {
    return static_cast<double>(elapsedNanoseconds()) / 1e6;
  }

  /** @brief Convenience accessor: elapsed time in seconds. */
  double elapsedSeconds() {
    return static_cast<double>(elapsedNanoseconds()) / 1e9;
  }
};

#ifndef __EMSCRIPTEN__
/**
 * @brief GPU timer based on OpenGL time-elapsed queries.
 *
 * Wraps a single `GL_QUERY` object to measure GPU time between @ref begin()
 * and @ref end(). The result can be obtained in either blocking or non-
 * blocking mode via @ref elapsedNanoseconds().
 */
class GLTimer : public PerformanceTimer {
public:
  /**
   * @brief Construct a timer and create an OpenGL query object.
   * @param blocking If true, @ref elapsedNanoseconds() blocks until the query result is available.
   */
  explicit GLTimer(bool blocking = true)
  : blocking(blocking)
  {
    glGenQueries(1, &queryId);
  }

  /** @brief Delete the OpenGL query object. */
  ~GLTimer() override {
    if (queryId != 0) {
      glDeleteQueries(1, &queryId);
    }
  }

  /** @brief Configure whether @ref elapsedNanoseconds() waits for availability. */
  void setBlocking(bool value) {
    blocking = value;
  }

  /**
   * @brief Begin a GPU time interval.
   *
   * Safely ignores nested calls when already running.
   */
  void begin() override {
    if (isRunning) return;
    glBeginQuery(GL_TIME_ELAPSED, queryId);
    isRunning = true;
  }

  /**
   * @brief End the GPU time interval and mark the result as pending.
   *
   * Safely ignores calls when not running.
   */
  void end() override {
    if (!isRunning) return;
    glEndQuery(GL_TIME_ELAPSED);
    isRunning = false;
    hasPending = true;
  }

  /**
   * @brief Retrieve the elapsed GPU time in nanoseconds.
   *
   * If @ref setBlocking(true) was chosen, this call blocks until the result is
   * available. Otherwise it returns the most recent completed value and only
   * updates when the new result becomes available.
   */
  std::uint64_t elapsedNanoseconds() override {
    if (!hasPending) return lastResultNs;

    if (blocking) {
      std::uint64_t ns = 0;
      glGetQueryObjectui64v(queryId, GL_QUERY_RESULT, &ns);
      lastResultNs = ns;
      hasPending = false;
    } else {
      GLint available = 0;
      glGetQueryObjectiv(queryId, GL_QUERY_RESULT_AVAILABLE, &available);
      if (available) {
        std::uint64_t ns = 0;
        glGetQueryObjectui64v(queryId, GL_QUERY_RESULT, &ns);
        lastResultNs = ns;
        hasPending = false;
      }
    }

    return lastResultNs;
  }

private:
  GLuint queryId = 0;       ///< OpenGL query object name.
  bool isRunning = false;   ///< True between @ref begin() and @ref end().
  bool hasPending = false;  ///< True after @ref end() until the result is read.
  bool blocking = true;     ///< If true, queries block until results are ready.
  std::uint64_t lastResultNs = 0; ///< Cached last completed result in nanoseconds.
};


/**
 * @brief CPU wall-clock timer based on std::chrono::steady_clock.
 *
 * Measures elapsed real time between @ref begin() and @ref end() using a
 * monotonic clock. If @ref elapsedNanoseconds() is queried while running, the
 * duration up to "now" is returned without stopping the timer.
 */
class CPUTimer : public PerformanceTimer {
public:
  /** @brief Underlying clock type used by this timer. */
  using clock_type = std::chrono::steady_clock;

  /** @brief Start a new interval. */
  void begin() override {
    startTime = clock_type::now();
    running = true;
    hasResult = false;
  }

  /** @brief End the current interval. */
  void end() override {
    if (!running) return;
    endTime = clock_type::now();
    running = false;
    hasResult = true;
  }

  /**
   * @brief Elapsed CPU time in nanoseconds.
   *
   * If the timer is still running, returns the duration since @ref begin(). If
   * it has been stopped, returns the finalized interval. If no interval has
   * been recorded yet, returns 0.
   */
  std::uint64_t elapsedNanoseconds() override {
    if (!hasResult) return 0;
    auto endPoint = running ? clock_type::now() : endTime;
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(endPoint - startTime).count();
    return static_cast<std::uint64_t>(ns);
  }

private:
  clock_type::time_point startTime{}; ///< Start timestamp of the interval.
  clock_type::time_point endTime{};   ///< End timestamp (valid after @ref end()).
  bool running = false;               ///< True between @ref begin() and @ref end().
  bool hasResult = false;             ///< True once a finished interval is available.
};


#else

#include <cstdint>
#include <limits>
#include <emscripten/emscripten.h>

class CPUTimer : public PerformanceTimer {
public:
  void begin() override {
    startMs = emscripten_get_now();   // milliseconds (double), high-resolution
    running = true;
    hasResult = false;
  }

  void end() override {
    if (!running) return;
    endMs = emscripten_get_now();
    running = false;
    hasResult = true;
  }

  std::uint64_t elapsedNanoseconds() override {
    if (!hasResult) return 0;

    const double curMs = running ? emscripten_get_now() : endMs;
    double deltaMs = curMs - startMs;
    if (deltaMs < 0.0) deltaMs = 0.0;

    const double ns = deltaMs * 1e6; // ms -> ns

    if (ns <= 0.0) return 0;
    if (ns > static_cast<double>(std::numeric_limits<std::uint64_t>::max()))
      return std::numeric_limits<std::uint64_t>::max();

    return static_cast<std::uint64_t>(ns);
  }

private:
  double startMs = 0.0;
  double endMs   = 0.0;
  bool running   = false;
  bool hasResult = false;
};

#endif
