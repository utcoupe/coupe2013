#!/usr/bin/env python3

import sys
sys.path.append('../zerobot')
sys.path.append('../../lib')
import arduino
import utcoupe

arduino.run(__file__, identity=utcoupe.ASSERV_BIG)
