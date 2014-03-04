/*
 * Copyright (c) 2013, Ford Motor Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ford Motor Company nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRYPTO_MANAGER_IMPL_TEST_H_
#define CRYPTO_MANAGER_IMPL_TEST_H_

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <gtest/gtest.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <iostream>

#include "security_manager/crypto_manager.h"
#include "security_manager/crypto_manager_impl.h"
#include "security_manager/ssl_context.h"

namespace test {
namespace components {
namespace security_manager_test {

bool isErrorFatal(SSL *connection, int res) {
  int error = SSL_get_error(connection, res);
  return (error != SSL_ERROR_WANT_READ &&
      error != SSL_ERROR_WANT_WRITE);
}

TEST(HandshakeTest, Positive) {

  using security_manager::CryptoManager;
  using security_manager::CryptoManagerImpl;
  using security_manager::SSLContext;

  SSL_load_error_strings();
  ERR_load_BIO_strings();
  OpenSSL_add_all_algorithms();
  SSL_library_init();

  SSL_CTX * ctx = SSL_CTX_new(SSLv23_client_method());
  SSL_CTX_set_cipher_list(ctx, "ALL");
  SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

  SSL * connection = SSL_new(ctx);

  BIO* bioIn = BIO_new(BIO_s_mem());
  BIO* bioOut = BIO_new(BIO_s_mem());
  SSL_set_bio(connection, bioIn, bioOut);

  SSL_set_connect_state(connection);

  CryptoManager* crypto_manager = new CryptoManagerImpl();
  crypto_manager->Init();

  SSLContext *server_ctx = crypto_manager->CreateSSLContext();

  int res = 0;

  char *outBuf = new char[1024 * 1024];
  char *inBuf;

  for(;;) {
    res = SSL_do_handshake(connection);
    if (res < 0) {
      if (isErrorFatal(connection, res)) {
        break;
      }

      size_t outLen  = BIO_ctrl_pending(bioOut);
      if (outLen) {
        BIO_read(bioOut, outBuf, outLen);
      }
      size_t inLen;
      inBuf = static_cast<char*>(server_ctx->DoHandshakeStep(outBuf, outLen, &inLen));
      EXPECT_TRUE(inBuf != NULL);

      if (inLen) {
        BIO_write(bioIn, inBuf, inLen);
      }
    } else {
      break;
    }
  }

  EXPECT_EQ(res, 1);

  BIO *bioF = BIO_new(BIO_f_ssl());
  BIO_set_ssl(bioF, connection, BIO_NOCLOSE);

  char text[] = "Hello, it's the text to be encrypted";
  char *encryptedText = new char[1024];
  char *decryptedText;
  size_t text_len;

  // Encrypt text on client side
  BIO_write(bioF, text, sizeof(text));
  text_len = BIO_ctrl_pending(bioOut);
  size_t len = BIO_read(bioOut, encryptedText, text_len);

  // Decrypt text on server
  decryptedText = static_cast<char*>(server_ctx->Decrypt(encryptedText, len, &text_len));

  delete[] encryptedText;

  EXPECT_TRUE(decryptedText != NULL);
  std::cout << "decrypted: " << decryptedText << std::endl;
  EXPECT_EQ(strcmp(decryptedText, text), 0);

  // Encrypt text on server
  encryptedText = static_cast<char*>(server_ctx->Encrypt(text, sizeof(text), &text_len));

  // Decrypt it on client
  BIO_write(bioIn, encryptedText, text_len);
  text_len = BIO_read(bioF, decryptedText, 1024);
  EXPECT_EQ(strcmp(decryptedText, text), 0);

  EVP_cleanup();
}

}  // namespace crypto_manager_test
}  // namespace components
}  // namespace test

#endif /* CRYPTO_MANAGER_IMPL_TEST_H_ */