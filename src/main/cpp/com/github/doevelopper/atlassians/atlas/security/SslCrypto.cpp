
#include <com/github/doevelopper/atlassians/atlas/security/SslCrypto.hpp>

#include <openssl/conf.h>
// SSL I/F for all symmetric encryption algorithms
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/opensslv.h>
#include <openssl/rand.h>

#include <random>
#include <sstream>

using namespace com::github::doevelopper::night::owl::security;

log4cxx::LoggerPtr SslCrypto::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.security.SslCrypto"));

using BYTE = unsigned char ;
std::string IV = "4DBA325EAA8DBC69"; // initialization vector 16 bytes randomly generated

SslCrypto::SslCrypto() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    // multithreading initialization
    // this->setup();

    // Load the human readable error strings for libcrypto
    ERR_load_crypto_strings();

    // Load all digest and cipher algorithms
    OpenSSL_add_all_ciphers();
    //OpenSSL_add_all_algorithms();

    // Load config file, and other important initialisation
    OPENSSL_config(NULL);
}

SslCrypto::~SslCrypto() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    /* Removes all digests and ciphers */
    EVP_cleanup();

    /* if you omit the next, a small leak may be left when you make use of the BIO (low level API) for e.g. base64 transformations */
    CRYPTO_cleanup_all_ex_data();

    ERR_remove_thread_state(NULL);

    /* Remove error strings */
    ERR_free_strings();

    // multithreading cleanup
    // teardown();
}

bool SslCrypto::encrypt(const std::string& ciphername, const std::string& key, const std::string& plaintext, std::string& ciphertext)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
        // const EVP_CIPHER *cipher = EVP_get_cipherbyname(ciphername.c_str());
        // if(!cipher)
        // {
        //     LOG4CXX_ERROR(logger, "Error loading cipher: " + ciphername + ". Check crypto configuration");
        //     return false;
        // }
        //
        // EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        // // Create and initialise the context
        // if(!ctx)
        // {
        //     LOG4CXX_ERROR(logger, "Impossible create cipher context");
        //     return false;
        // }
        //
        // ciphertext.clear();
        //
        // // Initialise the encryption operation. IMPORTANT - ensure you use a key
        // // and IV size appropriate for your cipher
        // // The IV size for *most* modes is the same as the block size. For AES this is 128 bits
        // if(1 != EVP_EncryptInit_ex(ctx, cipher, NULL, (BYTE*)&key[0], (BYTE*)&IV[0]))
        // {
        //     LOG4CXX_ERROR(logger, "Failed to initialise the encryption operation");
        //     EVP_CIPHER_CTX_free(ctx);
        //     return false;
        // }
        //
        // // Recovered text expands up to EVP_MAX_BLOCK_LENGTH
        // //ciphertext.resize(plaintext.size()+EVP_MAX_BLOCK_LENGTH);
        // int cipher_block_size = EVP_CIPHER_block_size(cipher);
        // ciphertext.resize(plaintext.size() + cipher_block_size);
        // int len = (int)ciphertext.size();
        //
        // // By default encryption operations are padded using standard block padding and the padding is checked and removed when decrypting.
        // // If the pad parameter is zero then no padding is performed,
        // // the total amount of data encrypted or decrypted must then be a multiple of the block size or an error will occur
        //
        // // Provide the message to be encrypted, and obtain the encrypted output.
        // // EVP_EncryptUpdate can be called multiple times if necessary
        // if(1 != EVP_EncryptUpdate(ctx, (BYTE*)&ciphertext[0], &len, (const BYTE*)&plaintext[0], plaintext.size()))
        // {
        //     LOG4CXX_ERROR(logger, "Failed to perform EVP_EncryptUpdate");
        //     EVP_CIPHER_CTX_free(ctx);
        //     return false;
        // }
        //
        // // Finalise the encryption. Further ciphertext bytes may be written at this stage.
        // int ciphertext_len = (int)ciphertext.size() - len;
        // if(1 != EVP_EncryptFinal_ex(ctx, (BYTE*)&ciphertext[0]+ len, &ciphertext_len))
        // {
        //     LOG4CXX_ERROR(logger, "Failed to finalise the encryption");
        //     EVP_CIPHER_CTX_free(ctx);
        //     return false;
        // }
        // ciphertext_len += len;
        // ciphertext.resize(ciphertext_len);
        //
        // // Clean up
        // EVP_CIPHER_CTX_cleanup(ctx);
        // EVP_CIPHER_CTX_free(ctx);

        return true;
}

