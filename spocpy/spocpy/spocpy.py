"""
Spocpy SPOC file format support
"""

import spocpy.spocpy_cpp


def getmajorversion():
    """
    Get the SPOC library major version number
    """

    return spocpy.spocpy_cpp.getmajorversion()


def getminorversion():
    """
    Get the SPOC library minor version number
    """

    return spocpy.spocpy_cpp.getminorversion()
