/**
 * @file   HardwareIdValidator.hpp
 * @brief  Hardware ID binding validator
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_HARDWAREIDVALIDATOR_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_HARDWAREIDVALIDATOR_HPP

#include <functional>

#include <com/github/doevelopper/atlassians/licensing/BaseLicenseValidator.hpp>

namespace com::github::doevelopper::atlassians::licensing
{

/**
 * @brief Validator for hardware-bound licenses
 *
 * Verifies that the license is valid for the current hardware
 * by comparing hardware IDs.
 */
class HardwareIdValidator : public BaseLicenseValidator
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    /**
     * @brief Hardware ID generator function type
     */
    using HardwareIdGenerator = std::function<std::string()>;

    /**
     * @brief Constructor
     */
    HardwareIdValidator();

    /**
     * @brief Constructor with custom hardware ID generator
     */
    explicit HardwareIdValidator(HardwareIdGenerator generator);

    ~HardwareIdValidator() override;

    /**
     * @brief Set custom hardware ID generator
     */
    auto setHardwareIdGenerator(HardwareIdGenerator generator) -> void;

    /**
     * @brief Get the current system's hardware ID
     */
    [[nodiscard]] auto getCurrentHardwareId() const -> std::string;

    /**
     * @brief Generate hardware ID using default algorithm
     *
     * Generates a hardware fingerprint based on:
     * - CPU information
     * - MAC address
     * - Disk serial number (if available)
     */
    [[nodiscard]] static auto generateDefaultHardwareId() -> std::string;

protected:
    [[nodiscard]] auto doValidate(const License& license) const -> ValidationResult override;

private:
    HardwareIdGenerator m_hardwareIdGenerator;
};

} // namespace com::github::doevelopper::atlassians::licensing

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LICENSING_HARDWAREIDVALIDATOR_HPP
