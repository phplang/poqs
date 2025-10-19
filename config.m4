PHP_ARG_WITH(poqs, for OpenQuantumSafe support,
[  --with-poqs                 Include POQS support])

if test "$PHP_POQS" != "no"; then
  PKG_CHECK_MODULES([OQS], [liboqs])
  PHP_EVAL_INCLINE([$OQS_CFLAGS])
  PHP_EVAL_LIBLINE([$OQS_LIBS], [POQS_SHARED_LIBADD])

  PHP_SETUP_OPENSSL([POQS_SHARED_LIBADD])

  PHP_SUBST([POQS_SHARED_LIBADD])

  dnl Explicitly not calling PHP_REQUIRE_CXX because we don't need -lstdc++
  dnl Only using C++ for the templates.
  dnl Other extensions may add it, but we're not going to be responsible for that.
  AC_PROG_CXX()
  AC_PROG_CXXCPP()

  PHP_NEW_EXTENSION(poqs, poqs.cpp poqs-kem.cpp poqs-rng.cpp poqs-sig.cpp, $ext_shared)
fi
