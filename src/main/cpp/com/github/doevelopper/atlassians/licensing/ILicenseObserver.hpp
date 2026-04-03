/**
 * @file   ILicenseObserver.hpp
 * @brief  Observer interface for license events
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSEOBSERVER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSEOBSERVER_HPP

#include <memory>

#include <com/github/doevelopper/atlassians/licensing/License.hpp>
#include <com/github/doevelopper/atlassians/licensing/LicenseTypes.hpp>

namespace com::github::doevelopper::atlassians::licensing
{

/**
 * @brief License event types
 */
enum class LicenseEvent : std::uint8_t
{
    Loaded,           ///< License was loaded
    Validated,        ///< License was validated
    ValidationFailed, ///< License validation failed
    Expired,          ///< License has expired
    ExpiringsSoon,    ///< License is expiring soon
    Revoked,          ///< License was revoked
    Updated,          ///< License was updated
    Removed           ///< License was removed
};

/**
 * @brief Observer interface for license events (Observer pattern)
 */
class ILicenseObserver
{
public:
    virtual ~ILicenseObserver() = default;

    /**
     * @brief Called when a license event occurs
     * @param event The type of event
     * @param license The license associated with the event
     */
    virtual auto onLicenseEvent(LicenseEvent event, const License& license) -> void = 0;

    /**
     * @brief Called when license validation result is available
     * @param license The license that was validated
     * @param result The validation result
     */
    virtual auto onValidationResult(const License& license, const ValidationResult& result) -> void = 0;

    // ============================================
    // Deleted Copy/Move (Interface)
    // ============================================

    ILicenseObserver(const ILicenseObserver&)            = delete;
    ILicenseObserver& operator=(const ILicenseObserver&) = delete;
    ILicenseObserver(ILicenseObserver&&)                 = default;
    ILicenseObserver& operator=(ILicenseObserver&&)      = default;

protected:
    ILicenseObserver() = default;
};

/**
 * @brief Shared pointer type for ILicenseObserver
 */
using ILicenseObserverPtr = std::shared_ptr<ILicenseObserver>;

/**
 * @brief Weak pointer type for ILicenseObserver
 */
using ILicenseObserverWPtr = std::weak_ptr<ILicenseObserver>;

} // namespace com::github::doevelopper::atlassians::licensing

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_ILICENSEOBSERVER_HPP
