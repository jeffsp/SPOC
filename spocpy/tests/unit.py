#!/usr/bin/env python3
"""
Spocpy tests
"""

import os
import random
import shutil
import string
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

    def temp_filename(self, length=16):
        fn = ''.join(random.choices(string.ascii_uppercase, k=length))
        return os.path.join(self.temp_dir, fn)

    def test_version(self):
        # print(f'Version {sp.getmajorversion()}.{sp.getminorversion()}')
        # Check python interface against cpp extension
        self.assertTrue(sp_cpp.getmajorversion() >= 0)
        self.assertTrue(sp_cpp.getminorversion() >= 1)
        self.assertTrue(sp_cpp.getmajorversion() == sp.getmajorversion())
        self.assertTrue(sp_cpp.getminorversion() == sp.getminorversion())

    def test_header(self):
        # Try to read a non-existent file
        doesnt_exist = self.temp_filename()
        self.assertRaises(RuntimeError, sp.readheader, doesnt_exist)
        # Read a test file
        h = sp.readheader('../../test_data/lidar/juarez50.spoc')
        # Check its fields
        self.assertTrue(h.major_version == sp.getmajorversion())
        self.assertTrue(h.minor_version == sp.getminorversion())
        self.assertTrue(len(h.wkt) > 0)
        self.assertTrue(h.extra_fields == 1)
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

    def test_spoc_file(self):
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

    def test_spoc_file_io(self):
        # Try to read a non-existent file
        doesnt_exist = self.temp_filename()
        self.assertRaises(RuntimeError, sp.readspocfile, doesnt_exist)
        # Read a file
        f1 = sp.readspocfile('../../test_data/lidar/juarez50.spoc')
        # Write it back out
        fn = self.temp_filename()
        sp.writespocfile(fn, f1)
        # Read it back in
        f2 = sp.readspocfile(fn)
        # They should have the same number of point records
        self.assertTrue(len(f1.getPointRecords()) == len(f2.getPointRecords()))

    def test_spoc_file_create(self):
        # Create a spoc file
        wkt = "test wkt"
        f = sp.SpocFile(wkt, False)
        # Add some records
        prs = []
        for n in range(0, 100):
            r = sp.PointRecord()
            r.x = 0 + 10 * random.random()
            r.y = 10 + 10 * random.random()
            r.z = 20 + 10 * random.random()
            prs.append(r)
        f.setPointRecords(prs)
        # Write it out
        fn = self.temp_filename()
        sp.writespocfile(fn, f)
        # Read it back in
        f2 = sp.readspocfile(fn)
        self.assertTrue(f2.getWKT() == wkt)
        # Check the records
        self.assertTrue(len(f2.getPointRecords()) == 100)
        for r in f2.getPointRecords():
            self.assertTrue(r.x > 0.0)
            self.assertTrue(r.x < 10.0)
            self.assertTrue(r.y > 10.0)
            self.assertTrue(r.y < 20.0)
            self.assertTrue(r.z > 20.0)
            self.assertTrue(r.z < 30.0)

    def test_spoc_file_pandas_helpers(self):
        # Read a file
        f = sp.readspocfile('../../test_data/lidar/juarez50.spoc')
        n = len(f.getPointRecords())
        x = f.getX()
        y = f.getY()
        z = f.getZ()
        c = f.getC()
        p = f.getP()
        i = f.getI()
        r = f.getR()
        g = f.getG()
        b = f.getB()
        self.assertTrue(len(x) == n)
        self.assertTrue(len(y) == n)
        self.assertTrue(len(z) == n)
        self.assertTrue(len(c) == n)
        self.assertTrue(len(p) == n)
        self.assertTrue(len(i) == n)
        self.assertTrue(len(r) == n)
        self.assertTrue(len(g) == n)
        self.assertTrue(len(b) == n)


if __name__ == '__main__':
    unittest.main()
