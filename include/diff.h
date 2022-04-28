#pragma once

#include "spoc.h"
#include <iostream>
#include <sstream>

namespace spoc
{

namespace diff
{

inline int diff (const spoc_file &f1, const spoc_file &f2,
    const bool header_only = false,
    const bool data_only = false,
    const std::vector<int> &fields = std::vector<int> (),
    const bool reverse = false)
{
    // Assume it's a success
    int return_code = 0;

    bool check_header = true;
    bool check_data = true;
    bool check_fields = false;

    if (header_only)
    {
        check_header = true;
        check_data = false;
        check_fields = false;
    }
    if (data_only)
    {
        check_header = false;
        check_data = true;
        check_fields = false;
    }
    if (!fields.empty ())
    {
        check_header = false;
        check_data = false;
        check_fields = true;
    }

    if (check_header)
    {
        if (f1.get_major_version () != f2.get_major_version ())
            return_code = -1;
        else if (f1.get_minor_version () != f2.get_minor_version ())
            return_code = -1;
        else if (f1.get_npoints () != f2.get_npoints ())
            return_code = -1;
        else if (f1.get_wkt () != f2.get_wkt ())
            return_code = -1;
    }
    if (check_data)
    {
        if (f1.get_x () != f2.get_x ())
            return_code = -1;
        else if (f1.get_y () != f2.get_y ())
            return_code = -1;
        else if (f1.get_z () != f2.get_z ())
            return_code = -1;
        else if (f1.get_c () != f2.get_c ())
            return_code = -1;
        else if (f1.get_p () != f2.get_p ())
            return_code = -1;
        else if (f1.get_i () != f2.get_i ())
            return_code = -1;
        else if (f1.get_r () != f2.get_r ())
            return_code = -1;
        else if (f1.get_g () != f2.get_g ())
            return_code = -1;
        else if (f1.get_b () != f2.get_b ())
            return_code = -1;
        else if (f1.get_extra () != f2.get_extra ())
            return_code = -1;
    }
    if (check_fields)
    {
        for (int field : fields)
        {
            switch (field)
            {
                default: {
                             std::stringstream ss;
                             ss << "Unknown field specifier: ";
                             ss << static_cast<char> (field);
                             throw std::runtime_error (ss.str ());
                         }
                case 'x': {
                              if (f1.get_x () != f2.get_x ())
                                  return_code = -1;
                              break;
                          }
                case 'y': {
                              if (f1.get_y () != f2.get_y ())
                                  return_code = -1;
                              break;
                          }
                case 'z': {
                              if (f1.get_z () != f2.get_z ())
                                  return_code = -1;
                              break;
                          }
                case 'c': {
                              if (f1.get_c () != f2.get_c ())
                                  return_code = -1;
                              break;
                          }
                case 'p': {
                              if (f1.get_p () != f2.get_p ())
                                  return_code = -1;
                              break;
                          }
                case 'i': {
                              if (f1.get_i () != f2.get_i ())
                                  return_code = -1;
                              break;
                          }
                case 'r': {
                              if (f1.get_r () != f2.get_r ())
                                  return_code = -1;
                              break;
                          }
                case 'g': {
                              if (f1.get_g () != f2.get_g ())
                                  return_code = -1;
                              break;
                          }
                case 'b': {
                              if (f1.get_b () != f2.get_b ())
                                  return_code = -1;
                              break;
                          }
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7': {
                              if (f1.get_extra ()[field - '0'] != f2.get_extra ()[field - '0'])
                                  return_code = -1;
                              break;
                          }
            }

            // If any are not equal, short-circuit
            if (return_code != 0)
                break;
        }
    }

    if (reverse)
        return !return_code;
    else
        return return_code;
}

} // namespace diff

} // namespace spoc
