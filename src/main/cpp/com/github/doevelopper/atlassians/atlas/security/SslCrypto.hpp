
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_SSLCRYPTO_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_SSLCRYPTO_HPP

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::night::owl::security
{
    /*!
     * @brief OpenSSL provides two primary libraries: libssl and libcrypto.
     *      The libcrypto library provides the fundamental cryptographic routines used
     *      by libssl. You can however use libcrypto without using libssl.
     */

    class SslCrypto
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
 public:
        SslCrypto() noexcept;
        SslCrypto(const SslCrypto&) noexcept = default;
        SslCrypto(SslCrypto&&) noexcept =  default;
        SslCrypto& operator=(const SslCrypto&) noexcept = default;
        SslCrypto& operator=(SslCrypto&&) noexcept = default;
        virtual ~SslCrypto() noexcept;

        /*!
        * @brief SSLCrypto API - Encrypt
        *
        * @param ciphername - input cipher
        * @param key - input key used for crypto operation
        * @param plaintext - input plain binary buffer to encrypt
        * @param ciphertext - output encrypted binary buffer
        * @param error - output error string set on failure
        * @return bool - true on success, false on failure
        */
        bool encrypt(const std::string& ciphername, const std::string& key, const std::string& plaintext, std::string& ciphertext);

        /*!
        * @brief SSLCrypto API - Decrypt
        *
        * @param ciphername - input cipher
        * @param key - input key used for crypto operation
        * @param ciphertext - input encrypted binary buffer to decrypt
        * @param plaintext - output plain binary buffer
        * @return bool - true on success, false on failure
        */
        bool decrypt(const std::string& ciphername, const std::string& key, const std::string& ciphertext, std::string& plaintext);

        /*!
        * @brief SSLCrypto API - ValidateKey
        *
        * @param key - input key used for crypto operation
        * @param ciphername - input cipher
        * @param error - output error string set on failure
        * @return bool - true on success, false on failure
        */
        bool validateCryptoParams(const std::string& ciphername, const std::string& key);

        bool generate_key(std::uint8_t* aKeyBuffer, std::size_t aKeySize);
    protected:
    private:
        bool entropy() const;
        void generateEntropy();

        static const std::size_t SEED_BUFFER_SIZE = 8;
    };
}

#endif
