#!/usr/bin/env python3
"""
Spocpy tests
"""

import unittest
import spocpy.spocpy_cpp as sp_cpp
import spocpy as sp


class TestAll(unittest.TestCase):

    def test_version(self):
        # print(f'Version {sp.getmajorversion()}.{sp.getminorversion()}')
        self.assertTrue(sp_cpp.getmajorversion() >= 0)
        self.assertTrue(sp_cpp.getminorversion() >= 1)
        self.assertTrue(sp_cpp.getmajorversion() == sp.getmajorversion())
        self.assertTrue(sp_cpp.getminorversion() == sp.getminorversion())

    def test_header(self):
        self.assertRaises(RuntimeError, sp.readheader, 'does not exist')
        h = sp.readheader('../../test_data/lidar/juarez50.spoc')
        self.assertTrue(h.major_version == sp.getmajorversion())
        self.assertTrue(h.minor_version == sp.getminorversion())
        self.assertTrue(len(h.wkt) > 0)
        self.assertTrue(h.extra_fields == 0)
        self.assertTrue(h.total_points > 1)
        self.assertFalse(h.compressed)

    def test_file(self):
        pass


if __name__ == '__main__':
    unittest.main()
