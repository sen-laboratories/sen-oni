# sen-oni icon work

`oni-logo_simplified.svg` is the logo as the owner traced it. Two drawings in one HVIF, chosen by the size (level of detail = size / 64): **under about 19 px a 16x16 pixel icon**
(`oni-pixel-16.svg`, the grid in `onipx.py`, needs `shapely`) and **from there the logo**, `oni-flat.svg` (`python3 flat.py oni-logo_simplified.svg oni-flat.svg`: 81 shapes, 128 paths).
`lodtool oni.hvif oni-pixel-16.svg 99 0 0.3 0  oni-flat.svg 0 0.3 4 0.3` (`lodtool.cpp`) joins them; `oni.hvif` is 29 KB, `oni.iom` the same for Icon-O-Matic, `oni-renders-haiku.png` shows how Haiku draws it.

Where it is used: the notification of `sen_oni` (`scripts/oni.sh`, the command `notify`, group SEN) passes `--icon`, which takes a file: `assets/images/oni.png` (64 px, rendered from `oni-flat.svg`; the
package installs it as `data/sen_oni/oni.png`). The MIME-type tool (`sen_mime`) shows nothing itself.

## Institution (`institution.svg`)

`institution.svg` is the drawing of the icon of the entity type Institution (a building with columns, a clock in the pediment, the network of the relations as nodes and lines: 64x64 units, gradients, strokes,
ellipses with a rotation). `svg2hvif.py` reads paths and gradients in the units of the drawing only, so `sento/tools/svg-prepare.py` makes the drawing one that it reads (every shape a path, a stroke a shape of its own,
the gradients of the bounding box ones in the units of the drawing; needs `shapely`):

    python3 sento/tools/svg-prepare.py institution.svg /tmp/institution-flat.svg
    python3 sento/tools/svg2hvif.py /tmp/institution-flat.svg --rdef | grep '^\s*\$"' > ../../schema/icons/x-vnd.sen-labs.entity.institution.hex

65 shapes, 9 KB. One drawing for all sizes: from 32 px it reads well, at 16 px it is the building without the network.

The drawing is by Gregor B. Rosenauer, made with Google Gemini (Neon) as co-author.
