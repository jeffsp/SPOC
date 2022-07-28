#pragma once

#include "spoc/spoc.h"
#include <iostream>
#include <sstream>

namespace spoc
{

namespace diff_app
{

inline int diff (const spoc::file::spoc_file &f1,
    const spoc::file::spoc_file &f2,
    const bool header_only = false,
    const bool data_only = false,
    const std::vector<int> &fields = std::vector<int> (),
    const bool reverse = false)
{
    using namespace spoc::io;

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
        if (f1.get_header ().major_version != f2.get_header ().major_version)
            return_code = -1;
        else if (f1.get_header ().minor_version != f2.get_header ().minor_version)
            return_code = -1;
        else if (f1.get_header ().total_points != f2.get_header ().total_points)
            return_code = -1;
        else if (f1.get_header ().wkt != f2.get_header ().wkt)
            return_code = -1;
    }
    if (check_data)
    {
        if (get_x (f1.get_point_records ()) != get_x (f2.get_point_records ()))
            return_code = -1;
        else if (get_y (f1.get_point_records ()) != get_y (f2.get_point_records ()))
            return_code = -1;
        else if (get_z (f1.get_point_records ()) != get_z (f2.get_point_records ()))
            return_code = -1;
        else if (get_c (f1.get_point_records ()) != get_c (f2.get_point_records ()))
            return_code = -1;
        else if (get_p (f1.get_point_records ()) != get_p (f2.get_point_records ()))
            return_code = -1;
        else if (get_i (f1.get_point_records ()) != get_i (f2.get_point_records ()))
            return_code = -1;
        else if (get_r (f1.get_point_records ()) != get_r (f2.get_point_records ()))
            return_code = -1;
        else if (get_g (f1.get_point_records ()) != get_g (f2.get_point_records ()))
            return_code = -1;
        else if (get_b (f1.get_point_records ()) != get_b (f2.get_point_records ()))
            return_code = -1;
        else if (get_extra_fields_size (f1.get_point_records ()) != get_extra_fields_size (f2.get_point_records ()))
            return_code = -1;
        else
        {
            for (size_t k = 0; k < get_extra_fields_size (f1.get_point_records ()); ++k)
            {
                if (get_extra (k, f1.get_point_records ()) != get_extra (k, f2.get_point_records ()))
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
                              if (get_x (f1.get_point_records ()) != get_x (f2.get_point_records ()))
                                  return_code = -1;
                              break;
                          }
                case 'y': {
                              if (get_y (f1.get_point_records ()) != get_y (f2.get_point_records ()))
                                  return_code = -1;
                              break;
                          }
                case 'z': {
                              if (get_z (f1.get_point_records ()) != get_z (f2.get_point_records ()))
                                  return_code = -1;
                              break;
                          }
                case 'c': {
                              if (get_c (f1.get_point_records ()) != get_c (f2.get_point_records ()))
                                  return_code = -1;
                              break;
                          }
                case 'p': {
                              if (get_p (f1.get_point_records ()) != get_p (f2.get_point_records ()))
                                  return_code = -1;
                              break;
                          }
                case 'i': {
                              if (get_i (f1.get_point_records ()) != get_i (f2.get_point_records ()))
                                  return_code = -1;
                              break;
                          }
                case 'r': {
                              if (get_r (f1.get_point_records ()) != get_r (f2.get_point_records ()))
                                  return_code = -1;
                              break;
                          }
                case 'g': {
                              if (get_g (f1.get_point_records ()) != get_g (f2.get_point_records ()))
                                  return_code = -1;
                              break;
                          }
                case 'b': {
                              if (get_b (f1.get_point_records ()) != get_b (f2.get_point_records ()))
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
                              if (index < get_extra_fields_size (f1.get_point_records ())
                                    && index < get_extra_fields_size (f2.get_point_records ())
                                    && (get_extra (index, f1.get_point_records ()) != get_extra (index, f2.get_point_records ())))
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

} // namespace diff_app

} // namespace spoc
