#!/usr/bin/env python3
# flat.py <in.svg> <out.svg> [tolerance]: a simple SVG (paths, rect, circle, ellipse, polygon, polyline, line; fill, stroke, opacity, clip-path of a circle, gradients) as plain filled polygons in
# the painter's order: strokes become shapes (buffer of the line), the clip is an intersection, a gradient is the colour at the middle of its stops, adjacent shapes of one colour are one.
import re, sys, math
import xml.etree.ElementTree as ET
from shapely.geometry import Polygon, LineString, Point, MultiPolygon, box
from shapely.ops import unary_union
from shapely import affinity
from shapely.geometry.polygon import orient

src, out = sys.argv[1], sys.argv[2]
TOL = float(sys.argv[3]) if len(sys.argv) > 3 else 0.3
MINAREA = float(sys.argv[4]) if len(sys.argv) > 4 else 0.6      # polygons and holes below this area (in the units of the drawing) are left out
NS = '{http://www.w3.org/2000/svg}'
root = ET.parse(src).getroot()
vb = [float(x) for x in root.get('viewBox').split()]

def strip(tag): return tag.replace(NS, '')
grads = {}
for g in root.iter():
    t = strip(g.tag)
    if t in ('linearGradient', 'radialGradient'):
        stops = [(float(st.get('offset', '0').strip('%')) / (100 if '%' in st.get('offset', '') else 1), st.get('stop-color')) for st in g if strip(st.tag) == 'stop']
        grads[g.get('id')] = stops
def hexcol(c):
    c = c.strip()
    if c.startswith('#') and len(c) == 4: return '#' + ''.join(ch * 2 for ch in c[1:])
    return c
def paint(value):
    if value is None or value == 'none': return None
    m = re.match(r'url\(#([^)]+)\)', value)
    if m:
        stops = grads[m.group(1)]
        mid = min(stops, key=lambda s: abs(s[0] - 0.5))      # (the stop nearest to the middle)
        return hexcol(mid[1])
    return hexcol(value)
clips = {}
for c in root.iter():
    if strip(c.tag) == 'clipPath':
        geoms = []
        for e in c:
            if strip(e.tag) == 'circle':
                geoms.append(Point(float(e.get('cx')), float(e.get('cy'))).buffer(float(e.get('r')), 64))
        clips[c.get('id')] = unary_union(geoms)

def cubic(p0, p1, p2, p3, n=14):
    return [((1-t)**3*p0[0] + 3*(1-t)**2*t*p1[0] + 3*(1-t)*t*t*p2[0] + t**3*p3[0], (1-t)**3*p0[1] + 3*(1-t)**2*t*p1[1] + 3*(1-t)*t*t*p2[1] + t**3*p3[1]) for t in [i / n for i in range(1, n + 1)]]
def quad(p0, p1, p2, n=10):
    return [((1-t)**2*p0[0] + 2*(1-t)*t*p1[0] + t*t*p2[0], (1-t)**2*p0[1] + 2*(1-t)*t*p1[1] + t*t*p2[1]) for t in [i / n for i in range(1, n + 1)]]
