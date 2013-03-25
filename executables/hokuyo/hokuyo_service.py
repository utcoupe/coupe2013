#!/usr/bin/env python3

import sys
sys.path.append('../../lib')
sys.path.append('.')
import utcoupe
import executable

args = {'executable_path':['driver/hokuyo', '-color', '0', '-port', '/dev/ttyACM0'],
        'identity':utcoupe.HOKUYO,
        'protocol_file':'driver/src/protocole.h'}
executable.run(**args);
