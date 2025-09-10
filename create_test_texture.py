#!/usr/bin/env python3
import numpy as np
from PIL import Image

# Create a UV test pattern
width, height = 256, 256
img = np.zeros((height, width, 3), dtype=np.uint8)

# Create UV gradient with grid overlay
for y in range(height):
    for x in range(width):
        # UV gradient
        u = x / (width - 1)
        v = y / (height - 1)
        
        # Base color from UV
        r = int(u * 255)
        g = int(v * 255)
        b = int((1 - u) * 255)
        
        # Add grid lines every 32 pixels
        if x % 32 == 0 or y % 32 == 0:
            # Grid lines are white
            r = g = b = 255
        # Add border
        elif x == 0 or x == width-1 or y == 0 or y == height-1:
            # Border is yellow
            r = g = 255
            b = 0
            
        img[y, x] = [r, g, b]

# Save as PNG
pil_img = Image.fromarray(img)
pil_img.save('assets/textures/uvtest.png')
print("Created uvtest.png")

# Create a simple checkerboard
checker_size = 32
checker_img = np.zeros((256, 256, 3), dtype=np.uint8)
for y in range(256):
    for x in range(256):
        if ((x // checker_size) + (y // checker_size)) % 2 == 0:
            checker_img[y, x] = [255, 255, 255]  # White
        else:
            checker_img[y, x] = [64, 64, 64]     # Dark gray

pil_checker = Image.fromarray(checker_img)
pil_checker.save('assets/textures/checker.png')
print("Created checker.png")