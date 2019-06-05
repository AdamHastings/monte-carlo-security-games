from Agent import Defender, Attacker
import random
import matplotlib.pyplot as plt

num_defenders = 100
num_attackers = 10

num_iterations = 100000
cost_to_attack = 10
loot = 50

Defenders = []
Attackers = []

d_iters = []
a_iters = []

linestyles = ['-', '--', '-.', ':']


def init_game():
    Defenders.clear()
    Attackers.clear()
    d_iters.clear()
    a_iters.clear()

    for i in range(num_defenders):
        x = Defender()
        Defenders.append(x)

    for i in range(num_attackers):
        x = Attacker()
        Attackers.append(x)


def compute_utility():
    defenders_money = 0
    for x in Defenders:
        defenders_money += x.money
    d_iters.append(defenders_money)

    attackers_money = 0
    for x in Attackers:
        attackers_money += x.money
    a_iters.append(attackers_money)

def fight(Defender, Attacker):
    if Defender.protection < Attacker.offenselevel:
        Defender.lose(loot)
        real_loot = Defender.lose(loot)
        Attacker.win(real_loot, cost_to_attack)
    else:
        Attacker.lose(cost_to_attack)
    
    if Defender.get_money() < loot:
        Defenders.remove(Defender)

    if Attacker.get_money() < cost_to_attack:
        Attackers.remove(Attacker)

def run_iterations():
    for i in range(num_iterations):
        fight(random.choice(Defenders), random.choice(Attackers))
        compute_utility()
        if len(Defenders) is 0:
            print("All Defenders dead")
            return i
        if len(Attackers) is 0:
            print("All Attackers dead")
            return i
            


def plot_results(i):
    plt.plot(d_iters, label="defenders, game " + str(i), linewidth=2, linestyle=linestyles[i], color="b")
    plt.plot(a_iters, label="attackers, game " + str(i), linewidth=2, linestyle=linestyles[i], color="r")

num_games = 4

def run_games():
    for i in range(num_games):
        init_game()
        final = run_iterations()
        print("Finished after " + str(final) + " iterations")
        plot_results(i)


def main():
    print("Starting game...")
    run_games()
    plt.xlabel("iterations")
    plt.ylabel("assets")
    plt.legend()
    plt.show()

    
  
if __name__== "__main__":
  main()