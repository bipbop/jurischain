import subprocess
import sha3

command = ["./sha3", "123456789", "5"]
seed = "123456789"

def test_sha3():
    global command
    global seed

    print('[*] Testing SHA3-256 algorithm output...')
    res = subprocess.check_output(command).splitlines()
    hash1 = res[1].decode('utf-8').lower()
    hash2 = sha3.sha3_256(seed.encode('utf-8')).hexdigest()

    assert hash1 == hash2, "Hashes don't match!"


#def test_try():
    

def main():
    test_sha3()
    print('Passed!')


main()
