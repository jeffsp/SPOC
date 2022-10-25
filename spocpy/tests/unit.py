#!/usr/bin/env python3
"""
Spocpy tests
"""

import os
import shutil
import tempfile
import unittest
import spocpy.spocpy_cpp as sp_cpp
import spocpy as sp


class TestAll(unittest.TestCase):

    def setUp(self):
        # Create a temp directory
        self.temp_dir = tempfile.mkdtemp()

    def tearDown(self):
        # Clean up
        shutil.rmtree(self.temp_dir)

    def test_version(self):
        # print(f'Version {sp.getmajorversion()}.{sp.getminorversion()}')
        # Check python interface against cpp extension
        self.assertTrue(sp_cpp.getmajorversion() >= 0)
        self.assertTrue(sp_cpp.getminorversion() >= 1)
        self.assertTrue(sp_cpp.getmajorversion() == sp.getmajorversion())
        self.assertTrue(sp_cpp.getminorversion() == sp.getminorversion())

    def test_header(self):
        # Try to read a non-existent file
        self.assertRaises(RuntimeError, sp.readheader, 'does not exist')
        # Read a test file
        h = sp.readheader('../../test_data/lidar/juarez50.spoc')
        # Check its fields
        self.assertTrue(h.major_version == sp.getmajorversion())
        self.assertTrue(h.minor_version == sp.getminorversion())
        self.assertTrue(len(h.wkt) > 0)
        self.assertTrue(h.extra_fields == 0)
        self.assertTrue(h.total_points > 1)
        self.assertFalse(h.compressed)

    def test_point_record(self):
        # Create an empty record
        pr = sp.PointRecord()
        # Change some fields
        pr.x = 1.1
        pr.y = 2.2
        pr.z = 3.3
        pr.c = 100
        pr.p = 200
        pr.i = 300
        pr.r = 400
        pr.g = 500
        pr.b = 600
        pr.extra = [5, 4, 3, 2, 1]
        # Check the new values
        self.assertAlmostEqual(pr.x, 1.1)
        self.assertAlmostEqual(pr.y, 2.2)
        self.assertAlmostEqual(pr.z, 3.3)
        self.assertEqual(pr.c, 100)
        self.assertEqual(pr.p, 200)
        self.assertEqual(pr.i, 300)
        self.assertEqual(pr.r, 400)
        self.assertEqual(pr.g, 500)
        self.assertEqual(pr.b, 600)
        self.assertEqual(pr.extra[0], 5)
        self.assertEqual(pr.extra[-1], 1)

    def test_file(self):
        # Read a file
        f = sp.readspocfile('../../test_data/lidar/juarez50.spoc')
        self.assertTrue(f.getMajorVersion() == sp.getmajorversion())
        self.assertTrue(f.getMinorVersion() == sp.getminorversion())
        self.assertTrue(len(f.getWKT()) > 0)
        self.assertFalse(f.getCompressed())
        # Change some fields
        wkt = "test"
        f.setWKT(wkt)
        self.assertTrue(f.getWKT() == wkt)
        f.setCompressed(True)
        self.assertTrue(f.getCompressed())
        pr10 = f.getPointRecord(10)
        prs = f.getPointRecords()
        self.assertFalse(pr10.x == 0.0)
        self.assertTrue(pr10.x == prs[10].x)

    def test_file_io(self):
        self.assertRaises(RuntimeError, sp.readspocfile, 'does not exist')
        # Read a file
        f1 = sp.readspocfile('../../test_data/lidar/juarez50.spoc')
        print(f1)
        # Write it back out
        fn = os.path.join(self.temp_dir, "test_file.spoc")
        print(fn)
        sp.writespocfile(fn, f1)
        # Read it back in
        f2 = sp.readspocfile(fn)
        print(f2)
        # They should be the same
        #self.assertTrue(f1 == f2)


if __name__ == '__main__':
    unittest.main()
