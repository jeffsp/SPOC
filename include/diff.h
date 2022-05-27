#pragma once

#include "spoc.h"
#include <iostream>
#include <sstream>

namespace spoc
{

namespace diff
{

inline int diff (const spoc_file &f1,
    const spoc_file &f2,
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
        if (f1.h.major_version != f2.h.major_version)
            return_code = -1;
        else if (f1.h.minor_version != f2.h.minor_version)
            return_code = -1;
        else if (f1.h.total_points != f2.h.total_points)
            return_code = -1;
        else if (f1.h.wkt != f2.h.wkt)
            return_code = -1;
    }
    if (check_data)
    {
        if (get_x (f1.p) != get_x (f2.p))
            return_code = -1;
        else if (get_y (f1.p) != get_y (f2.p))
            return_code = -1;
        else if (get_z (f1.p) != get_z (f2.p))
            return_code = -1;
        else if (get_c (f1.p) != get_c (f2.p))
            return_code = -1;
        else if (get_p (f1.p) != get_p (f2.p))
            return_code = -1;
        else if (get_i (f1.p) != get_i (f2.p))
            return_code = -1;
        else if (get_r (f1.p) != get_r (f2.p))
            return_code = -1;
        else if (get_g (f1.p) != get_g (f2.p))
            return_code = -1;
        else if (get_b (f1.p) != get_b (f2.p))
            return_code = -1;
        else if (get_extra_size (f1.p) != get_extra_size (f2.p))
            return_code = -1;
        else
        {
            for (size_t k = 0; k < get_extra_size (f1.p); ++k)
            {
                if (get_extra (k, f1.p) != get_extra (k, f2.p))
                    return_code = -1;
            }
        }
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
                              if (get_x (f1.p) != get_x (f2.p))
                                  return_code = -1;
                              break;
                          }
                case 'y': {
                              if (get_y (f1.p) != get_y (f2.p))
                                  return_code = -1;
                              break;
                          }
                case 'z': {
                              if (get_z (f1.p) != get_z (f2.p))
                                  return_code = -1;
                              break;
                          }
                case 'c': {
                              if (get_c (f1.p) != get_c (f2.p))
                                  return_code = -1;
                              break;
                          }
                case 'p': {
                              if (get_p (f1.p) != get_p (f2.p))
                                  return_code = -1;
                              break;
                          }
                case 'i': {
                              if (get_i (f1.p) != get_i (f2.p))
                                  return_code = -1;
                              break;
                          }
                case 'r': {
                              if (get_r (f1.p) != get_r (f2.p))
                                  return_code = -1;
                              break;
                          }
                case 'g': {
                              if (get_g (f1.p) != get_g (f2.p))
                                  return_code = -1;
                              break;
                          }
                case 'b': {
                              if (get_b (f1.p) != get_b (f2.p))
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
                case '7':
                case '8':
                case '9': {
                              const size_t index = field - '0';
                              if (index < get_extra_size (f1.p)
                                    && index < get_extra_size (f2.p)
                                    && (get_extra (index, f1.p) != get_extra (index, f2.p)))
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
