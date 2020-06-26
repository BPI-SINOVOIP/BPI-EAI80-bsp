ugelis_interface_library_named(mbedTLS)

if(CONFIG_MBEDTLS_BUILTIN)
  target_compile_definitions(mbedTLS INTERFACE
	MBEDTLS_CONFIG_FILE="${CONFIG_MBEDTLS_CFG_FILE}"
	)

  target_include_directories(mbedTLS INTERFACE
	include
	configs
	)

  ugelis_library()
  ugelis_library_sources(ugelis_init.c)

  ugelis_library_sources(library/aes.c)
  ugelis_library_sources(library/aesni.c)
  ugelis_library_sources(library/arc4.c)
  ugelis_library_sources(library/asn1parse.c)
  ugelis_library_sources(library/asn1write.c)
  ugelis_library_sources(library/base64.c)
  ugelis_library_sources(library/bignum.c)
  ugelis_library_sources(library/blowfish.c)
  ugelis_library_sources(library/camellia.c)
  ugelis_library_sources(library/ccm.c)
  ugelis_library_sources(library/certs.c)
  ugelis_library_sources(library/cipher.c)
  ugelis_library_sources(library/cipher_wrap.c)
  ugelis_library_sources(library/cmac.c)
  ugelis_library_sources(library/ctr_drbg.c)
  ugelis_library_sources(library/debug.c)
  ugelis_library_sources(library/des.c)
  ugelis_library_sources(library/dhm.c)
  ugelis_library_sources(library/ecdh.c)
  ugelis_library_sources(library/ecdsa.c)
  ugelis_library_sources(library/ecjpake.c)
  ugelis_library_sources(library/ecp.c)
  ugelis_library_sources(library/ecp_curves.c)
  ugelis_library_sources(library/entropy.c)
  ugelis_library_sources(library/entropy_poll.c)
  ugelis_library_sources(library/error.c)
  ugelis_library_sources(library/gcm.c)
  ugelis_library_sources(library/havege.c)
  ugelis_library_sources(library/hmac_drbg.c)
  ugelis_library_sources(library/md.c)
  ugelis_library_sources(library/md2.c)
  ugelis_library_sources(library/md4.c)
  ugelis_library_sources(library/md5.c)
  ugelis_library_sources(library/md_wrap.c)
  ugelis_library_sources(library/memory_buffer_alloc.c)
  ugelis_library_sources(library/net_sockets.c)
  ugelis_library_sources(library/oid.c)
  ugelis_library_sources(library/padlock.c)
  ugelis_library_sources(library/pem.c)
  ugelis_library_sources(library/pk.c)
  ugelis_library_sources(library/pk_wrap.c)
  ugelis_library_sources(library/pkcs11.c)
  ugelis_library_sources(library/pkcs12.c)
  ugelis_library_sources(library/pkcs5.c)
  ugelis_library_sources(library/pkparse.c)
  ugelis_library_sources(library/pkwrite.c)
  ugelis_library_sources(library/platform.c)
  ugelis_library_sources(library/ripemd160.c)
  ugelis_library_sources(library/rsa.c)
  ugelis_library_sources(library/rsa_internal.c)
  ugelis_library_sources(library/sha1.c)
  ugelis_library_sources(library/sha256.c)
  ugelis_library_sources(library/sha512.c)
  ugelis_library_sources(library/ssl_cache.c)
  ugelis_library_sources(library/ssl_ciphersuites.c)
  ugelis_library_sources(library/ssl_cli.c)
  ugelis_library_sources(library/ssl_cookie.c)
  ugelis_library_sources(library/ssl_srv.c)
  ugelis_library_sources(library/ssl_ticket.c)
  ugelis_library_sources(library/ssl_tls.c)
  ugelis_library_sources(library/threading.c)
  ugelis_library_sources(library/timing.c)
  ugelis_library_sources(library/version.c)
  ugelis_library_sources(library/version_features.c)
  ugelis_library_sources(library/x509.c)
  ugelis_library_sources(library/x509_create.c)
  ugelis_library_sources(library/x509_crl.c)
  ugelis_library_sources(library/x509_crt.c)
  ugelis_library_sources(library/x509_csr.c)
  ugelis_library_sources(library/x509write_crt.c)
  ugelis_library_sources(library/x509write_csr.c)
  ugelis_library_sources(library/xtea.c)

  ugelis_library_link_libraries(mbedTLS)
else()
  assert(CONFIG_MBEDTLS_LIBRARY "MBEDTLS was enabled, but neither BUILTIN or LIBRARY was selected.")

  # NB: CONFIG_MBEDTLS_LIBRARY is not regression tested and is
  # therefore susceptible to bit rot

  target_include_directories(mbedTLS INTERFACE
	${CONFIG_MBEDTLS_INSTALL_PATH}
	)

  ugelis_link_libraries(
    mbedtls_external
    -L${CONFIG_MBEDTLS_INSTALL_PATH}
    gcc
    )
  # Lib mbedtls_external depends on libgcc (I assume?) so to allow
  # mbedtls_external to link with gcc we need to ensure it is placed
  # after mbedtls_external on the linkers command line.
endif()

target_link_libraries(mbedTLS INTERFACE ugelis_interface)
