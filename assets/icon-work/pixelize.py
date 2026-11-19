#!/usr/bin/env python3
# pixelize.py <in.png> <size> <k> <mode: circle|dark|none> <out.svg>: the picture as a pixel icon: down-sampled (sips), colours quantised to k (weighted k-means), horizontal runs of one colour are rectangles
import sys, subprocess, struct, math, os, tempfile
src, size, K, mode, out = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), sys.argv[4], sys.argv[5]
tmp = tempfile.mkdtemp()
subprocess.run(['sips', '-z', str(size), str(size), src, '--out', tmp + '/s.png'], capture_output=True)
subprocess.run(['sips', '-s', 'format', 'bmp', tmp + '/s.png', '--out', tmp + '/s.bmp'], capture_output=True)
d = open(tmp + '/s.bmp', 'rb').read()
off = struct.unpack_from('<I', d, 10)[0]
w, h, planes, bpp = struct.unpack_from('<iiHH', d, 18)
rowsize = ((w * bpp + 31) // 32) * 4
flip = h > 0
h = abs(h)
px = {}
for y in range(h):
    sy = (h - 1 - y) if flip else y
    row = off + sy * rowsize
    for x in range(w):
        if bpp == 32:
            b, g, r, a = d[row + x * 4: row + x * 4 + 4]
        else:
            b, g, r = d[row + x * 3: row + x * 3 + 3]; a = 255
        px[(x, y)] = (r, g, b, a)
def visible(x, y, c):
    r, g, b, a = c
    if mode == 'circle':
        return (x + 0.5 - w / 2) ** 2 + (y + 0.5 - h / 2) ** 2 <= (w / 2 - 0.3) ** 2
    if mode == 'dark':
        return a > 128 and (r + g + b) > 90
    return a > 128
pix = [(x, y, px[(x, y)][:3]) for y in range(h) for x in range(w) if visible(x, y, px[(x, y)])]
cent = [p[2] for p in pix[::max(1, len(pix) // K)]][:K]
def near(c): return min(range(len(cent)), key=lambda j: sum((c[t] - cent[j][t]) ** 2 for t in range(3)))
for _ in range(25):
    groups = [[] for _ in cent]
    for p in pix: groups[near(p[2])].append(p[2])
    for i, g in enumerate(groups):
        if g: cent[i] = tuple(sum(c[t] for c in g) / len(g) for t in range(3))
grid = {(x, y): near(c) for x, y, c in pix}
hexs = lambda c: '#%02x%02x%02x' % tuple(int(round(v)) for v in c)
from shapely.geometry import box
from shapely.ops import unary_union
cells = {}
for (x, y), k in grid.items(): cells.setdefault(k, []).append(box(x, y, x + 1, y + 1))
def ring(c): return 'M' + ' L'.join('%d %d' % p for p in list(c.coords)[:-1]) + 'Z'
o = ['<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" viewBox="0 0 %d %d">' % (w, h, w, h)]
for k in sorted(cells, key=lambda k: -len(cells[k])):
    u = unary_union(cells[k]); polys = [u] if u.geom_type == 'Polygon' else list(u.geoms)
    d = ''
    for p in polys:
        d += ring(p.exterior) + ''.join(ring(i) for i in p.interiors)
    o.append('<path fill-rule="evenodd" d="%s" fill="%s"/>' % (d, hexs(cent[k])))
o.append('</svg>'); open(out, 'w').write('\n'.join(o))
print(size, 'px:', len(o) - 2, 'colours')
