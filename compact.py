import math
import random

def Deindex(index):
    b = []
    for i in range(9):
        b.append(index % 3)
        index = int(math.floor(index / 3))
    return b

def Index(b):
    index = 0
    for i in reversed(range(9)):
        index *= 3
        index += b[i]
    return index

def DetectWinner(b):
    victories = [[0, 1, 2], [3, 4, 5], [6, 7, 8], [0, 3, 6],
                 [1, 4, 7], [2, 5, 8], [0, 4, 8], [2, 4, 6]]
    for v in victories:
        x, y, z = b[v[0]], b[v[1]], b[v[2]]
        if x != 0 and x == y and x == z:
            return x
    return 0

human_readable = '-XO'

def PrintBoard(b):
    for row in range(3):
        line = ''
        for col in range(3):
            line += human_readable[b[3 * row + col]]
        print line

state_successors = []
state_groups = []
unclassified_states = set()
# Two pre-existing base case groups: win and non-win-game-over.
group_states = [set(), set()]
group_successors = [set(), set()]
for index in range(19683):
    b = Deindex(index)
    successors = set()
    for i in range(9):
        if b[i] == 0:
            b[i] = 1
            successors.add(Index(b))
            b[i] = 2
            successors.add(Index(b))
            b[i] = 0
    state_successors.append(successors)
    winner = DetectWinner(b)
    # Separate the states into the two base case groups or unclassified.
    if winner == 1:
        state_groups.append(0)
        group_states[0].add(index)
    elif not successors or winner == 2:
        state_groups.append(1)
        group_states[1].add(index)
    else:
        state_groups.append(-1)
        unclassified_states.add(index)
while unclassified_states:
    print 'unclassified states:', len(unclassified_states)
    print 'groups:', len(group_successors)
    leftovers = []
    for index in unclassified_states:
        gs = set(state_groups[s] for s in state_successors[index])
        if -1 in gs:
            leftovers.append(index)
            continue
        if gs not in group_successors:
            group_successors.append(gs)
            group_states.append(set())
        group_id = group_successors.index(gs)
        state_groups[index] = group_id
        group_states[group_id].add(index)
    unclassified_states = leftovers
print 'groups:', len(group_successors)
for i, (states, successors) in enumerate(zip(group_states, group_successors)):
    print i, len(states), successors
    for s in states:
        break
    b = Deindex(s)
    PrintBoard(b)
