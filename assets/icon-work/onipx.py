#!/usr/bin/env python3
# onipx.py <out.svg>: the oni as a 16x16 pixel icon (a grid of characters, one for each colour)
import sys
from shapely.geometry import box
from shapely.ops import unary_union
pal = {'K': '#3a0a10', 'R': '#b3202f', 'r': '#7e1121', 'Y': '#f2b134', 'W': '#f6e9e0', 'B': '#1c0206', 'L': '#e2485a'}
grid = [
"................",
"..K..........K..",
"..KL........LK..",
"..KRK......KRK..",
"...KRKKKKKKRK...",
"...KRRRRRRRRK...",
"..KRKKRRRRKKRK..",
"..KRYYKRRKYYRK..",
"..KRYKRRRRKYRK..",
"..KRRRRKKRRRRK..",
"..KRRWBBBBWRRK..",
"...KRWBBBBWRK...",
"...KRRBWWBRRK...",
"....KRRRRRRK....",
".....KKKKKK.....",
"................",
]
assert len(grid) == 16 and all(len(r) == 16 for r in grid)
cells = {}
for y, row in enumerate(grid):
    for x, c in enumerate(row):
        if c != '.': cells.setdefault(c, []).append(box(x, y, x + 1, y + 1))
def ring(c): return 'M' + ' L'.join('%d %d' % p for p in list(c.coords)[:-1]) + 'Z'
o = ['<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">']
for c in 'RrLKYWB':
    if c in cells:
        u = unary_union(cells[c]); polys = [u] if u.geom_type == 'Polygon' else list(u.geoms)
        d = ''.join(ring(p.exterior) + ''.join(ring(i) for i in p.interiors) for p in polys)
        o.append('<path fill-rule="evenodd" d="%s" fill="%s"/>' % (d, pal[c]))
o.append('</svg>'); open(sys.argv[1], 'w').write('\n'.join(o))
