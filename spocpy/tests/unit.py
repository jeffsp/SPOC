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

    def test_point_record(self):
        pr = sp.PointRecord()
        pr.x = 1.1
        pr.y = 2.2
        pr.z = 2.2
        pr.c = 100
        pr.r = 200
        pr.extra = [5, 4, 3, 2, 1]

    def test_file(self):
        self.assertRaises(RuntimeError, sp.readspocfile, 'does not exist')
        f = sp.readspocfile('../../test_data/lidar/juarez50.spoc')
        self.assertTrue(f.getMajorVersion() == sp.getmajorversion())
        self.assertTrue(f.getMinorVersion() == sp.getminorversion())
        self.assertTrue(len(f.getWKT()) > 0)
        self.assertFalse(f.getCompressed())
        pr10 = f.getPointRecord(10)
        print(pr10)
        prs = f.getPointRecords()
        print(len(prs))


if __name__ == '__main__':
    unittest.main()
