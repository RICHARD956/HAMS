#include "CryptoUtils.h"

#ifdef USE_OPENSSL
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#endif

#include <iomanip>
#include <sstream>

// static key/iv for demonstration only; in real usage these should be
// generated securely and not hard-coded.
static const unsigned char AES_KEY[32] = {
    0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,
    0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4
};
static const unsigned char AES_IV[16] = {0x00};

bool encryptAES256(const std::string &plaintext, std::vector<unsigned char> &ciphertext) {
#ifdef USE_OPENSSL
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, AES_KEY, AES_IV)) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    int outlen1 = plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc());
    ciphertext.resize(outlen1);
    int len;
    if (1 != EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
                               reinterpret_cast<const unsigned char*>(plaintext.data()), plaintext.size())) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    int outlen = len;
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    outlen += len;
    ciphertext.resize(outlen);
    EVP_CIPHER_CTX_free(ctx);
    return true;
#else
    // fallback: just store plaintext
    ciphertext.assign(plaintext.begin(), plaintext.end());
    return true;
#endif
}

bool decryptAES256(const std::vector<unsigned char> &ciphertext, std::string &plaintext) {
#ifdef USE_OPENSSL
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, AES_KEY, AES_IV)) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    int outlen1 = ciphertext.size();
    std::vector<unsigned char> outbuf(outlen1 + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len;
    if (1 != EVP_DecryptUpdate(ctx, outbuf.data(), &len, ciphertext.data(), ciphertext.size())) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    int outlen = len;
    if (1 != EVP_DecryptFinal_ex(ctx, outbuf.data() + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    outlen += len;
    plaintext.assign(reinterpret_cast<char*>(outbuf.data()), outlen);
    EVP_CIPHER_CTX_free(ctx);
    return true;
#else
    // fallback: return original data as string
    plaintext.assign(ciphertext.begin(), ciphertext.end());
    return true;
#endif
}

std::string sha256(const std::string &input) {
#ifdef USE_OPENSSL
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.c_str(), input.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
#else
    // simple non-cryptographic fallback (not real SHA) for build when OpenSSL unavailable
    std::stringstream ss;
    for (unsigned char c : input) {
        ss << std::hex << ((int)c + 0x10);
    }
    return ss.str();
#endif
}
