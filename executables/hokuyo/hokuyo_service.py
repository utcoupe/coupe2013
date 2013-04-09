#!/usr/bin/env python3

import sys
sys.path.append('../../lib')
sys.path.append('.')
import utcoupe
import executable

args = {'executable_path':'driver/hokuyo',
        'executable_options' : '-color,1',
        'identity':utcoupe.HOKUYO,
        'protocol_file':'driver/src/protocole.h',
        'protocol_prefix': 'Q_'}
executable.run(**args);
