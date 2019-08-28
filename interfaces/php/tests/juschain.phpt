--TEST--
echo - basic test for juschain
--FILE--
<?php
$juschain = juschain_gen(10, "Hello");
while (!juschain_try($juschain));
echo juschain_get($juschain) . "\r\n";
$juschain_new = juschain_gen(10, "Hello");
juschain_set($juschain_new, juschain_get($juschain));
echo juschain_get($juschain_new) . "\r\n";
echo juschain_verify($juschain) ? "1" : "0";
?>
--EXPECT--
C9392EDB7A4DBAE48017FD43E376E45EAC1CB54568BD206E4DD1F204E4003F7A
C9392EDB7A4DBAE48017FD43E376E45EAC1CB54568BD206E4DD1F204E4003F7A
1