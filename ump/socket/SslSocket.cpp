/*
Copyright 2016 fuzz, Inc. All rights reserved. 
   http://www.fuzz.co.jp

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of fuzz, Inc. nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL FUZZ, INC. BE LIABLE FOR ANY 
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/***********************************************************************//**
	@file
***************************************************************************/
#include "ump/socket/SslSocket.hpp"

namespace ump {
namespace socket {
/***********************************************************************//**
	@brief 
***************************************************************************/
SslSocket::SslSocket(BIO* bio)
  : bio_(bio)
{
}
/***********************************************************************//**
	@brief デストラクタ.
***************************************************************************/
SslSocket::~SslSocket() {
  close();
}
/***********************************************************************//**
	@copydoc Socket::isOpen
***************************************************************************/
bool SslSocket::isOpen() const {
  return bio_;
}
/***********************************************************************//**
	@copydoc Socket::connect
***************************************************************************/
bool SslSocket::onConnect(const std::string& host, int port) {
  bio_ = BIO_new_ssl_connect(GetContext().getClientCtx());
  std::stringstream buff;
  buff << host << ":" << port;
  BIO_set_conn_hostname(bio_, buff.str().c_str());
  if(BIO_do_connect(bio_) <= 0) {
    ERR_print_errors_fp(stderr);
    return false;
  }
  return true;
}
/***********************************************************************//**
	@copydoc Socket::listen
***************************************************************************/
bool SslSocket::onListen(int port) {
  std::stringstream buff;
  buff << port;
  bio_ = BIO_new_accept(const_cast<char*>(buff.str().c_str()));
  BIO_set_accept_bios(bio_, BIO_new_ssl(GetContext().getServerCtx(), 0));
  return true;
}
/***********************************************************************//**
	@copydoc Socket::onAccept
***************************************************************************/
std::shared_ptr<Socket> SslSocket::onAccept(int timeout) {
  if(BIO_do_accept(bio_) > 0) {
    if(BIO_do_accept(bio_) > 0) {
      BIO* bio = BIO_pop(bio_);
      if(BIO_do_handshake(bio) > 0) {
        return std::make_shared<SslSocket>(bio);
      }
      BIO_free_all(bio);
    }
  }
  return nullptr;
}
/***********************************************************************//**
	@copydoc Socket::send
***************************************************************************/
bool SslSocket::onSend(const char* buff, size_t size) {
  while(size > 0) {
    auto sendSize = BIO_write(bio_, buff, size);
    if(sendSize > 0) {
      buff += sendSize;
      size -= sendSize;
    }
    else if(!BIO_should_retry(bio_)) {
      onClose();
      return false;
    }
  }
  return true;
}
/***********************************************************************//**
	@copydoc Socket::recv
***************************************************************************/
bool SslSocket::onRecv(char* buff, size_t size) {
  while(size > 0) {
    auto readSize = BIO_read(bio_, buff, size);
    if(readSize > 0) {
      buff += readSize;
      size -= readSize;
    }
    else if(!BIO_should_retry(bio_)) {
      onClose();
      return false;
    }
  }
  return true;
}
/***********************************************************************//**
	@copydoc Socket::onClose
***************************************************************************/
void SslSocket::onClose() {
  if(bio_) {
    BIO_free_all(bio_);
    bio_ = nullptr;
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
SslSocket::Context& SslSocket::GetContext() {
  static std::unique_ptr<Context> context;
  if(!context) {
    context.reset(new Context());
  }
  return *context;
}
/***********************************************************************//**
	@brief デフォルトコンストラクタ
***************************************************************************/
SslSocket::Context::Context()
  : clientCtx_(nullptr), 
    serverCtx_(nullptr), 
    pkey_(nullptr), 
    x509_(nullptr)
{
  SSL_load_error_strings();
  ERR_load_BIO_strings();
  OpenSSL_add_all_algorithms();
  SSL_library_init();
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
SslSocket::Context::~Context() {
  if(clientCtx_) {
    SSL_CTX_free(clientCtx_);
  }
  if(serverCtx_) {
    SSL_CTX_free(serverCtx_);
  }
  if(x509_) {
    X509_free(x509_);
  }
  if(rsa_) {
    RSA_free(rsa_);
  }
  if(pkey_) {
    EVP_PKEY_free(pkey_);
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
SSL_CTX* SslSocket::Context::getClientCtx() {
  if(!clientCtx_) {
    clientCtx_ = SSL_CTX_new(SSLv23_client_method());
  }
  return clientCtx_;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
SSL_CTX* SslSocket::Context::getServerCtx() {
  if(!serverCtx_) {
    setupCertificate();
    serverCtx_ = SSL_CTX_new(SSLv23_server_method());
    if(SSL_CTX_use_certificate(serverCtx_, x509_) <= 0) {
      ERR_print_errors_fp(stderr);
      assert(0);
    }
    if(SSL_CTX_use_PrivateKey(serverCtx_, pkey_) <= 0) {
      ERR_print_errors_fp(stderr);
      assert(0);
    }
  }
  return serverCtx_;
}
/***********************************************************************//**
	@brief 証明書のセットアップ
***************************************************************************/
void SslSocket::Context::setupCertificate() {
  pkey_ = EVP_PKEY_new();
  rsa_ = RSA_generate_key(2048, RSA_F4, nullptr, nullptr);
  EVP_PKEY_assign_RSA(pkey_, rsa_);
  x509_ = X509_new();
  X509_gmtime_adj(X509_get_notBefore(x509_), 0);
  X509_gmtime_adj(X509_get_notAfter(x509_), 60 * 60 * 24);
  X509_set_pubkey(x509_, pkey_);
  X509_sign(x509_, pkey_, EVP_sha1());
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
}
