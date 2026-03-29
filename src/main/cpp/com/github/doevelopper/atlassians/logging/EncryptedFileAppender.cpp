/*******************************************************************
 * @file   EncryptedFileAppender.cpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    log4cxx appender with encryption support - Implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/logging/EncryptedFileAppender.hpp>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <random>
#include <sstream>
#include <stdexcept>

#include <log4cxx/helpers/transcoder.h>
#include <log4cxx/logstring.h>

// Conditional compilation for encryption library
#if __has_include(<openssl/evp.h>)
    #include <openssl/evp.h>
    #include <openssl/rand.h>
    #define USE_OPENSSL 1
#elif __has_include(<cryptopp/aes.h>)
    #include <cryptopp/aes.h>
    #include <cryptopp/gcm.h>
    #include <cryptopp/filters.h>
    #include <cryptopp/base64.h>
    #include <cryptopp/osrng.h>
    #define USE_CRYPTOPP 1
#else
    #define USE_NO_CRYPTO 1
#endif

namespace com::github::doevelopper::atlassians::logging
{
    // ============================================
    // StaticKeyProvider Implementation
    // ============================================

    class StaticKeyProvider::Impl
    {
    public:
        explicit Impl(std::vector<uint8_t> key)
            : m_key(std::move(key))
            , m_keyId(generateKeyId())
        {
        }

        explicit Impl(std::string_view hexKey)
            : m_key(hexToBytes(hexKey))
            , m_keyId(generateKeyId())
        {
        }

        [[nodiscard]] std::vector<uint8_t> getKey() const
        {
            return m_key;
        }

        [[nodiscard]] std::string getKeyId() const
        {
            return m_keyId;
        }

    private:
        static std::vector<uint8_t> hexToBytes(std::string_view hex)
        {
            std::vector<uint8_t> bytes;
            bytes.reserve(hex.size() / 2);

            for (std::size_t i = 0; i < hex.size(); i += 2)
            {
                std::string byteStr(hex.substr(i, 2));
                bytes.push_back(static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16)));
            }

            return bytes;
        }

        std::string generateKeyId()
        {
            std::ostringstream oss;
            oss << "static-";

            // Generate hash-like ID from first bytes
            for (std::size_t i = 0; i < std::min(m_key.size(), std::size_t{4}); ++i)
            {
                oss << std::hex << std::setw(2) << std::setfill('0')
                    << static_cast<int>(m_key[i]);
            }

            return oss.str();
        }

        std::vector<uint8_t> m_key;
        std::string m_keyId;
    };

    StaticKeyProvider::StaticKeyProvider(std::vector<uint8_t> key)
        : m_impl(std::make_unique<Impl>(std::move(key)))
    {
    }

    StaticKeyProvider::StaticKeyProvider(std::string_view hexKey)
        : m_impl(std::make_unique<Impl>(hexKey))
    {
    }

    StaticKeyProvider::StaticKeyProvider(StaticKeyProvider&&) noexcept = default;
    StaticKeyProvider& StaticKeyProvider::operator=(StaticKeyProvider&&) noexcept = default;
    StaticKeyProvider::~StaticKeyProvider() = default;

    std::vector<uint8_t> StaticKeyProvider::getKey() const
    {
        return m_impl->getKey();
    }

    std::string StaticKeyProvider::getKeyId() const
    {
        return m_impl->getKeyId();
    }

    // ============================================
    // EnvironmentKeyProvider Implementation
    // ============================================

    class EnvironmentKeyProvider::Impl
    {
    public:
        explicit Impl(std::string envVarName)
            : m_envVarName(std::move(envVarName))
            , m_keyVersion(0)
        {
            loadKey();
        }

        [[nodiscard]] std::vector<uint8_t> getKey() const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_key;
        }

        bool rotateKey()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            loadKey();
            ++m_keyVersion;
            return !m_key.empty();
        }

        [[nodiscard]] std::string getKeyId() const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::ostringstream oss;
            oss << "env-" << m_envVarName << "-v" << m_keyVersion;
            return oss.str();
        }

    private:
        void loadKey()
        {
            const char* envValue = std::getenv(m_envVarName.c_str());
            if (envValue != nullptr)
            {
                std::string_view keyStr(envValue);
                m_key = hexToBytes(keyStr);
            }
        }

        static std::vector<uint8_t> hexToBytes(std::string_view hex)
        {
            std::vector<uint8_t> bytes;
            bytes.reserve(hex.size() / 2);

            for (std::size_t i = 0; i + 1 < hex.size(); i += 2)
            {
                std::string byteStr(hex.substr(i, 2));
                try
                {
                    bytes.push_back(static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16)));
                }
                catch (...)
                {
                    break;
                }
            }

            return bytes;
        }

        std::string m_envVarName;
        std::vector<uint8_t> m_key;
        std::size_t m_keyVersion;
        mutable std::mutex m_mutex;
    };

    EnvironmentKeyProvider::EnvironmentKeyProvider(std::string envVarName)
        : m_impl(std::make_unique<Impl>(std::move(envVarName)))
    {
    }

    EnvironmentKeyProvider::EnvironmentKeyProvider(EnvironmentKeyProvider&&) noexcept = default;
    EnvironmentKeyProvider& EnvironmentKeyProvider::operator=(EnvironmentKeyProvider&&) noexcept = default;
    EnvironmentKeyProvider::~EnvironmentKeyProvider() = default;

    std::vector<uint8_t> EnvironmentKeyProvider::getKey() const
    {
        return m_impl->getKey();
    }

    bool EnvironmentKeyProvider::rotateKey()
    {
        return m_impl->rotateKey();
    }

    std::string EnvironmentKeyProvider::getKeyId() const
    {
        return m_impl->getKeyId();
    }

    // ============================================
    // AesGcmEncryptionStrategy Implementation
    // ============================================

    class AesGcmEncryptionStrategy::Impl
    {
    public:
        static constexpr std::size_t KEY_SIZE = 32;      // 256 bits
        static constexpr std::size_t IV_SIZE = 12;       // 96 bits (recommended for GCM)
        static constexpr std::size_t TAG_SIZE = 16;      // 128 bits

        explicit Impl(const std::vector<uint8_t>& key)
            : m_key(key)
            , m_keyProvider(nullptr)
        {
            if (key.size() != KEY_SIZE)
            {
                throw std::invalid_argument(
                    "AES-256-GCM requires a 32-byte key, got " +
                    std::to_string(key.size()) + " bytes");
            }
        }

        explicit Impl(std::shared_ptr<IKeyProvider> keyProvider)
            : m_keyProvider(std::move(keyProvider))
        {
            if (m_keyProvider)
            {
                m_key = m_keyProvider->getKey();
                if (m_key.size() != KEY_SIZE)
                {
                    throw std::invalid_argument(
                        "AES-256-GCM requires a 32-byte key, got " +
                        std::to_string(m_key.size()) + " bytes");
                }
            }
        }

        [[nodiscard]] std::vector<uint8_t> encrypt(std::string_view plaintext) const
        {
#if defined(USE_OPENSSL)
            return encryptOpenSSL(plaintext);
#elif defined(USE_CRYPTOPP)
            return encryptCryptoPP(plaintext);
#else
            return encryptFallback(plaintext);
#endif
        }

        [[nodiscard]] std::string decrypt(const std::vector<uint8_t>& ciphertext) const
        {
#if defined(USE_OPENSSL)
            return decryptOpenSSL(ciphertext);
#elif defined(USE_CRYPTOPP)
            return decryptCryptoPP(ciphertext);
#else
            return decryptFallback(ciphertext);
#endif
        }

        [[nodiscard]] bool isInitialized() const noexcept
        {
            return m_key.size() == KEY_SIZE;
        }

    private:
        std::vector<uint8_t> generateIV() const
        {
            std::vector<uint8_t> iv(IV_SIZE);

#if defined(USE_OPENSSL)
            RAND_bytes(iv.data(), static_cast<int>(iv.size()));
#elif defined(USE_CRYPTOPP)
            CryptoPP::AutoSeededRandomPool rng;
            rng.GenerateBlock(iv.data(), iv.size());
#else
            std::random_device rd;
            std::mt19937_64 gen(rd());
            std::uniform_int_distribution<uint16_t> dis(0, 255);
            for (auto& byte : iv)
            {
                byte = static_cast<uint8_t>(dis(gen));
            }
#endif
            return iv;
        }

#if defined(USE_OPENSSL)
        std::vector<uint8_t> encryptOpenSSL(std::string_view plaintext) const
        {
            std::vector<uint8_t> iv = generateIV();
            std::vector<uint8_t> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
            std::vector<uint8_t> tag(TAG_SIZE);

            EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
            if (!ctx)
            {
                throw std::runtime_error("Failed to create EVP_CIPHER_CTX");
            }

            try
            {
                if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1)
                {
                    throw std::runtime_error("EVP_EncryptInit_ex failed");
                }

                if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(IV_SIZE), nullptr) != 1)
                {
                    throw std::runtime_error("EVP_CIPHER_CTX_ctrl failed to set IV length");
                }

                if (EVP_EncryptInit_ex(ctx, nullptr, nullptr, m_key.data(), iv.data()) != 1)
                {
                    throw std::runtime_error("EVP_EncryptInit_ex failed to set key/IV");
                }

                int len = 0;
                if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
                    reinterpret_cast<const unsigned char*>(plaintext.data()),
                    static_cast<int>(plaintext.size())) != 1)
                {
                    throw std::runtime_error("EVP_EncryptUpdate failed");
                }

                int ciphertextLen = len;

                if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1)
                {
                    throw std::runtime_error("EVP_EncryptFinal_ex failed");
                }
                ciphertextLen += len;

                ciphertext.resize(static_cast<std::size_t>(ciphertextLen));

                if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, static_cast<int>(TAG_SIZE), tag.data()) != 1)
                {
                    throw std::runtime_error("EVP_CIPHER_CTX_ctrl failed to get tag");
                }

                EVP_CIPHER_CTX_free(ctx);
            }
            catch (...)
            {
                EVP_CIPHER_CTX_free(ctx);
                throw;
            }

            // Format: IV || ciphertext || tag
            std::vector<uint8_t> result;
            result.reserve(iv.size() + ciphertext.size() + tag.size());
            result.insert(result.end(), iv.begin(), iv.end());
            result.insert(result.end(), ciphertext.begin(), ciphertext.end());
            result.insert(result.end(), tag.begin(), tag.end());

            return result;
        }

        std::string decryptOpenSSL(const std::vector<uint8_t>& ciphertext) const
        {
            if (ciphertext.size() < IV_SIZE + TAG_SIZE)
            {
                throw std::runtime_error("Ciphertext too short");
            }

            // Extract IV, ciphertext, and tag
            std::vector<uint8_t> iv(ciphertext.begin(), ciphertext.begin() + IV_SIZE);
            std::vector<uint8_t> encData(ciphertext.begin() + IV_SIZE,
                                          ciphertext.end() - TAG_SIZE);
            std::vector<uint8_t> tag(ciphertext.end() - TAG_SIZE, ciphertext.end());

            std::vector<unsigned char> plaintext(encData.size());

            EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
            if (!ctx)
            {
                throw std::runtime_error("Failed to create EVP_CIPHER_CTX");
            }

            try
            {
                if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1)
                {
                    throw std::runtime_error("EVP_DecryptInit_ex failed");
                }

                if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(IV_SIZE), nullptr) != 1)
                {
                    throw std::runtime_error("EVP_CIPHER_CTX_ctrl failed to set IV length");
                }

                if (EVP_DecryptInit_ex(ctx, nullptr, nullptr, m_key.data(), iv.data()) != 1)
                {
                    throw std::runtime_error("EVP_DecryptInit_ex failed to set key/IV");
                }

                int len = 0;
                if (EVP_DecryptUpdate(ctx, plaintext.data(), &len,
                    encData.data(), static_cast<int>(encData.size())) != 1)
                {
                    throw std::runtime_error("EVP_DecryptUpdate failed");
                }

                int plaintextLen = len;

                if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, static_cast<int>(TAG_SIZE),
                    const_cast<uint8_t*>(tag.data())) != 1)
                {
                    throw std::runtime_error("EVP_CIPHER_CTX_ctrl failed to set tag");
                }

                if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1)
                {
                    throw std::runtime_error("Authentication failed");
                }
                plaintextLen += len;

                EVP_CIPHER_CTX_free(ctx);

                return std::string(plaintext.begin(), plaintext.begin() + plaintextLen);
            }
            catch (...)
            {
                EVP_CIPHER_CTX_free(ctx);
                throw;
            }
        }
#endif

#if defined(USE_CRYPTOPP)
        std::vector<uint8_t> encryptCryptoPP(std::string_view plaintext) const
        {
            std::vector<uint8_t> iv = generateIV();

            CryptoPP::GCM<CryptoPP::AES>::Encryption encryptor;
            encryptor.SetKeyWithIV(m_key.data(), m_key.size(), iv.data(), iv.size());

            std::string ciphertext;
            CryptoPP::StringSource ss(
                reinterpret_cast<const CryptoPP::byte*>(plaintext.data()),
                plaintext.size(),
                true,
                new CryptoPP::AuthenticatedEncryptionFilter(
                    encryptor,
                    new CryptoPP::StringSink(ciphertext),
                    false,
                    TAG_SIZE
                )
            );

            // Format: IV || ciphertext (includes tag)
            std::vector<uint8_t> result;
            result.reserve(iv.size() + ciphertext.size());
            result.insert(result.end(), iv.begin(), iv.end());
            result.insert(result.end(), ciphertext.begin(), ciphertext.end());

            return result;
        }

        std::string decryptCryptoPP(const std::vector<uint8_t>& ciphertext) const
        {
            if (ciphertext.size() < IV_SIZE + TAG_SIZE)
            {
                throw std::runtime_error("Ciphertext too short");
            }

            std::vector<uint8_t> iv(ciphertext.begin(), ciphertext.begin() + IV_SIZE);
            std::string encData(ciphertext.begin() + IV_SIZE, ciphertext.end());

            CryptoPP::GCM<CryptoPP::AES>::Decryption decryptor;
            decryptor.SetKeyWithIV(m_key.data(), m_key.size(), iv.data(), iv.size());

            std::string plaintext;
            CryptoPP::StringSource ss(
                encData,
                true,
                new CryptoPP::AuthenticatedDecryptionFilter(
                    decryptor,
                    new CryptoPP::StringSink(plaintext),
                    CryptoPP::AuthenticatedDecryptionFilter::DEFAULT_FLAGS,
                    TAG_SIZE
                )
            );

            return plaintext;
        }
#endif

#if defined(USE_NO_CRYPTO)
        // Fallback: XOR-based obfuscation (NOT SECURE - for compilation only)
        std::vector<uint8_t> encryptFallback(std::string_view plaintext) const
        {
            std::vector<uint8_t> result;
            result.reserve(plaintext.size());

            for (std::size_t i = 0; i < plaintext.size(); ++i)
            {
                result.push_back(static_cast<uint8_t>(plaintext[i]) ^
                                 m_key[i % m_key.size()]);
            }

            return result;
        }

        std::string decryptFallback(const std::vector<uint8_t>& ciphertext) const
        {
            std::string result;
            result.reserve(ciphertext.size());

            for (std::size_t i = 0; i < ciphertext.size(); ++i)
            {
                result.push_back(static_cast<char>(ciphertext[i] ^
                                 m_key[i % m_key.size()]));
            }

            return result;
        }
#endif

        std::vector<uint8_t> m_key;
        std::shared_ptr<IKeyProvider> m_keyProvider;
    };

    AesGcmEncryptionStrategy::AesGcmEncryptionStrategy(const std::vector<uint8_t>& key)
        : m_impl(std::make_unique<Impl>(key))
    {
    }

    AesGcmEncryptionStrategy::AesGcmEncryptionStrategy(std::shared_ptr<IKeyProvider> keyProvider)
        : m_impl(std::make_unique<Impl>(std::move(keyProvider)))
    {
    }

    AesGcmEncryptionStrategy::AesGcmEncryptionStrategy(AesGcmEncryptionStrategy&&) noexcept = default;
    AesGcmEncryptionStrategy& AesGcmEncryptionStrategy::operator=(AesGcmEncryptionStrategy&&) noexcept = default;
    AesGcmEncryptionStrategy::~AesGcmEncryptionStrategy() = default;

    std::vector<uint8_t> AesGcmEncryptionStrategy::encrypt(std::string_view plaintext) const
    {
        return m_impl->encrypt(plaintext);
    }

    std::string AesGcmEncryptionStrategy::decrypt(const std::vector<uint8_t>& ciphertext) const
    {
        return m_impl->decrypt(ciphertext);
    }

    bool AesGcmEncryptionStrategy::isInitialized() const noexcept
    {
        return m_impl->isInitialized();
    }

    // ============================================
    // EncryptedFileAppender Implementation
    // ============================================

    class EncryptedFileAppender::Impl
    {
    public:
        Impl()
            : m_append(true)
            , m_compress(false)
        {
        }

        Impl(const std::filesystem::path& filePath,
             std::unique_ptr<IEncryptionStrategy> strategy,
             bool append)
            : m_filePath(filePath)
            , m_append(append)
            , m_compress(false)
            , m_encryptionStrategy(std::move(strategy))
        {
        }

        Impl(const std::filesystem::path& filePath,
             std::shared_ptr<IKeyProvider> keyProvider,
             bool append)
            : m_filePath(filePath)
            , m_append(append)
            , m_compress(false)
            , m_keyProvider(std::move(keyProvider))
            , m_encryptionStrategy(
                std::make_unique<AesGcmEncryptionStrategy>(m_keyProvider))
        {
        }

        void activateOptions()
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            // Ensure encryption is set up
            if (!m_encryptionStrategy && m_keyProvider)
            {
                m_encryptionStrategy =
                    std::make_unique<AesGcmEncryptionStrategy>(m_keyProvider);
            }

            if (!m_encryptionStrategy && !m_keyEnvVar.empty())
            {
                m_keyProvider = std::make_shared<EnvironmentKeyProvider>(m_keyEnvVar);
                m_encryptionStrategy =
                    std::make_unique<AesGcmEncryptionStrategy>(m_keyProvider);
            }

            // Open file
            if (!m_filePath.empty())
            {
                auto openMode = std::ios::out | std::ios::binary;
                if (m_append)
                {
                    openMode |= std::ios::app;
                }
                else
                {
                    openMode |= std::ios::trunc;
                }

                // Create parent directories if needed
                if (m_filePath.has_parent_path())
                {
                    std::filesystem::create_directories(m_filePath.parent_path());
                }

                m_fileStream.open(m_filePath, openMode);
            }
        }

        void close()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_fileStream.is_open())
            {
                m_fileStream.flush();
                m_fileStream.close();
            }
        }

        void append(std::string_view formattedMessage)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (!m_fileStream.is_open() || !m_encryptionStrategy)
            {
                return;
            }

            try
            {
                // Encrypt the message
                std::vector<uint8_t> encrypted =
                    m_encryptionStrategy->encrypt(formattedMessage);

                // Base64 encode for text file storage
                std::string base64 = base64Encode(encrypted);

                // Write with newline separator
                m_fileStream << base64 << '\n';
            }
            catch (const std::exception&)
            {
                // Log encryption errors somewhere safe
            }
        }

        void flush()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_fileStream.is_open())
            {
                m_fileStream.flush();
            }
        }

        bool rotateKey()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_keyProvider && m_keyProvider->rotateKey())
            {
                m_encryptionStrategy =
                    std::make_unique<AesGcmEncryptionStrategy>(m_keyProvider);
                return true;
            }
            return false;
        }

        [[nodiscard]] std::string getCurrentKeyId() const
        {
            if (m_keyProvider)
            {
                return m_keyProvider->getKeyId();
            }
            return "unknown";
        }

        [[nodiscard]] std::string_view getAlgorithm() const
        {
            if (m_encryptionStrategy)
            {
                return m_encryptionStrategy->algorithmName();
            }
            return "none";
        }

        // Property accessors
        void setFilePath(const std::filesystem::path& path) { m_filePath = path; }
        [[nodiscard]] std::filesystem::path getFilePath() const { return m_filePath; }

        void setAppend(bool append) { m_append = append; }
        [[nodiscard]] bool getAppend() const { return m_append; }

        void setKeyEnvVar(const std::string& envVar) { m_keyEnvVar = envVar; }
        void setCompress(bool compress) { m_compress = compress; }

        void setEncryptionStrategy(std::unique_ptr<IEncryptionStrategy> strategy)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_encryptionStrategy = std::move(strategy);
        }

        void setKeyProvider(std::shared_ptr<IKeyProvider> provider)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_keyProvider = std::move(provider);
        }

    private:
        static std::string base64Encode(const std::vector<uint8_t>& data)
        {
            static constexpr char base64Chars[] =
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

            std::string result;
            result.reserve(((data.size() + 2) / 3) * 4);

            for (std::size_t i = 0; i < data.size(); i += 3)
            {
                uint32_t n = static_cast<uint32_t>(data[i]) << 16;

                if (i + 1 < data.size())
                {
                    n |= static_cast<uint32_t>(data[i + 1]) << 8;
                }
                if (i + 2 < data.size())
                {
                    n |= static_cast<uint32_t>(data[i + 2]);
                }

                result.push_back(base64Chars[(n >> 18) & 0x3F]);
                result.push_back(base64Chars[(n >> 12) & 0x3F]);

                if (i + 1 < data.size())
                {
                    result.push_back(base64Chars[(n >> 6) & 0x3F]);
                }
                else
                {
                    result.push_back('=');
                }

                if (i + 2 < data.size())
                {
                    result.push_back(base64Chars[n & 0x3F]);
                }
                else
                {
                    result.push_back('=');
                }
            }

            return result;
        }

        std::filesystem::path m_filePath;
        bool m_append;
        bool m_compress;
        std::string m_keyEnvVar;
        std::ofstream m_fileStream;
        std::shared_ptr<IKeyProvider> m_keyProvider;
        std::unique_ptr<IEncryptionStrategy> m_encryptionStrategy;
        mutable std::mutex m_mutex;
    };

    IMPLEMENT_LOG4CXX_OBJECT(EncryptedFileAppender)

    EncryptedFileAppender::EncryptedFileAppender()
        : m_impl(std::make_unique<Impl>())
    {
    }

    EncryptedFileAppender::EncryptedFileAppender(
        const std::filesystem::path& filePath,
        std::unique_ptr<IEncryptionStrategy> encryptionStrategy,
        bool append)
        : m_impl(std::make_unique<Impl>(filePath, std::move(encryptionStrategy), append))
    {
    }

    EncryptedFileAppender::EncryptedFileAppender(
        const std::filesystem::path& filePath,
        std::shared_ptr<IKeyProvider> keyProvider,
        bool append)
        : m_impl(std::make_unique<Impl>(filePath, std::move(keyProvider), append))
    {
    }

    EncryptedFileAppender::EncryptedFileAppender(EncryptedFileAppender&&) noexcept = default;
    EncryptedFileAppender& EncryptedFileAppender::operator=(EncryptedFileAppender&&) noexcept = default;
    EncryptedFileAppender::~EncryptedFileAppender() = default;

    void EncryptedFileAppender::setFile(const log4cxx::LogString& file)
    {
        std::string narrowFile;
        log4cxx::helpers::Transcoder::encode(file, narrowFile);
        m_impl->setFilePath(narrowFile);
    }

    log4cxx::LogString EncryptedFileAppender::getFile() const
    {
        log4cxx::LogString result;
        log4cxx::helpers::Transcoder::decode(m_impl->getFilePath().string(), result);
        return result;
    }

    void EncryptedFileAppender::setAppend(bool append)
    {
        m_impl->setAppend(append);
    }

    bool EncryptedFileAppender::getAppend() const
    {
        return m_impl->getAppend();
    }

    void EncryptedFileAppender::setKeyEnvVar(const log4cxx::LogString& envVar)
    {
        std::string narrowEnvVar;
        log4cxx::helpers::Transcoder::encode(envVar, narrowEnvVar);
        m_impl->setKeyEnvVar(narrowEnvVar);
    }

    void EncryptedFileAppender::setCompress(bool compress)
    {
        m_impl->setCompress(compress);
    }

    void EncryptedFileAppender::setEncryptionStrategy(
        std::unique_ptr<IEncryptionStrategy> strategy)
    {
        m_impl->setEncryptionStrategy(std::move(strategy));
    }

    void EncryptedFileAppender::setKeyProvider(std::shared_ptr<IKeyProvider> provider)
    {
        m_impl->setKeyProvider(std::move(provider));
    }

    void EncryptedFileAppender::activateOptions(log4cxx::helpers::Pool& pool)
    {
        AppenderSkeleton::activateOptions(pool);
        m_impl->activateOptions();
    }

    void EncryptedFileAppender::close()
    {
        m_impl->close();
    }

    bool EncryptedFileAppender::requiresLayout() const
    {
        return true;
    }

    bool EncryptedFileAppender::rotateKey()
    {
        return m_impl->rotateKey();
    }

    void EncryptedFileAppender::flush()
    {
        m_impl->flush();
    }

    std::string EncryptedFileAppender::getCurrentKeyId() const
    {
        return m_impl->getCurrentKeyId();
    }

    std::string_view EncryptedFileAppender::getAlgorithm() const
    {
        return m_impl->getAlgorithm();
    }

    void EncryptedFileAppender::append(
        const log4cxx::spi::LoggingEventPtr& event,
        log4cxx::helpers::Pool& pool)
    {
        if (layout)
        {
            log4cxx::LogString formattedEvent;
            layout->format(formattedEvent, event, pool);

            std::string narrowFormatted;
            log4cxx::helpers::Transcoder::encode(formattedEvent, narrowFormatted);

            m_impl->append(narrowFormatted);
        }
    }

}  // namespace com::github::doevelopper::atlassians::logging