bool SslCrypto::decrypt(
    const std::string & ciphername, const std::string & key, const std::string & ciphertext, std::string & plaintext)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    const EVP_CIPHER * cipher = EVP_get_cipherbyname(ciphername.c_str());
    if (!cipher)
    {
        LOG4CXX_ERROR(logger, "Error loading cipher: " << ciphername << ". Check crypto configuration");
        return false;
    }

    EVP_CIPHER_CTX * ctx = EVP_CIPHER_CTX_new();
    // Create and initialise the context
    if (!ctx)
    {
        LOG4CXX_ERROR(logger, "Impossible create cipher context.");
        return false;
    }

    plaintext.clear();

    // Initialise the encryption operation. IMPORTANT - ensure you use a key
    // and IV size appropriate for your cipher
    // The IV size for *most* modes is the same as the block size. For AES this is 128 bits
    if (1 != EVP_DecryptInit_ex(ctx, cipher, NULL, (BYTE *)&key[0], (BYTE *)&IV[0]))
    {
        LOG4CXX_ERROR(logger, "Failed to initialise the decryption operation");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    // Recovered text expands up to EVP_MAX_BLOCK_LENGTH
    plaintext.resize(ciphertext.size() + EVP_MAX_BLOCK_LENGTH);
    int len = (int)ciphertext.size();

    // By default encryption operations are padded using standard block padding and the padding is checked and removed
    // when decrypting. If the pad parameter is zero then no padding is performed, the total amount of data encrypted or
    // decrypted must then be a multiple of the block size or an error will occur.

    // Provide the message to be encrypted, and obtain the encrypted output.
    // EVP_EncryptUpdate can be called multiple times if necessary
    if (1 != EVP_DecryptUpdate(ctx, (BYTE *)&plaintext[0], &len, (const BYTE *)&ciphertext[0], ciphertext.size()))
    {
        LOG4CXX_ERROR(logger, "Failed to perform EVP_DecryptUpdate");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    // Finalise the encryption. Further ciphertext bytes may be written at this stage.
    int plaintext_len = (int)plaintext.size() - len;
    if (1 != EVP_DecryptFinal_ex(ctx, (BYTE *)&plaintext[0] + len, &plaintext_len))
    {
        LOG4CXX_ERROR(logger, "Failed to finalise the decryption");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    plaintext_len += len;
    plaintext.resize(plaintext_len);

    // Clean up
    EVP_CIPHER_CTX_cleanup(ctx);
    EVP_CIPHER_CTX_free(ctx);

    return true;
}

bool SslCrypto::validateCryptoParams(const std::string & ciphername, const std::string & key)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );

    // if(!key.empty() && !ciphername.empty())
    // {
    //     // check for valid key
    //     {
    //         const EVP_CIPHER *cipher = EVP_get_cipherbyname(ciphername.c_str());
    //         if(!cipher)
    //         {
    //             LOG4CXX_ERROR(logger, "Error validating encryption params: ciphername: " << ciphername);
    //             return false;
    //         }
    //
    //         EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    //         if(!ctx)
    //         {
    //             LOG4CXX_ERROR(logger, "Error validating encryption params: CIPHER_CTX");
    //             return false;
    //         }
    //
    //         if(1 != EVP_EncryptInit_ex(ctx, cipher, NULL, NULL, (BYTE*)&IV[0]))
    //         {
    //             LOG4CXX_ERROR(logger, "Error validating encryption params: EncryptInit");
    //             EVP_CIPHER_CTX_free(ctx);
    //             return false;
    //         }
    //
    //         int IV_size = EVP_CIPHER_CTX_iv_length(ctx);
    //         if(IV_size > 0 && (IV_size != IV.size()))
    //         {
    //             std::stringstream iv_len;
    //             iv_len << IV_size;
    //             LOG4CXX_ERROR(logger, " FAILED - cipher: " << ciphername << ", IV length not correct. Expected: " << iv_len.str());
    //             EVP_CIPHER_CTX_free(ctx);
    //             return false;
    //         }
    //
    //         // check the key length for the current cipher
    //         if(!EVP_CIPHER_CTX_set_key_length(ctx, key.size()))
    //         {
    //             std::stringstream key_len;
    //             key_len << EVP_CIPHER_key_length(cipher);
    //             LOG4CXX_ERROR(logger, " FAILED - cipher: " << ciphername << ", key length not correct. Expected: " << key_len.str());
    //             EVP_CIPHER_CTX_free(ctx);
    //             return false;
    //         }
    //
    //         if(1 != EVP_EncryptInit_ex(ctx, cipher, NULL, (BYTE*)&key[0], (BYTE*)&IV[0]))
    //         {
    //             LOG4CXX_ERROR(logger, " Error validating encryption params: EncryptInit");
    //             EVP_CIPHER_CTX_free(ctx);
    //             return false;
    //         }
    //
    //         EVP_CIPHER_CTX_cleanup(ctx);
    //         EVP_CIPHER_CTX_free(ctx);
    //     }
    //     return true;
    // }
    LOG4CXX_ERROR(logger, " FAILED - empty param found - cipher:" << ciphername << " , key:" << key);
    return false;
}

bool SslCrypto::generate_key(std::uint8_t* aKeyBuffer, std::size_t aKeySize)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    while (this->entropy())
        this->generateEntropy();
    return RAND_bytes(aKeyBuffer, static_cast<int>(aKeySize)) == 1;
}

bool SslCrypto::entropy() const
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    return RAND_status() == 0;
}

void SslCrypto::generateEntropy()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    thread_local std::random_device tRandomDevice;
    thread_local std::random_device::result_type tSeedBuffer[SEED_BUFFER_SIZE];
    for (std::size_t n = 0; n < SEED_BUFFER_SIZE; ++n)
        tSeedBuffer[n] = tRandomDevice();
    RAND_seed(tSeedBuffer, sizeof(tSeedBuffer));
}