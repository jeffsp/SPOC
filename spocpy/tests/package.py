#!/usr/bin/env python3
"""
Spocpy tests
"""

import unittest
import spocpy as sp


class TestPackage(unittest.TestCase):

    def test_package(self):
        self.assertEqual(sp.__name__, 'spocpy')


if __name__ == '__main__':
    unittest.main()
