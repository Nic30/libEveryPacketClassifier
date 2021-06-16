#!/usr/bin/python3
# -*- coding: utf-8 -*-

from mesonbuild.mesonmain import main
import os
import re
import sys
from mesonbuild.wrap import wrap
root = os.path.join(os.path.dirname(__file__), '..')

sys.argv[0] = '/usr/local/bin/meson'
sys.argv.extend([
    '--internal',
    'regenerate',
    root,
    f'{root}/build/meson.debug.linux.x86_64',
    '--backend',
    'ninja',
])
print(sys.argv)
sys.exit(main())