def parse_path(d):
    toks = re.findall(r'[MmLlHhVvCcSsQqZz]|-?\d*\.?\d+(?:e-?\d+)?', d)
    subs = []; cur = []; closed = False
    i = 0; cmd = None; x = y = 0.0; sx = sy = 0.0; last_c = None; last_q = None
    def num():
        nonlocal i; v = float(toks[i]); i += 1; return v
    while i < len(toks):
        if re.match(r'[A-Za-z]', toks[i]): cmd = toks[i]; i += 1
        rel = cmd.islower(); C = cmd.upper()
        if C == 'Z':
            if cur: subs.append((cur, True)); cur = []
            x, y = sx, sy; continue
        if C == 'M':
            if cur: subs.append((cur, False))
            nx, ny = num(), num()
            if rel: nx += x; ny += y
            x, y = nx, ny; sx, sy = x, y; cur = [(x, y)]; cmd = 'l' if rel else 'L'; last_c = last_q = None; continue
        if C == 'L':
            nx, ny = num(), num()
            if rel: nx += x; ny += y
            x, y = nx, ny; cur.append((x, y)); last_c = last_q = None
        elif C == 'H':
            nx = num(); x = x + nx if rel else nx; cur.append((x, y)); last_c = last_q = None
        elif C == 'V':
            ny = num(); y = y + ny if rel else ny; cur.append((x, y)); last_c = last_q = None
        elif C == 'C':
            a = [num() for _ in range(6)]
            if rel: a = [a[0]+x, a[1]+y, a[2]+x, a[3]+y, a[4]+x, a[5]+y]
            cur += cubic((x, y), (a[0], a[1]), (a[2], a[3]), (a[4], a[5])); last_c = (a[2], a[3]); x, y = a[4], a[5]; last_q = None
        elif C == 'S':
            a = [num() for _ in range(4)]
            if rel: a = [a[0]+x, a[1]+y, a[2]+x, a[3]+y]
            c1 = (2*x - last_c[0], 2*y - last_c[1]) if last_c else (x, y)
            cur += cubic((x, y), c1, (a[0], a[1]), (a[2], a[3])); last_c = (a[0], a[1]); x, y = a[2], a[3]; last_q = None
        elif C == 'Q':
            a = [num() for _ in range(4)]
            if rel: a = [a[0]+x, a[1]+y, a[2]+x, a[3]+y]
            cur += quad((x, y), (a[0], a[1]), (a[2], a[3])); last_q = (a[0], a[1]); x, y = a[2], a[3]; last_c = None
        else:
            raise SystemExit('unsupported path command ' + cmd)
    if cur: subs.append((cur, False))
    return subs
def fill_geom(subs):
    g = None
    for pts, closed in subs:
        if len(pts) < 3: continue
        p = Polygon(pts).buffer(0)
        g = p if g is None else g.symmetric_difference(p)     # even-odd
    return g
def stroke_geom(subs, width, cap, join):
    parts = []
    for pts, closed in subs:
        if len(pts) < 2: continue
        line = LineString(pts + ([pts[0]] if closed else []))
        parts.append(line.buffer(width / 2, quad_segs=6, cap_style=1 if cap == 'round' else 2, join_style=1 if join == 'round' else 2))
    return unary_union(parts) if parts else None

shapes = []     # (geometry, colour, alpha)
def emit(geom, col, alpha, clip):
    if geom is None or geom.is_empty or col is None: return
    if clip is not None: geom = geom.intersection(clip)
    if geom.is_empty: return
    shapes.append([geom, col, alpha])
