#ifndef CRYPTOUTILS_H
#define CRYPTOUTILS_H

#include <string>
#include <vector>

// AES-256-CBC encryption/decryption using OpenSSL if available
bool encryptAES256(const std::string &plaintext, std::vector<unsigned char> &ciphertext);
bool decryptAES256(const std::vector<unsigned char> &ciphertext, std::string &plaintext);

// SHA-256 hashing; returns a hex-encoded digest
std::string sha256(const std::string &input);

#endif // CRYPTOUTILS_H
