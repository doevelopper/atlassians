/*******************************************************************
 * @file   EncryptedFileAppender.hpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    log4cxx appender with encryption support
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_ENCRYPTEDFILEAPPENDER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_ENCRYPTEDFILEAPPENDER_HPP

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <log4cxx/appenderskeleton.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/spi/loggingevent.h>

#include <com/github/doevelopper/atlassians/API_Export.hpp>

namespace com::github::doevelopper::atlassians::logging
{
    /**
     * @class IEncryptionStrategy
     * @brief Interface for encryption strategies
     *
     * Allows pluggable encryption algorithms (AES, ChaCha20, etc.)
     * following the Strategy pattern.
     */
    class IEncryptionStrategy
    {
    public:
        virtual ~IEncryptionStrategy() = default;

        /**
         * @brief Encrypt plaintext data
         * @param plaintext Data to encrypt
         * @return Encrypted data
         */
        [[nodiscard]] virtual std::vector<uint8_t> encrypt(
            std::string_view plaintext) const = 0;

        /**
         * @brief Decrypt ciphertext data
         * @param ciphertext Data to decrypt
         * @return Decrypted plaintext
         */
        [[nodiscard]] virtual std::string decrypt(
            const std::vector<uint8_t>& ciphertext) const = 0;

        /**
         * @brief Get the encryption algorithm name
         * @return Algorithm identifier (e.g., "AES-256-GCM")
         */
        [[nodiscard]] virtual std::string_view algorithmName() const noexcept = 0;

        /**
         * @brief Check if the strategy is properly initialized
         * @return true if ready to encrypt/decrypt
         */
        [[nodiscard]] virtual bool isInitialized() const noexcept = 0;

        /**
         * @brief Get the key size in bytes
         * @return Key size
         */
        [[nodiscard]] virtual std::size_t keySize() const noexcept = 0;
    };

    /**
     * @class IKeyProvider
     * @brief Interface for providing encryption keys
     *
     * Abstracts key management - keys can come from files,
     * environment variables, hardware security modules, etc.
     */
    class IKeyProvider
    {
    public:
        virtual ~IKeyProvider() = default;

        /**
         * @brief Get the current encryption key
         * @return Key bytes
         */
        [[nodiscard]] virtual std::vector<uint8_t> getKey() const = 0;

        /**
         * @brief Rotate to a new key
         * @return true if rotation successful
         */
        virtual bool rotateKey() = 0;

        /**
         * @brief Get key identifier for audit/tracking
         * @return Key ID
         */
        [[nodiscard]] virtual std::string getKeyId() const = 0;
    };

    /**
     * @class AesGcmEncryptionStrategy
     * @brief AES-256-GCM encryption implementation
     *
     * Uses authenticated encryption with associated data (AEAD)
     * for both confidentiality and integrity.
     */
    class SDLC_API AesGcmEncryptionStrategy : public IEncryptionStrategy
    {
    public:
        /**
         * @brief Constructor with key
         * @param key 32-byte key for AES-256
         * @throws std::invalid_argument if key size is invalid
         */
        explicit AesGcmEncryptionStrategy(const std::vector<uint8_t>& key);

        /**
         * @brief Constructor with key provider
         * @param keyProvider Provider for encryption key
         */
        explicit AesGcmEncryptionStrategy(std::shared_ptr<IKeyProvider> keyProvider);

        AesGcmEncryptionStrategy(const AesGcmEncryptionStrategy&) = delete;
        AesGcmEncryptionStrategy& operator=(const AesGcmEncryptionStrategy&) = delete;
        AesGcmEncryptionStrategy(AesGcmEncryptionStrategy&&) noexcept;
        AesGcmEncryptionStrategy& operator=(AesGcmEncryptionStrategy&&) noexcept;
        ~AesGcmEncryptionStrategy() override;

        [[nodiscard]] std::vector<uint8_t> encrypt(
            std::string_view plaintext) const override;

        [[nodiscard]] std::string decrypt(
            const std::vector<uint8_t>& ciphertext) const override;

        [[nodiscard]] std::string_view algorithmName() const noexcept override
        {
            return "AES-256-GCM";
        }

        [[nodiscard]] bool isInitialized() const noexcept override;

        [[nodiscard]] std::size_t keySize() const noexcept override
        {
            return 32;  // 256 bits
        }

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };

    /**
     * @class StaticKeyProvider
     * @brief Simple key provider with a static key
     *
     * For testing or simple use cases. In production, consider
     * using HSM-backed or key-rotation-capable providers.
     */
    class SDLC_API StaticKeyProvider : public IKeyProvider
    {
    public:
        /**
         * @brief Constructor with key bytes
         * @param key The encryption key
         */
        explicit StaticKeyProvider(std::vector<uint8_t> key);

        /**
         * @brief Constructor with hex-encoded key
         * @param hexKey Hex-encoded key string
         */
        explicit StaticKeyProvider(std::string_view hexKey);

        StaticKeyProvider(const StaticKeyProvider&) = delete;
        StaticKeyProvider& operator=(const StaticKeyProvider&) = delete;
        StaticKeyProvider(StaticKeyProvider&&) noexcept;
        StaticKeyProvider& operator=(StaticKeyProvider&&) noexcept;
        ~StaticKeyProvider() override;

        [[nodiscard]] std::vector<uint8_t> getKey() const override;

        bool rotateKey() override
        {
            return false;  // Static key doesn't support rotation
        }

        [[nodiscard]] std::string getKeyId() const override;

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };

    /**
     * @class EnvironmentKeyProvider
     * @brief Key provider that reads from environment variable
     */
    class SDLC_API EnvironmentKeyProvider : public IKeyProvider
    {
    public:
        /**
         * @brief Constructor with environment variable name
         * @param envVarName Environment variable containing the key
         */
        explicit EnvironmentKeyProvider(std::string envVarName);

        EnvironmentKeyProvider(const EnvironmentKeyProvider&) = delete;
        EnvironmentKeyProvider& operator=(const EnvironmentKeyProvider&) = delete;
        EnvironmentKeyProvider(EnvironmentKeyProvider&&) noexcept;
        EnvironmentKeyProvider& operator=(EnvironmentKeyProvider&&) noexcept;
        ~EnvironmentKeyProvider() override;

        [[nodiscard]] std::vector<uint8_t> getKey() const override;

        bool rotateKey() override;

        [[nodiscard]] std::string getKeyId() const override;

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };

    /**
     * @class EncryptedFileAppender
     * @brief log4cxx appender that encrypts log entries before writing
     *
     * Features:
     * - Pluggable encryption strategy (AES-GCM by default)
     * - Pluggable key provider
     * - Base64 encoding for text file output
     * - Optional compression before encryption
     * - Thread-safe operation
     * - Key rotation support
     *
     * Example usage in log4cxx.xml:
     * @code{.xml}
     * <appender name="ENCRYPTED" class="com.github.doevelopper.premisses.logging.EncryptedFileAppender">
     *     <param name="File" value="encrypted.log"/>
     *     <param name="Append" value="true"/>
     *     <param name="KeyEnvVar" value="LOG_ENCRYPTION_KEY"/>
     *     <layout class="org.apache.log4j.PatternLayout">
     *         <param name="ConversionPattern" value="%d{ISO8601} [%t] %-5p %c - %m%n"/>
     *     </layout>
     * </appender>
     * @endcode
     *
     * Example programmatic usage:
     * @code
     * // Create key provider
     * auto keyProvider = std::make_shared<StaticKeyProvider>("your-32-byte-hex-key-here");
     *
     * // Create encryption strategy
     * auto encryption = std::make_unique<AesGcmEncryptionStrategy>(keyProvider);
     *
     * // Create appender
     * auto appender = std::make_shared<EncryptedFileAppender>(
     *     "encrypted.log",
     *     std::move(encryption)
     * );
     * appender->setLayout(std::make_shared<::log4cxx::PatternLayout>(...));
     * appender->activateOptions(pool);
     *
     * // Add to logger
     * logger->addAppender(appender);
     * @endcode
     */
    class SDLC_API EncryptedFileAppender : public ::log4cxx::AppenderSkeleton
    {
    public:
        DECLARE_LOG4CXX_OBJECT(EncryptedFileAppender)
        BEGIN_LOG4CXX_CAST_MAP()
            LOG4CXX_CAST_ENTRY(EncryptedFileAppender)
            LOG4CXX_CAST_ENTRY_CHAIN(AppenderSkeleton)
        END_LOG4CXX_CAST_MAP()

        /**
         * @brief Default constructor
         */
        EncryptedFileAppender();

        /**
         * @brief Constructor with file path and encryption strategy
         * @param filePath Path to the encrypted log file
         * @param encryptionStrategy Encryption strategy to use
         * @param append true to append, false to overwrite
         */
        EncryptedFileAppender(
            const std::filesystem::path& filePath,
            std::unique_ptr<IEncryptionStrategy> encryptionStrategy,
            bool append = true);

        /**
         * @brief Constructor with file path and key provider
         * @param filePath Path to the encrypted log file
         * @param keyProvider Provider for encryption key
         * @param append true to append, false to overwrite
         */
        EncryptedFileAppender(
            const std::filesystem::path& filePath,
            std::shared_ptr<IKeyProvider> keyProvider,
            bool append = true);

        EncryptedFileAppender(const EncryptedFileAppender&) = delete;
        EncryptedFileAppender& operator=(const EncryptedFileAppender&) = delete;
        EncryptedFileAppender(EncryptedFileAppender&&) noexcept;
        EncryptedFileAppender& operator=(EncryptedFileAppender&&) noexcept;
        ~EncryptedFileAppender() override;

        // ============================================
        // log4cxx Configuration Properties
        // ============================================

        /**
         * @brief Set the log file path
         * @param file File path
         */
        void setFile(const ::log4cxx::LogString& file);

        /**
         * @brief Get the log file path
         * @return File path
         */
        [[nodiscard]] ::log4cxx::LogString getFile() const;

        /**
         * @brief Set append mode
         * @param append true to append, false to overwrite
         */
        void setAppend(bool append);

        /**
         * @brief Get append mode
         * @return true if appending
         */
        [[nodiscard]] bool getAppend() const;

        /**
         * @brief Set the environment variable for key
         * @param envVar Environment variable name
         */
        void setKeyEnvVar(const ::log4cxx::LogString& envVar);

        /**
         * @brief Set whether to compress before encrypting
         * @param compress true to enable compression
         */
        void setCompress(bool compress);

        /**
         * @brief Set the encryption strategy (programmatic use)
         * @param strategy The encryption strategy
         */
        void setEncryptionStrategy(std::unique_ptr<IEncryptionStrategy> strategy);

        /**
         * @brief Set the key provider (programmatic use)
         * @param provider The key provider
         */
        void setKeyProvider(std::shared_ptr<IKeyProvider> provider);

        // ============================================
        // log4cxx Appender Interface
        // ============================================

        /**
         * @brief Activate appender options
         * @param pool APR pool for memory allocation
         */
        void activateOptions(::log4cxx::helpers::Pool& pool) override;

        /**
         * @brief Close the appender
         */
        void close() override;

        /**
         * @brief Check if a layout is required
         * @return true (layout is required for formatting)
         */
        [[nodiscard]] bool requiresLayout() const override;

        // ============================================
        // Advanced Operations
        // ============================================

        /**
         * @brief Rotate the encryption key
         * @return true if rotation successful
         */
        bool rotateKey();

        /**
         * @brief Flush buffered data to disk
         */
        void flush();

        /**
         * @brief Get the current key ID
         * @return Key identifier
         */
        [[nodiscard]] std::string getCurrentKeyId() const;

        /**
         * @brief Get encryption algorithm name
         * @return Algorithm name
         */
        [[nodiscard]] std::string_view getAlgorithm() const;

    protected:
        /**
         * @brief Append a logging event
         * @param event The logging event
         * @param pool APR pool
         */
        void append(
            const ::log4cxx::spi::LoggingEventPtr& event,
            ::log4cxx::helpers::Pool& pool) override;

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };

    // Type alias for shared pointer
    using EncryptedFileAppenderPtr = std::shared_ptr<EncryptedFileAppender>;

}  // namespace com::github::doevelopper::atlassians::logging

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_ENCRYPTEDFILEAPPENDER_HPP
