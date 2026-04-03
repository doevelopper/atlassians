/**
 * @file ErrorReporting.hpp
 * @brief Error reporting and publishing mechanisms
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 *
 * Provides comprehensive reporting mechanisms:
 * - Detailed error logging with context information
 * - Error publishing via Boost.Signals2
 * - Integration with monitoring systems
 * - Structured error data for analysis
 * - Error aggregation to prevent flooding
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORREPORTING_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORREPORTING_HPP

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <deque>

#include <boost/signals2.hpp>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>
#include <com/github/doevelopper/atlassians/triada/Error.hpp>

namespace com::github::doevelopper::atlassians::triada
{

/**
 * @brief Error report with additional metadata
 */
class ErrorReport
{
public:
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;

    /**
     * @brief Report priority for ordering
     */
    enum class Priority
    {
        Low = 0,
        Normal = 1,
        High = 2,
        Critical = 3,
        Emergency = 4
    };

    /**
     * @brief Default constructor
     */
    ErrorReport() noexcept;

    /**
     * @brief Construct from Error
     */
    explicit ErrorReport(Error error);

    /**
     * @brief Construct with all parameters
     */
    ErrorReport(
        Error error,
        Priority priority,
        std::string_view component = {},
        std::string_view correlationId = {});

    ~ErrorReport() noexcept = default;
    ErrorReport(const ErrorReport&) = default;
    ErrorReport& operator=(const ErrorReport&) = default;
    ErrorReport(ErrorReport&&) noexcept = default;
    ErrorReport& operator=(ErrorReport&&) noexcept = default;

    // ============================================
    // Accessors
    // ============================================

    [[nodiscard]] auto error() const noexcept -> const Error&;
    [[nodiscard]] auto priority() const noexcept -> Priority;
    [[nodiscard]] auto reportId() const noexcept -> std::uint64_t;
    [[nodiscard]] auto timestamp() const noexcept -> TimePoint;
    [[nodiscard]] auto component() const noexcept -> std::string_view;
    [[nodiscard]] auto correlationId() const noexcept -> std::string_view;
    [[nodiscard]] auto occurrenceCount() const noexcept -> std::size_t;
    [[nodiscard]] auto tags() const noexcept -> const std::vector<std::string>&;
    [[nodiscard]] auto metadata() const noexcept -> const std::unordered_map<std::string, std::string>&;

    // ============================================
    // Modifiers
    // ============================================

    auto withPriority(Priority p) -> ErrorReport&;
    auto withComponent(std::string_view comp) -> ErrorReport&;
    auto withCorrelationId(std::string_view id) -> ErrorReport&;
    auto withTag(std::string_view tag) -> ErrorReport&;
    auto withMetadata(std::string_view key, std::string_view value) -> ErrorReport&;
    auto incrementOccurrence() -> ErrorReport&;

    // ============================================
    // Formatting
    // ============================================

    [[nodiscard]] auto toString() const -> std::string;
    [[nodiscard]] auto toJson() const -> std::string;

    // ============================================
    // Comparison for deduplication
    // ============================================

    [[nodiscard]] auto fingerprint() const -> std::size_t;
    [[nodiscard]] auto isSimilarTo(const ErrorReport& other) const -> bool;

public:
    TimePoint lastSeen;  ///< Last time this error was seen (for aggregation)

private:
    Error m_error;
    Priority m_priority{Priority::Normal};
    std::uint64_t m_reportId{0};
    TimePoint m_timestamp;
    std::string m_component;
    std::string m_correlationId;
    std::size_t m_occurrenceCount{1};
    std::vector<std::string> m_tags;
    std::unordered_map<std::string, std::string> m_metadata;

    static std::atomic<std::uint64_t> s_nextReportId;
};

/**
 * @brief Error subscriber interface
 */
class IErrorSubscriber
{
public:
    virtual ~IErrorSubscriber() = default;
    virtual auto onError(const ErrorReport& report) -> void = 0;
    virtual auto name() const -> std::string_view = 0;
};

/**
 * @brief Error aggregation configuration
 */
