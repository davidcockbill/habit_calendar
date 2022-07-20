# Habit Calendar Build

## Prototype

I originally prototyped this. I built only the first 8 days of January and February:

![prototype](./images/prototype.jpg) 

Note:

In the prototype there is resistor per LED. This is overkill as only one LED will be lit at a time due to the month multiplexing. Therefore for the proper thing I am only going to use 31 resistors rather than 365!

## Strip Board

Bit of a mess, but moved the prototype to strip board:

![prototype](./images/circuit_1.jpg) 


With buttons plugged in:

![prototype](./images/circuit_2.jpg) 

I used sockets for the shift registers and Arduino. This has made the whols thing higher than I expected as I want a low profile frame. We'll see if I ditch these sockets once it is working.

Testing:

![prototype](./images/testing_strip_board.jpg) 

Note that you can see me trying to add numbers (31 in this case) to the panel lenses. I've given up on this as the results were not very good.

## Matrix Build

The LED matrix is 3D printed to provide the appropriate spacing.

I put a cup around each matrix to hopefully prevent light bleed between days.

Due to printer constraints (physical size and the fact that prints kept failing with larger prints), I have split the whole matrix into 4 panels of 12 month x 8 days.

Here is the test of the first panel:

![testing first matrix](./images/testing_first_matrix.jpg) 

## Front Panel

I have ordered a couple of 440(H)x220(W)x3(D) coloured acrylic sheets from [here](https://www.sheetplastics.co.uk), to be the front panel.

This will have 8mm holess drilled to accept the day lenses

### Front Panel Day Lenses

3D printed to insert through the front panel. The lense will be filled with UV hardening resin. This will then be sanded to make the resin more opaque to hopefully difuse the light from a lit day.

Here is the prototype, along with bubble in the resin:

![prototype lens](./images/prototype_lens.jpg) 

Need to get rid of bubbles in the resin for the 365 real versions!
