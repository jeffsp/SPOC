#!/usr/bin/env python3
"""
Spocpy tests
"""

import unittest
import spocpy.spocpy as sp
import spocpy.spocpy_cpp as sp_cpp
import numpy as np


class TestAll(unittest.TestCase):

    def test_version(self):
        print('major', sp_cpp.getmajorversion())
        print('minor', sp_cpp.getminorversion())
        print('major', sp.getmajorversion())
        print('minor', sp.getminorversion())

    def test_header(self):
        pass

    def test_file(self):
        pass


if __name__ == '__main__':
    unittest.main()