def walk(el, inh):
    t = strip(el.tag)
    st = dict(inh)
    for k in ('fill', 'stroke', 'stroke-width', 'stroke-linecap', 'stroke-linejoin', 'clip-path'):
        if el.get(k) is not None: st[k] = el.get(k)
    op = float(el.get('opacity', 1)) * st.get('opacity', 1.0)
    st['opacity'] = op
    if t in ('defs', 'clipPath', 'linearGradient', 'radialGradient', 'title', 'desc'): return
    if t in ('svg', 'g'):
        for c in el: walk(c, st)
        return
    # a shape
    subs = None
    if t == 'path': subs = parse_path(el.get('d'))
    elif t == 'rect':
        x, y, w, h = [float(el.get(k, 0)) for k in ('x', 'y', 'width', 'height')]; rx = float(el.get('rx', 0))
        if rx > 0: g = box(x + rx, y + rx, x + w - rx, y + h - rx).buffer(rx, 16); subs = [(list(g.exterior.coords)[:-1], True)]
        else: subs = [([(x, y), (x + w, y), (x + w, y + h), (x, y + h)], True)]
    elif t == 'circle':
        g = Point(float(el.get('cx')), float(el.get('cy'))).buffer(float(el.get('r')), 48); subs = [(list(g.exterior.coords)[:-1], True)]
    elif t == 'ellipse':
        g = affinity.scale(Point(0, 0).buffer(1, 40), float(el.get('rx')), float(el.get('ry')))
        tr = el.get('transform')
        if tr:
            m = re.match(r'rotate\(([-\d.]+)\s+([-\d.]+)\s+([-\d.]+)\)', tr)
            if m: g = affinity.rotate(g, float(m.group(1)), origin=(0, 0)); 
        g = affinity.translate(g, float(el.get('cx')), float(el.get('cy')))
        if tr and m: g = affinity.rotate(g, 0)
        subs = [(list(g.exterior.coords)[:-1], True)]
    elif t in ('polygon', 'polyline'):
        nums = [float(v) for v in re.findall(r'-?\d*\.?\d+', el.get('points'))]; subs = [(list(zip(nums[0::2], nums[1::2])), t == 'polygon')]
    elif t == 'line':
        subs = [([(float(el.get('x1')), float(el.get('y1'))), (float(el.get('x2')), float(el.get('y2')))], False)]
    else: return
    clip = None
    cp = st.get('clip-path')
    if cp: clip = clips[re.match(r'url\(#([^)]+)\)', cp).group(1)]
    fill = st.get('fill', 'black') if t != 'line' and t != 'polyline' else (el.get('fill') or 'none')
    if t == 'path' and el.get('fill') is None and 'fill' not in inh: fill = st.get('fill', 'black')
    fc = paint(fill); sc = paint(st.get('stroke'))
    if fc is not None: emit(fill_geom(subs), fc, op, clip)
    if sc is not None:
        w = float(st.get('stroke-width', 1))
        emit(stroke_geom(subs, w, st.get('stroke-linecap', 'butt'), st.get('stroke-linejoin', 'miter')), sc, op, clip)
walk(root, {'opacity': 1.0})

# adjacent shapes of one colour are one; the rest in order
merged = []
for g, c, a in shapes:
    if merged and merged[-1][1] == c and abs(merged[-1][2] - a) < 1e-6: merged[-1][0] = merged[-1][0].union(g)
    else: merged.append([g, c, a])
def ring(c): return 'M' + ' L'.join('%.1f %.1f' % p for p in list(c.coords)[:-1]) + 'Z'
o = ['<svg xmlns="http://www.w3.org/2000/svg" width="%g" height="%g" viewBox="%g %g %g %g">' % (vb[2], vb[3], vb[0], vb[1], vb[2], vb[3])]
n = 0; pts = 0
for g, c, a in merged:
    g = g.simplify(TOL, preserve_topology=True)
    polys = [g] if g.geom_type == 'Polygon' else [p for p in getattr(g, 'geoms', []) if p.geom_type == 'Polygon']
    d = ''
    for p in polys:
        p = orient(p, 1.0)      # (the outline counter-clockwise, the holes clockwise: a hole is a hole for the non-zero rule, too)
        if p.area < MINAREA: continue
        d += ring(p.exterior) + ''.join(ring(i) for i in p.interiors if Polygon(i).area >= MINAREA); pts += len(p.exterior.coords)
    if d:
        if a < 0.999:
            # (no transparent colours: a shape with an opacity is blended with the colour of the first shape, the ground)
            g0 = merged[0][1].lstrip('#'); c0 = [int(g0[i:i+2], 16) for i in (0, 2, 4)]; c1 = c.lstrip('#'); c1 = [int(c1[i:i+2], 16) for i in (0, 2, 4)]
            c = '#%02x%02x%02x' % tuple(int(round(a * c1[k] + (1 - a) * c0[k])) for k in range(3)); a = 1.0
        o.append('<path fill-rule="evenodd" d="%s" fill="%s"/>' % (d, c)); n += 1
o.append('</svg>'); open(out, 'w').write('\n'.join(o))
print('shapes', n, 'points', pts)
