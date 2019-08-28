PHP_ARG_ENABLE(jurischain, whether to enable jurischain support,
[  --enable-jurischain          Enable jurischain support])

if test "$PHP_JURISCHAIN" != "no"; then
  AC_DEFINE(HAVE_JURISCHAIN, 1, [ Have jurischain support ])
  PHP_NEW_EXTENSION(jurischain, jurischain.c, $ext_shared)
fi

-ljurischain