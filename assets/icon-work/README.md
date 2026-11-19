# sen-oni icon work

`oni-logo_simplified.svg` is the logo as the owner traced it. Two drawings in one HVIF, chosen by the size (level of detail = size / 64): **under about 19 px a 16x16 pixel icon**
(`oni-pixel-16.svg`, the grid in `onipx.py`, needs `shapely`) and **from there the logo**, `oni-flat.svg` (`python3 flat.py oni-logo_simplified.svg oni-flat.svg`: 81 shapes, 128 paths).
`lodtool oni.hvif oni-pixel-16.svg 99 0 0.3 0  oni-flat.svg 0 0.3 4 0.3` (`lodtool.cpp`) joins them; `oni.hvif` is 29 KB, `oni.iom` the same for Icon-O-Matic, `oni-renders-haiku.png` shows how Haiku draws it.

Where it is used: the notification of `sen_oni` (`scripts/oni.sh`, the command `notify`, group SEN) passes `--icon`, which takes a file: `assets/images/oni.png` (64 px, rendered from `oni-flat.svg`; the
package installs it as `data/sen_oni/oni.png`). The MIME-type tool (`sen_mime`) shows nothing itself.
