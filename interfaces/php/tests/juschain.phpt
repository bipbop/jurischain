--TEST--
echo - basic test for jurischain
--FILE--
<?php
$jurischain = jurischain_gen(10, "Hello");
while (!jurischain_try($jurischain));
echo jurischain_get($jurischain) . "\r\n";
$jurischain_new = jurischain_gen(10, "Hello");
jurischain_set($jurischain_new, jurischain_get($jurischain));
echo jurischain_get($jurischain_new) . "\r\n";
echo jurischain_verify($jurischain) ? "1" : "0";
?>
--EXPECT--
C9392EDB7A4DBAE48017FD43E376E45EAC1CB54568BD206E4DD1F204E4003F7A
C9392EDB7A4DBAE48017FD43E376E45EAC1CB54568BD206E4DD1F204E4003F7A
1