import sys

def bits(n):
    return ''.join(('*' if n&(1<<i) else '.') for i in range(7,-1,-1))

def unbits(s):
    r = 0
    #print(list(zip(range(7,-1,-1), s)))
    for i,z in zip(range(7,-1,-1), s):
        if z == '*':
            r = r | (1<<i)
    return ('%02x' % r).upper()

def chunks(s, n):
    l = len(s)
    i = 0
    while i < l:
        yield s[i:i+n]
        i += n

in_bitmap = False
for x in sys.stdin:
    if x.startswith(u'BITMAP'):
        in_bitmap = True
    elif x.startswith('ENDCHAR'):
        in_bitmap = False
    elif in_bitmap:
        if x.startswith('X '):
            x = ''.join(unbits(y) for y in chunks(x.split()[1], 8)) + '\n'
        else:
            x = 'X ' + ''.join(bits(int(y,16)) for y in chunks(x[:-1], 2)) + '\n'
    sys.stdout.write(x)


