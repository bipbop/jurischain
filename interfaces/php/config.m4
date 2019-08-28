PHP_ARG_ENABLE(juschain, whether to enable juschain support,
[  --enable-juschain          Enable juschain support])

if test "$PHP_JUSCHAIN" != "no"; then
  AC_DEFINE(HAVE_JUSCHAIN, 1, [ Have juschain support ])
  PHP_NEW_EXTENSION(juschain, juschain.c, $ext_shared)
fi

-ljuschain