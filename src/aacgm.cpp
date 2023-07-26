// Copyright 2020, the Aether Development Team (see doc/dev_team.md for members)
// Full license can be found in License.md


#include <cmath>
#include <iostream>
#include <stdio.h>
#include <math.h>

#include "aether.h"

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
    double radius =  planet.get_radius(lat);


    //IGRF
    //IGRF_SetNow();

    double rtp[3]; //r (km), theta (co-latitude in radians), phi (longitude in radians)
    double brtp[3]; // x, y, z essentially (br, btheta, bphi)

    rtp[0] = planet.get_radius(lat) / 1000.0; // converting from meters to kilometers
    rtp[1] = (M_PI / 2.0) - lat; //co-latitude, given latitude
    rtp[2] = lon;

    brtp;

    //IGRF_compute(rtp, brtp);

    bfield_info.b[0] = brtp[0];
    bfield_info.b[1] = brtp[1];
    bfield_info.b[2] = brtp[2];

    precision_t b_env[3];  // env = East, North, Vertical

    bfield_info.lon = lon;
    bfield_info.lat = lat;
    
    if (DoDebug)
        report.exit(function);

    return bfield_info;
}