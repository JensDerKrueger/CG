#pragma once

#include <cstdint>
#include <optional>
#include <utility>

#ifndef __EMSCRIPTEN__
#include <chrono>
#include <future>
#endif

template <typename Result>
class BackgroundTask {
public:
	BackgroundTask() = default;
	BackgroundTask(const BackgroundTask&) = delete;
	BackgroundTask& operator=(const BackgroundTask&) = delete;

	void request() {
		++requestGeneration;
		requestPending = true;
		resultReady = false;
		result.reset();
	}

	void cancel() {
		++requestGeneration;
		requestPending = false;
		resultReady = false;
		result.reset();
	}

	bool canStart() {
		updateFinishedTask();
		return requestPending && !isTaskRunning();
	}

	template <typename Work>
	bool start(Work&& work) {
		updateFinishedTask();
		if (!requestPending || isTaskRunning())
			return false;

		const uint64_t generation = requestGeneration;
		requestPending = false;

#ifdef __EMSCRIPTEN__
		Result value = work();
		if (generation == requestGeneration) {
			result = std::move(value);
			resultReady = true;
		}
#else
		task = std::async(std::launch::async,
		                  [generation, work = std::forward<Work>(work)]() mutable {
			                  return TaskResult{generation, work()};
		                  });
#endif

		return true;
	}

	bool takeResult(Result& destination) {
		updateFinishedTask();
		if (!resultReady || !result)
			return false;

		destination = std::move(*result);
		result.reset();
		resultReady = false;
		return true;
	}

	bool hasPendingRequest() {
		updateFinishedTask();
		return requestPending;
	}

	bool isRunning() {
		updateFinishedTask();
		return isTaskRunning();
	}

private:
	struct TaskResult {
		uint64_t generation{0};
		Result result;
	};

	uint64_t requestGeneration{0};
	bool requestPending{false};
	bool resultReady{false};
	std::optional<Result> result;

#ifndef __EMSCRIPTEN__
	std::future<TaskResult> task;
#endif

	bool isTaskRunning() const {
#ifdef __EMSCRIPTEN__
		return false;
#else
		return task.valid();
#endif
	}

	void updateFinishedTask() {
#ifndef __EMSCRIPTEN__
		if (!task.valid())
			return;

		if (task.wait_for(std::chrono::seconds{0}) != std::future_status::ready)
			return;

		TaskResult taskResult = task.get();
		if (taskResult.generation == requestGeneration) {
			result = std::move(taskResult.result);
			resultReady = true;
		}
#endif
	}
};

/*
 Copyright (c) 2026 Computer Graphics and Visualization Group, University of
 Duisburg-Essen

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in the
 Software without restriction, including without limitation the rights to use, copy,
 modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 to permit persons to whom the Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be included in all copies
 or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
