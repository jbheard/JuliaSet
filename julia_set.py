import numpy as np
from os.path import isfile
from random import choices
from PIL import Image
from timeit import default_timer as timer

from julia_set_c import make_julia_set
from colormaps import get_color_map

FPS = 24
SECONDS = 3

WIDTH, HEIGHT = 1000, 1000

SEEDS = [
(-0.76 - 0.24j), (-0.125 + 0.75j), (0.285 + 0.535j), (-0.687 + 0.312j),
(-0.500 + 0.563j), (-0.79 + 0.15j), (0.27 + 0.54j), (-0.36 + 0.64j),
(0.42 + 0.42j), (0.41 - 0.32j), (0.275 + 0.j), (-1 + 0.j), (-1 + 0.25j),
(0 - 1.j)
]
START_SEED, END_SEED = choices(SEEDS, k=2)


def get_seed(i, steps, start=START_SEED, end=END_SEED):
	step = i / steps
	seed = start
	seed += (end.real - start.real)*step + (end.imag - start.imag)*step*1j
	return seed.real, seed.imag

def get_next_filename():
	fileno = 1
	filestr = 'gifs/result-{}.gif'
	while isfile(filestr.format(fileno)):
		fileno += 1
	return filestr.format(fileno)

def make_file(seeds=None, palette=0):
	start = timer()
	
	if seeds:
		s1, s2 = seeds[0], seeds[1]
	else:
		s1, s2 = choices(SEEDS, k=2)
	
	do_the_thing = lambda x: Image.fromarray(make_julia_set(*get_seed(x, FPS * SECONDS, s1, s2), WIDTH, HEIGHT, palette), 'RGB')
	
	img, *imgs = [ do_the_thing(i) for i in range(SECONDS*FPS)]
	imgs.extend( imgs[::-1] )
	
	end = timer()
	print(f"Finished generating images in {end-start:,.2f} seconds")
	start = timer()

	fileno = 1
	filestr = 'gifs/result-{}.gif'
	while isfile(filestr.format(fileno)):
		fileno += 1
	
	with open(filestr.format(fileno), "wb") as file:
		img.save(fp=file, format='GIF', append_images=imgs,
			save_all=True, duration=1000/FPS, loop=0)
	
	end = timer()
	print( f"Finished writing '{filestr.format(fileno)}' in {end-start:,.3f} seconds" )


if __name__ == "__main__":
	run = 0
	runs = 15
	while run < runs:
		make_file(palette=run)
		run += 1
