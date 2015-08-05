import math
import sys

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

def Rotate(b):
    return [b[6], b[3], b[0],
            b[7], b[4], b[1],
            b[8], b[5], b[2]]

def Reflect(b):
    return [b[2], b[1], b[0],
            b[5], b[4], b[3],
            b[8], b[7], b[6]]

def SwitchTeams(b):
    result = []
    for x in b:
        if x == 0:
            result.append(0)
        else:
            result.append(3 - x)
    return result

def MinRotationIndex(b):
    i = Index(b)
    b = Rotate(b)
    i = min(i, Index(b))
    b = Rotate(b)
    i = min(i, Index(b))
    b = Rotate(b)
    return min(i, Index(b))

def MinReflectAndRotateIndex(b):
    return min(MinRotationIndex(b), MinRotationIndex(Reflect(b)))

def CanonicalIndex(b):
    return min(MinReflectAndRotateIndex(b),
               MinReflectAndRotateIndex(SwitchTeams(b)))

victories = [
    [0, 1, 2],
    [3, 4, 5],
    [6, 7, 8],
    [0, 3, 6],
    [1, 4, 7],
    [2, 5, 8],
    [0, 4, 8],
    [2, 4, 6],
]

def IsWinOrDraw(b):
    is_draw = True
    for v in victories:
        counts = [0, 0, 0]
        for i in v:
            counts[b[i]] += 1
        if counts[1] == 3 or counts[2] == 3:
            return True
        if counts[1] == 0 or counts[2] == 0:
            is_draw = False
    return is_draw

def CountDistinctMiniBoards():
    distinct = []
    for index in range(19683):
        b = Deindex(index)
        if CanonicalIndex(b) == index and not IsWinOrDraw(b):
            distinct.append(index)
    print len(distinct)
    print max(distinct)

human_readable = '-XO'

def PrintBoard(b):
    for row in range(3):
        line = ''
        for col in range(3):
            line += human_readable[b[3 * row + col]]
        print line

def PrintBoardList(boards):
    lines = ['', '', '', '']
    for b in boards:
        for row in range(3):
            for col in range(3):
                lines[row] += human_readable[b[3 * row + col]]
            lines[row] += '   '
        lines[3] += str(Index(b)).ljust(6)
    for line in lines:
        print line

index = int(sys.argv[1])
b = Deindex(index)
canon = CanonicalIndex(b)
print 'index:', index, 'canon:', canon
PrintBoard(b)
children = set()
for i in range(9):
    if b[i] == 0:
        b[i] = 1
        if not IsWinOrDraw(b):
            children.add(CanonicalIndex(b))
        b[i] = 2
        if not IsWinOrDraw(b):
            children.add(CanonicalIndex(b))
        b[i] = 0
children_boards = []
for c in children:
    cb = Deindex(c)
    children_boards.append(cb)
print 'children:', len(children)
PrintBoardList(children_boards)
