import os
import subprocess
import json
import matplotlib.pyplot as plt

from multiprocessing import Pool


def calculate(seed):
    result = []
    s = f"{seed[:8]}"

    for i in range(1, 23):
        print(f"[*] Seed: {s} [{i}/22]")
        command = ["./sha3", "aaaaaa", f"{seed}", f"{i}"]
    
        res = subprocess.check_output(command).splitlines()
        diff = int(str(res[2]).split(':')[1].strip("'"))
        tries = int(str(res[3]).split(':')[1].strip("'"))

        assert diff == i
        result.append((diff, tries))

    return result


def generate_data():
    seeds = [os.urandom(32) for s in range(10)]

    print("[*] Generating data...")
    p = Pool(10)
    result = p.map(calculate, seeds)

    results = {x: 0 for x in range(1,23)}

    for seed in result:
        for pair in seed:
            results[pair[0]] += pair[1]

    for diff in results.keys():
        results[diff] = results[diff]/22

    return results


def generate_graph():
    data = generate_data()

    with open('stats/dataset.txt', 'w') as dataset:
        json.dump(data, dataset)

    means = [x for x in data.values()]
    plt.semilogy([x for x in range(len(means))], means)
    plt.title('Average tries per challenge difficulty')
    plt.xlabel('Difficulty')
    plt.ylabel('Tries')
    plt.savefig('stats/statistics.png')


generate_graph()
