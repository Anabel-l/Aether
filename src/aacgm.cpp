// Copyright 2020, the Aether Development Team (see doc/dev_team.md for members)
// Full license can be found in License.md

#include <cmath>
#include <iostream>

#include "aether.h"
#include "aacgmlib_v2.h"
#include "mlt_v2.h"

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
    /*bfield_info.b[0] = b_env[0];
    bfield_info.b[1] = b_env[1];
    bfield_info.b[2] = b_env[2];
    bfield_info.lon = mlon;
    bfield_info.lat = mlat;*/

  if (DoDebug)
    report.exit(function);

  return bfield_info;
}