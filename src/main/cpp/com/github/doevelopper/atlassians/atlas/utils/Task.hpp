/*******************************************************************
* @file
* @version  v0.0.
* @date     02/07/26 F 00:00
* @brief    Runnable and schedulable task utilities
*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_UTILS_TASK_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_UTILS_TASK_HPP

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

#include <chrono>
#include <cstddef>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>

namespace com::github::doevelopper::atlassians::atlas::utils
{

/**
 * @brief Runnable interface
 */
class IRunnable
{
public:
    virtual ~IRunnable() = default;

    /**
     * @brief Execute the runnable action
     */
    virtual void run() = 0;

protected:
    IRunnable() = default;
};

/**
 * @brief Schedulable interface
 */
class ISchedulable
{
public:
    virtual ~ISchedulable() = default;

    /**
     * @brief Schedule the task for execution
     */
    virtual void schedule() = 0;

    /**
     * @brief Cancel the task
     */
    virtual void cancel() = 0;

protected:
    ISchedulable() = default;
};

/**
 * @brief Concrete task implementation
 *
 * A minimal task abstraction that can be scheduled, cancelled, and executed.
 * This class is thread-safe for state transitions and status queries.
 */
class Task final : public IRunnable, public ISchedulable
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    using Action = std::function<void()>;

    enum class State : std::uint8_t
    {
        Idle,
        Scheduled,
        Running,
        Completed,
        Cancelled,
        Failed
    };

    struct Status
    {
        State state{State::Idle};
        std::size_t runCount{0};
        std::optional<std::string> lastError{std::nullopt};
        std::chrono::steady_clock::time_point lastStarted{};
        std::chrono::steady_clock::time_point lastFinished{};
    };

    /**
     * @brief Construct a task with an action and optional name
     */
    explicit Task(Action action, std::string_view name = "Task");

    /**
     * @brief Destructor
     */
    ~Task() noexcept override;

    // IRunnable
    void run() override;

    // ISchedulable
    void schedule() override;
    void cancel() override;

    // ============================================
    // Accessors
    // ============================================

    [[nodiscard]] auto getName() const noexcept -> std::string_view;
    auto setName(std::string_view name) -> void;

    [[nodiscard]] auto getState() const noexcept -> State;
    [[nodiscard]] auto isScheduled() const noexcept -> bool;
    [[nodiscard]] auto isCancelled() const noexcept -> bool;
    [[nodiscard]] auto isRunning() const noexcept -> bool;
    [[nodiscard]] auto hasCompleted() const noexcept -> bool;

    [[nodiscard]] auto getRunCount() const noexcept -> std::size_t;
    [[nodiscard]] auto getLastError() const noexcept -> std::optional<std::string>;
    [[nodiscard]] auto getStatus() const noexcept -> Status;

    // ============================================
    // Deleted Copy (Task is stateful)
    // ============================================

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&&) noexcept = default;
    Task& operator=(Task&&) noexcept = default;

private:
    mutable std::mutex m_mutex{};
    std::string m_name{};
    Action m_action{};
    State m_state{State::Idle};
    std::size_t m_runCount{0};
    std::optional<std::string> m_lastError{std::nullopt};
    std::chrono::steady_clock::time_point m_lastStarted{};
    std::chrono::steady_clock::time_point m_lastFinished{};
};

} // namespace com::github::doevelopper::atlassians::atlas::utils

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_UTILS_TASK_HPP