struct AggregationConfig
{
    std::chrono::milliseconds windowDuration{5000};  ///< Time window for aggregation
    std::size_t maxSimilarErrors{100};               ///< Max similar errors to track
    std::size_t maxErrorsPerWindow{1000};            ///< Max errors per time window
    bool enableDeduplication{true};                   ///< Enable duplicate suppression
    bool enableRateLimiting{true};                    ///< Enable rate limiting
    double similarityThreshold{0.9};                  ///< Threshold for similarity (0-1)
};

/**
 * @brief Error aggregator to prevent error flooding
 *
 * Implements mechanisms to prevent error flooding:
 * - Duplicate error suppression
 * - Error rate limiting
 * - Similarity-based grouping
 * - Periodic summary reports
 */
class ErrorAggregator
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using SummaryCallback = std::function<void(const std::vector<ErrorReport>&)>;

    explicit ErrorAggregator(AggregationConfig config = {});
    ~ErrorAggregator() noexcept;

    ErrorAggregator(const ErrorAggregator&) = delete;
    ErrorAggregator& operator=(const ErrorAggregator&) = delete;
    ErrorAggregator(ErrorAggregator&&) = delete;
    ErrorAggregator& operator=(ErrorAggregator&&) = delete;

    /**
     * @brief Process an error report
     * @return true if report should be published, false if suppressed
     */
    [[nodiscard]] auto process(ErrorReport& report) -> bool;

    /**
     * @brief Get aggregated summary
     */
    [[nodiscard]] auto getSummary() const -> std::vector<ErrorReport>;

    /**
     * @brief Set callback for periodic summaries
     */
    auto setSummaryCallback(SummaryCallback callback) -> void;

    /**
     * @brief Clear aggregation state
     */
    auto reset() -> void;

    /**
     * @brief Get statistics
     */
    [[nodiscard]] auto totalReceived() const noexcept -> std::size_t;
    [[nodiscard]] auto totalSuppressed() const noexcept -> std::size_t;
    [[nodiscard]] auto totalPublished() const noexcept -> std::size_t;

private:
    auto cleanupOldEntries() -> void;
    auto findSimilar(const ErrorReport& report) -> ErrorReport*;

    struct AggregatedError
    {
        ErrorReport report;
        TimePoint firstSeen;
        TimePoint lastSeen;
        std::size_t count{1};
    };

    AggregationConfig m_config;
    std::deque<AggregatedError> m_aggregatedErrors;
    std::size_t m_windowErrorCount{0};
    TimePoint m_windowStart;

    std::atomic<std::size_t> m_totalReceived{0};
    std::atomic<std::size_t> m_totalSuppressed{0};
    std::atomic<std::size_t> m_totalPublished{0};

    SummaryCallback m_summaryCallback;
    mutable std::mutex m_mutex;
};

/**
 * @brief Central error reporter with signal-based publishing
 *
 * Provides:
 * - Multi-subscriber error notification via Boost.Signals2
 * - Error aggregation and deduplication
 * - Priority-based handling
 * - Integration with logging framework
 */
