#!/usr/bin/python

import pygame
import pygame.camera
from pygame.locals import *
import time

pygame.init()
pygame.camera.init()

cam = pygame.camera.Camera("/dev/video0",(320,240))
cam.start()

t=time.time()
image = cam.get_image()
print time.time()-t

pygame.image.save(image,"gcap.jpg")
