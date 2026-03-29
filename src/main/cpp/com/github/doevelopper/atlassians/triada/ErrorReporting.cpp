/**
 * @file ErrorReporting.cpp
 * @brief Implementation of error reporting mechanisms
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/triada/ErrorReporting.hpp>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace com::github::doevelopper::atlassians::triada;

// Static member initialization
std::atomic<std::uint64_t> ErrorReport::s_nextReportId{1};

log4cxx::LoggerPtr ErrorAggregator::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.ErrorAggregator"));

log4cxx::LoggerPtr ErrorReporter::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.ErrorReporter"));

log4cxx::LoggerPtr LoggingErrorSubscriber::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.LoggingErrorSubscriber"));

// ============================================
// ErrorReport Implementation
// ============================================

ErrorReport::ErrorReport() noexcept
    : m_reportId{s_nextReportId.fetch_add(1, std::memory_order_relaxed)}
    , m_timestamp{Clock::now()}
{
}

ErrorReport::ErrorReport(Error error)
    : m_error{std::move(error)}
    , m_reportId{s_nextReportId.fetch_add(1, std::memory_order_relaxed)}
    , m_timestamp{Clock::now()}
{
    // Map error severity to priority
    switch (m_error.severity())
    {
        case ErrorSeverity::Fatal:
            m_priority = Priority::Emergency;
            break;
        case ErrorSeverity::Critical:
            m_priority = Priority::Critical;
            break;
        case ErrorSeverity::Error:
        case ErrorSeverity::Recoverable:
            m_priority = Priority::High;
            break;
        case ErrorSeverity::Warning:
            m_priority = Priority::Normal;
            break;
        default:
            m_priority = Priority::Low;
            break;
    }
}

ErrorReport::ErrorReport(
    Error error,
    Priority priority,
    std::string_view component,
    std::string_view correlationId)
    : m_error{std::move(error)}
    , m_priority{priority}
    , m_reportId{s_nextReportId.fetch_add(1, std::memory_order_relaxed)}
    , m_timestamp{Clock::now()}
    , m_component{component}
    , m_correlationId{correlationId}
{
}

auto ErrorReport::error() const noexcept -> const Error&
{
    return m_error;
}

auto ErrorReport::priority() const noexcept -> Priority
{
    return m_priority;
}

auto ErrorReport::reportId() const noexcept -> std::uint64_t
{
    return m_reportId;
}

auto ErrorReport::timestamp() const noexcept -> TimePoint
{
    return m_timestamp;
}

auto ErrorReport::component() const noexcept -> std::string_view
{
    return m_component;
}

auto ErrorReport::correlationId() const noexcept -> std::string_view
{
    return m_correlationId;
}

auto ErrorReport::occurrenceCount() const noexcept -> std::size_t
{
    return m_occurrenceCount;
}

auto ErrorReport::tags() const noexcept -> const std::vector<std::string>&
{
    return m_tags;
}

auto ErrorReport::metadata() const noexcept -> const std::unordered_map<std::string, std::string>&
{
    return m_metadata;
}

auto ErrorReport::withPriority(Priority p) -> ErrorReport&
{
    m_priority = p;
    return *this;
}

auto ErrorReport::withComponent(std::string_view comp) -> ErrorReport&
{
    m_component = comp;
    return *this;
}

auto ErrorReport::withCorrelationId(std::string_view id) -> ErrorReport&
{
    m_correlationId = id;
    return *this;
}

auto ErrorReport::withTag(std::string_view tag) -> ErrorReport&
{
    m_tags.emplace_back(tag);
    return *this;
}

auto ErrorReport::withMetadata(std::string_view key, std::string_view value) -> ErrorReport&
{
    m_metadata[std::string{key}] = std::string{value};
    return *this;
}

auto ErrorReport::incrementOccurrence() -> ErrorReport&
{
    ++m_occurrenceCount;
    return *this;
}

auto ErrorReport::toString() const -> std::string
{
    std::ostringstream oss;

    // Format timestamp
    auto timeT = Clock::to_time_t(m_timestamp);
    std::tm tm{};
    gmtime_r(&timeT, &tm);

    oss << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] "
        << "[" << static_cast<int>(m_priority) << "] ";

    if (!m_component.empty())
    {
        oss << "[" << m_component << "] ";
    }

    oss << m_error.toString();

    if (m_occurrenceCount > 1)
    {
        oss << " (x" << m_occurrenceCount << ")";
    }

    return oss.str();
}

auto ErrorReport::toJson() const -> std::string
{
    std::ostringstream oss;

    auto timeT = Clock::to_time_t(m_timestamp);
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        m_timestamp.time_since_epoch()) % 1000000;
    std::tm tm{};
    gmtime_r(&timeT, &tm);

    oss << "{\n"
        << "  \"reportId\": " << m_reportId << ",\n"
        << "  \"timestamp\": \"" << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
        << "." << std::setfill('0') << std::setw(6) << microseconds.count() << "Z\",\n"
        << "  \"priority\": " << static_cast<int>(m_priority) << ",\n";

    if (!m_component.empty())
    {
        oss << "  \"component\": \"" << m_component << "\",\n";
    }

    if (!m_correlationId.empty())
    {
        oss << "  \"correlationId\": \"" << m_correlationId << "\",\n";
    }

    oss << "  \"occurrenceCount\": " << m_occurrenceCount << ",\n";

    if (!m_tags.empty())
    {
        oss << "  \"tags\": [";
        for (std::size_t i = 0; i < m_tags.size(); ++i)
        {
            if (i > 0) oss << ", ";
            oss << "\"" << m_tags[i] << "\"";
        }
        oss << "],\n";
    }

    if (!m_metadata.empty())
    {
        oss << "  \"metadata\": {\n";
        bool first = true;
        for (const auto& [key, value] : m_metadata)
        {
            if (!first) oss << ",\n";
            first = false;
            oss << "    \"" << key << "\": \"" << value << "\"";
        }
        oss << "\n  },\n";
    }

    oss << "  \"error\": " << m_error.toJson() << "\n"
        << "}";

    return oss.str();
}

auto ErrorReport::fingerprint() const -> std::size_t
{
    std::size_t hash = 0;

    // Combine error code
    hash ^= m_error.code().hash() + 0x9e3779b9 + (hash << 6) + (hash >> 2);

    // Combine service
    hash ^= std::hash<int>{}(static_cast<int>(m_error.code().service())) +
            0x9e3779b9 + (hash << 6) + (hash >> 2);

    // Combine component
    hash ^= std::hash<std::string>{}(m_component) +
            0x9e3779b9 + (hash << 6) + (hash >> 2);

    return hash;
}

auto ErrorReport::isSimilarTo(const ErrorReport& other) const -> bool
{
    // Same error code and service
    if (m_error.code().value() != other.m_error.code().value())
    {
        return false;
    }

    // Same component
    if (m_component != other.m_component)
    {
        return false;
    }

    return true;
}

// ============================================
// ErrorAggregator Implementation
// ============================================

ErrorAggregator::ErrorAggregator(AggregationConfig config)
    : m_config{std::move(config)}
    , m_windowStart{Clock::now()}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ErrorAggregator::~ErrorAggregator() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto ErrorAggregator::process(ErrorReport& report) -> bool
{
    std::lock_guard lock{m_mutex};

    m_totalReceived.fetch_add(1, std::memory_order_relaxed);

    // Cleanup old entries first
    cleanupOldEntries();

    // Check rate limiting
    if (m_config.enableRateLimiting)
    {
        if (m_windowErrorCount >= m_config.maxErrorsPerWindow)
        {
            m_totalSuppressed.fetch_add(1, std::memory_order_relaxed);
            return false;
        }
    }

    // Check deduplication
    if (m_config.enableDeduplication)
    {
        if (auto* existing = findSimilar(report))
        {
            existing->incrementOccurrence();
            existing->lastSeen = ErrorReport::Clock::now();
            m_totalSuppressed.fetch_add(1, std::memory_order_relaxed);
            return false;
        }
    }

    // Add to aggregated errors
    AggregatedError aggError;
    aggError.report = report;
    aggError.firstSeen = Clock::now();
    aggError.lastSeen = aggError.firstSeen;
    aggError.count = 1;

    m_aggregatedErrors.push_back(std::move(aggError));
    ++m_windowErrorCount;

    // Trim if too many
    while (m_aggregatedErrors.size() > m_config.maxSimilarErrors)
    {
        m_aggregatedErrors.pop_front();
    }

    m_totalPublished.fetch_add(1, std::memory_order_relaxed);
    return true;
}

auto ErrorAggregator::getSummary() const -> std::vector<ErrorReport>
{
    std::lock_guard lock{m_mutex};

    std::vector<ErrorReport> summary;
    summary.reserve(m_aggregatedErrors.size());

    for (const auto& agg : m_aggregatedErrors)
    {
        summary.push_back(agg.report);
    }

    return summary;
}

auto ErrorAggregator::setSummaryCallback(SummaryCallback callback) -> void
{
    std::lock_guard lock{m_mutex};
    m_summaryCallback = std::move(callback);
}

auto ErrorAggregator::reset() -> void
{
    std::lock_guard lock{m_mutex};
    m_aggregatedErrors.clear();
    m_windowErrorCount = 0;
    m_windowStart = Clock::now();
}

auto ErrorAggregator::totalReceived() const noexcept -> std::size_t
{
    return m_totalReceived.load(std::memory_order_relaxed);
}

auto ErrorAggregator::totalSuppressed() const noexcept -> std::size_t
{
    return m_totalSuppressed.load(std::memory_order_relaxed);
}

auto ErrorAggregator::totalPublished() const noexcept -> std::size_t
{
    return m_totalPublished.load(std::memory_order_relaxed);
}

auto ErrorAggregator::cleanupOldEntries() -> void
{
    auto now = Clock::now();
    auto windowExpiry = now - m_config.windowDuration;

    // Check if window has expired
    if (now - m_windowStart >= m_config.windowDuration)
    {
        // Generate summary if callback set
        if (m_summaryCallback && !m_aggregatedErrors.empty())
        {
            auto summary = getSummary();
            m_summaryCallback(summary);
        }

        // Reset window
        m_windowStart = now;
        m_windowErrorCount = 0;
    }

    // Remove expired entries
    while (!m_aggregatedErrors.empty() &&
           m_aggregatedErrors.front().lastSeen < windowExpiry)
    {
        m_aggregatedErrors.pop_front();
    }
}

auto ErrorAggregator::findSimilar(const ErrorReport& report) -> ErrorReport*
{
    for (auto& agg : m_aggregatedErrors)
    {
        if (agg.report.isSimilarTo(report))
        {
            return &agg.report;
        }
    }
    return nullptr;
}

// ============================================
// ErrorReporter Implementation
// ============================================

ErrorReporter::ErrorReporter()
    : m_aggregator{std::make_unique<ErrorAggregator>()}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ErrorReporter::~ErrorReporter() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto ErrorReporter::instance() -> ErrorReporter&
{
    static ErrorReporter instance;
    return instance;
}

auto ErrorReporter::configure(AggregationConfig config) -> void
{
    std::lock_guard lock{m_mutex};
    m_aggregator = std::make_unique<ErrorAggregator>(std::move(config));
}

auto ErrorReporter::setEnabled(bool enabled) -> void
{
    m_enabled.store(enabled, std::memory_order_release);
}

auto ErrorReporter::isEnabled() const noexcept -> bool
{
    return m_enabled.load(std::memory_order_acquire);
}

auto ErrorReporter::report(const Error& error) -> void
{
    if (!isEnabled())
    {
        return;
    }

    ErrorReport rep{error};
    report(std::move(rep));
}

auto ErrorReporter::report(ErrorReport report) -> void
{
    if (!isEnabled())
    {
        return;
    }

    m_totalReported.fetch_add(1, std::memory_order_relaxed);

    // Process through aggregator
    bool shouldPublish = true;
    {
        std::lock_guard lock{m_mutex};
        if (m_aggregator)
        {
            shouldPublish = m_aggregator->process(report);
        }
    }

    if (shouldPublish)
    {
        publish(report);
    }
}

auto ErrorReporter::reportAndLog(const Error& error) -> void
{
    // Log first
    switch (error.severity())
    {
        case ErrorSeverity::Fatal:
            LOG4CXX_FATAL(logger, error.toString());
            break;
        case ErrorSeverity::Critical:
        case ErrorSeverity::Error:
            LOG4CXX_ERROR(logger, error.toString());
            break;
        case ErrorSeverity::Recoverable:
            LOG4CXX_WARN(logger, error.toString());
            break;
        case ErrorSeverity::Warning:
            LOG4CXX_WARN(logger, error.toString());
            break;
        case ErrorSeverity::Informational:
            LOG4CXX_INFO(logger, error.toString());
            break;
        default:
            LOG4CXX_DEBUG(logger, error.toString());
            break;
    }

    // Then report
    report(error);
}

auto ErrorReporter::createReport(const Error& error) -> ErrorReport
{
    return ErrorReport{error};
}

auto ErrorReporter::subscribe(ErrorSignal::slot_type slot) -> Connection
{
    return m_errorSignal.connect(std::move(slot));
}

auto ErrorReporter::subscribeScoped(ErrorSignal::slot_type slot) -> ScopedConnection
{
    return ScopedConnection{subscribe(std::move(slot))};
}

auto ErrorReporter::subscribeToSeverity(
    ErrorSeverity minSeverity,
    ErrorSignal::slot_type slot) -> Connection
{
    return subscribe([minSeverity, slot = std::move(slot)](const ErrorReport& report) {
        if (report.error().severity() >= minSeverity)
        {
            slot(report);
        }
    });
}

auto ErrorReporter::subscribeToService(
    ServiceId service,
    ErrorSignal::slot_type slot) -> Connection
{
    return subscribe([service, slot = std::move(slot)](const ErrorReport& report) {
        if (report.error().code().service() == service)
        {
            slot(report);
        }
    });
}

auto ErrorReporter::addSubscriber(std::shared_ptr<IErrorSubscriber> subscriber) -> Connection
{
    return subscribe([subscriber](const ErrorReport& report) {
        subscriber->onError(report);
    });
}

auto ErrorReporter::totalReported() const noexcept -> std::size_t
{
    return m_totalReported.load(std::memory_order_relaxed);
}

auto ErrorReporter::totalSuppressed() const noexcept -> std::size_t
{
    std::lock_guard lock{m_mutex};
    return m_aggregator ? m_aggregator->totalSuppressed() : 0;
}

auto ErrorReporter::totalPublished() const noexcept -> std::size_t
{
    std::lock_guard lock{m_mutex};
    return m_aggregator ? m_aggregator->totalPublished() : 0;
}

auto ErrorReporter::resetStatistics() -> void
{
    std::lock_guard lock{m_mutex};
    m_totalReported.store(0, std::memory_order_relaxed);
    if (m_aggregator)
    {
        m_aggregator->reset();
    }
}

auto ErrorReporter::publish(const ErrorReport& report) -> void
{
    m_errorSignal(report);
}

// ============================================
// LoggingErrorSubscriber Implementation
// ============================================

LoggingErrorSubscriber::LoggingErrorSubscriber()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto LoggingErrorSubscriber::onError(const ErrorReport& report) -> void
{
    logAtLevel(report);
}

auto LoggingErrorSubscriber::name() const -> std::string_view
{
    return "LoggingErrorSubscriber";
}

auto LoggingErrorSubscriber::logAtLevel(const ErrorReport& report) -> void
{
    const auto& error = report.error();
    auto message = report.toString();

    switch (error.severity())
    {
        case ErrorSeverity::Fatal:
            LOG4CXX_FATAL(logger, message);
            break;
        case ErrorSeverity::Critical:
        case ErrorSeverity::Error:
            LOG4CXX_ERROR(logger, message);
            break;
        case ErrorSeverity::Recoverable:
        case ErrorSeverity::Warning:
            LOG4CXX_WARN(logger, message);
            break;
        case ErrorSeverity::Informational:
            LOG4CXX_INFO(logger, message);
            break;
        case ErrorSeverity::Debug:
            LOG4CXX_DEBUG(logger, message);
            break;
        case ErrorSeverity::Trace:
            LOG4CXX_TRACE(logger, message);
            break;
    }
}

// ============================================
// ConsoleErrorSubscriber Implementation
// ============================================

ConsoleErrorSubscriber::ConsoleErrorSubscriber(Format format)
    : m_format{format}
{
}

auto ConsoleErrorSubscriber::onError(const ErrorReport& report) -> void
{
    switch (m_format)
    {
        case Format::Simple:
            std::cerr << report.error().message() << std::endl;
            break;
        case Format::Detailed:
            std::cerr << report.toString() << std::endl;
            break;
        case Format::Json:
            std::cerr << report.toJson() << std::endl;
            break;
    }
}

auto ConsoleErrorSubscriber::name() const -> std::string_view
{
    return "ConsoleErrorSubscriber";
}

// ============================================
// MetricsErrorSubscriber Implementation
// ============================================

MetricsErrorSubscriber::MetricsErrorSubscriber() = default;

auto MetricsErrorSubscriber::onError(const ErrorReport& report) -> void
{
    m_metrics.totalErrors.fetch_add(1, std::memory_order_relaxed);

    switch (report.error().severity())
    {
        case ErrorSeverity::Fatal:
            m_metrics.fatalErrors.fetch_add(1, std::memory_order_relaxed);
            break;
        case ErrorSeverity::Critical:
            m_metrics.criticalErrors.fetch_add(1, std::memory_order_relaxed);
            break;
        case ErrorSeverity::Recoverable:
        case ErrorSeverity::Error:
            m_metrics.recoverableErrors.fetch_add(1, std::memory_order_relaxed);
            break;
        case ErrorSeverity::Warning:
            m_metrics.warnings.fetch_add(1, std::memory_order_relaxed);
            break;
        default:
            break;
    }
}

auto MetricsErrorSubscriber::name() const -> std::string_view
{
    return "MetricsErrorSubscriber";
}

auto MetricsErrorSubscriber::metrics() const noexcept -> const Metrics&
{
    return m_metrics;
}

auto MetricsErrorSubscriber::reset() -> void
{
    std::lock_guard lock{m_mutex};
    m_metrics.totalErrors.store(0, std::memory_order_relaxed);
    m_metrics.fatalErrors.store(0, std::memory_order_relaxed);
    m_metrics.criticalErrors.store(0, std::memory_order_relaxed);
    m_metrics.recoverableErrors.store(0, std::memory_order_relaxed);
    m_metrics.warnings.store(0, std::memory_order_relaxed);
    m_metrics.byService.clear();
    m_metrics.byComponent.clear();
}