class ErrorReporter
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    using ErrorSignal = boost::signals2::signal<void(const ErrorReport&)>;
    using Connection = boost::signals2::connection;
    using ScopedConnection = boost::signals2::scoped_connection;

    // Singleton access
    static auto instance() -> ErrorReporter&;

    // Non-copyable, non-movable
    ErrorReporter(const ErrorReporter&) = delete;
    ErrorReporter& operator=(const ErrorReporter&) = delete;

    // ============================================
    // Configuration
    // ============================================

    /**
     * @brief Configure aggregation
     */
    auto configure(AggregationConfig config) -> void;

    /**
     * @brief Enable/disable reporting
     */
    auto setEnabled(bool enabled) -> void;

    /**
     * @brief Check if enabled
     */
    [[nodiscard]] auto isEnabled() const noexcept -> bool;

    // ============================================
    // Reporting Methods
    // ============================================

    /**
     * @brief Report an error
     */
    auto report(const Error& error) -> void;

    /**
     * @brief Report with additional context
     */
    auto report(ErrorReport report) -> void;

    /**
     * @brief Report and log an error
     */
    auto reportAndLog(const Error& error) -> void;

    /**
     * @brief Create report builder
     */
    [[nodiscard]] auto createReport(const Error& error) -> ErrorReport;

    // ============================================
    // Subscription
    // ============================================

    /**
     * @brief Subscribe to all errors
     */
    [[nodiscard]] auto subscribe(ErrorSignal::slot_type slot) -> Connection;

    /**
     * @brief Subscribe with automatic disconnection
     */
    [[nodiscard]] auto subscribeScoped(ErrorSignal::slot_type slot) -> ScopedConnection;

    /**
     * @brief Subscribe to errors of specific severity or higher
     */
    [[nodiscard]] auto subscribeToSeverity(
        ErrorSeverity minSeverity,
        ErrorSignal::slot_type slot) -> Connection;

    /**
     * @brief Subscribe to errors from specific service
     */
    [[nodiscard]] auto subscribeToService(
        ServiceId service,
        ErrorSignal::slot_type slot) -> Connection;

    /**
     * @brief Add a subscriber object
     */
    [[nodiscard]] auto addSubscriber(std::shared_ptr<IErrorSubscriber> subscriber) -> Connection;

    // ============================================
    // Statistics
    // ============================================

    [[nodiscard]] auto totalReported() const noexcept -> std::size_t;
    [[nodiscard]] auto totalSuppressed() const noexcept -> std::size_t;
    [[nodiscard]] auto totalPublished() const noexcept -> std::size_t;

    /**
     * @brief Reset statistics
     */
    auto resetStatistics() -> void;

private:
    ErrorReporter();
    ~ErrorReporter() noexcept;

    auto publish(const ErrorReport& report) -> void;

    std::atomic<bool> m_enabled{true};
    ErrorSignal m_errorSignal;
    std::unique_ptr<ErrorAggregator> m_aggregator;
    std::atomic<std::size_t> m_totalReported{0};
    mutable std::mutex m_mutex;
};

/**
 * @brief Logging subscriber that logs errors via Log4CXX
 */
class LoggingErrorSubscriber : public IErrorSubscriber
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    LoggingErrorSubscriber();
    ~LoggingErrorSubscriber() override = default;

    auto onError(const ErrorReport& report) -> void override;
    [[nodiscard]] auto name() const -> std::string_view override;

private:
    auto logAtLevel(const ErrorReport& report) -> void;
};

/**
 * @brief Console subscriber for development/debugging
 */
class ConsoleErrorSubscriber : public IErrorSubscriber
{
public:
    enum class Format
    {
        Simple,
        Detailed,
        Json
    };

    explicit ConsoleErrorSubscriber(Format format = Format::Simple);
    ~ConsoleErrorSubscriber() override = default;

    auto onError(const ErrorReport& report) -> void override;
    [[nodiscard]] auto name() const -> std::string_view override;

private:
    Format m_format;
};

/**
 * @brief Metrics collector subscriber
 */
class MetricsErrorSubscriber : public IErrorSubscriber
{
public:
    struct Metrics
    {
        std::atomic<std::size_t> totalErrors{0};
        std::atomic<std::size_t> fatalErrors{0};
        std::atomic<std::size_t> criticalErrors{0};
        std::atomic<std::size_t> recoverableErrors{0};
        std::atomic<std::size_t> warnings{0};
        std::unordered_map<std::string, std::atomic<std::size_t>> byService;
        std::unordered_map<std::string, std::atomic<std::size_t>> byComponent;
    };

    MetricsErrorSubscriber();
    ~MetricsErrorSubscriber() override = default;

    auto onError(const ErrorReport& report) -> void override;
    [[nodiscard]] auto name() const -> std::string_view override;

    [[nodiscard]] auto metrics() const noexcept -> const Metrics&;
    auto reset() -> void;

private:
    Metrics m_metrics;
    mutable std::mutex m_mutex;
};

} // namespace com::github::doevelopper::atlassians::triada

// ============================================
// Convenience Macros
// ============================================

/**
 * @brief Report an error
 */
#define TRIADA_REPORT_ERROR(err) \
    ::com::github::doevelopper::atlassians::triada::ErrorReporter::instance().report(err)

/**
 * @brief Report and log an error
 */
#define TRIADA_REPORT_AND_LOG(err) \
    ::com::github::doevelopper::atlassians::triada::ErrorReporter::instance().reportAndLog(err)

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORREPORTING_HPP
