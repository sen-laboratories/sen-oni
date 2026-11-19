# sen-oni icon work (not committed)

From `sen-oni-logo.svg` (the owner's trace: 3154 shapes with gradients, a red oni on black). Two drawings in one HVIF: **under about 19 px a 16x16 pixel icon** (`pixelize.py` makes it from the picture:
down-sampled, 7 colours, near-black is transparent) and **from about 19 px a trace** of the picture with 16 colours (`img2svg` of hvif-tools, `oni-trace-16-colours.svg`, 108 shapes), with the black
background taken away. `oni.hvif` (23 KB) is the icon, `oni.iom` the same for Icon-O-Matic. The trace is busy and keeps part of the dark aura of the picture: it needs the hand of the owner.
Where it goes: the script `oni.sh` notifies with the command `notify` (group SEN): its `--icon` option takes a file, so the icon would be a file of the package (not wired yet).
