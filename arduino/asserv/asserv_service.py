#!/usr/bin/env python3

import sys
sys.path.append('../zerobot')
import arduino

arduino.run(__file__, identity="gros_asserv")
