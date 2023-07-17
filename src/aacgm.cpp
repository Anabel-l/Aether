// Copyright 2020, the Aether Development Team (see doc/dev_team.md for members)
// Full license can be found in License.md

#include <cmath>
#include <iostream>

#include "aether.h"
#include "../c_aacgm_v2.6/aacgmlib_v2.h"
#include "../c_aacgm_v2.6/mlt_v2.h"

// -----------------------------------------------------------------------------
// Calculate an altitude adjusted corrected geomagnetic coordinate given the planetary
// characteristics
// -----------------------------------------------------------------------------


bfield_info_type get_aacgm(precision_t lon,
                           precision_t lat,
                           precision_t alt,
                           bool DoDebug,
                           Planets planet,
                           Inputs input,
                           Report &report) {
                            
    std::string function = "aacgm";
    static int iFunction = -1;

    if (DoDebug)
        report.enter(function, iFunction);

    bfield_info_type bfield_info;
    double radius = planet.get_radius(lat);
    

    double aacgm_lat;
    double aacgm_lon;
    AACGM_v2_SetNow();
    AACGM_v2_Convert(lat, lon, alt, &aacgm_lat, &aacgm_lon, &radius, G2A);
    bfield_info.b[0] = aacgm_lon;
    bfield_info.b[1] = aacgm_lat;
    bfield_info.b[2] = alt;

    precision_t b_env[3];  // env = East, North, Vertical

    bfield_info.lon = aacgm_lon;
    bfield_info.lat = aacgm_lat;

    if (DoDebug)
        report.exit(function);

    return bfield_info;
}