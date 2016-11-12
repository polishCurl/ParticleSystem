#!/usr/bin/env python

"""
File:         build.py
Author:       Krzysztof Koch  
Date created: 11/11/2016
Last mod:     11/11/2016
Brief: 		  Build script for particle system
"""

import os

bashCommand = "gcc -DMACOSX -framework OpenGL -framework GLUT -framework CoreFoundation particleSystem.c -o particleSystem -lSOIL"
os.system(bashCommand)