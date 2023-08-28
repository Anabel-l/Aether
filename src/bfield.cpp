// Copyright 2020, the Aether Development Team (see doc/dev_team.md for members)
// Full license can be found in License.md

#include "aether.h"
// -----------------------------------------------------------------------------
// This is the head bfield function that determines which bfield
// to get, then gets it.
// 
// Bfield methods:
//      - Dipole: our current method for setting magnetic field 
//      - AACGM: Use Altitude-adjusted corrected geomagnetic coordinates
//               (AACGM) to set the magnetic field data instead of the current
//               method.
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Calculate an altitude adjusted corrected geomagnetic coordinate given the planetary
// characteristics
//
// Variables:
//  - longitude in radians
//  - latitude in radians
//  - altitude in meters
// -----------------------------------------------------------------------------
bfield_info_type get_aacgm(precision_t lon,
                           precision_t lat,
                           precision_t alt,
                           bool DoDebug,
                           Planets planet) {
                  
    std::string function = "aacgm";
    static int iFunction = -1;

    if (DoDebug)
        report.enter(function, iFunction);

    bfield_info_type bfield_info;

    double rtp[3];
    double brtp[3];
    double bxyz[3];
    precision_t bxyz_precise[3];
    precision_t b_env[3];

    //make sure longitude is between -180 & 180 or -pi & pi
    if(lon > cPI)
      lon = cPI - lon;
    if(lon < -cPI)
      lon = lon - cPI; 

    //convert to degrees
    double deg_lat = lat * 180. / cPI;
    double deg_lon = lon * 180. / cPI;

    geod2geoc(deg_lat, deg_lon, alt / 1000., rtp);
    IGRF_compute(rtp, brtp);
    bspcar(rtp[1],rtp[2], brtp, bxyz); 
    for(int i = 0; i < 3; ++i)
      bxyz_precise[i] = bxyz[i];
    transform_vector_xyz_to_env(bxyz_precise, lat, lon, b_env);

    bfield_info.b[0] = b_env[0];
    bfield_info.b[1] = b_env[1];
    bfield_info.b[2] = b_env[2];

    double aacgm_lat;
    double aacgm_lon;
    double r;

    //std::cout << "Lat: " << deg_lat << " Lon: " << deg_lon << " alt: " << alt / 1000.0 << endl;
    int err = AACGM_v2_Convert(deg_lat, deg_lon, alt / 1000.0, &aacgm_lat, &aacgm_lon, &r, G2A|TRACE);
    if(err != 0){
      report.error("AACGM calculation error, most likely near the equator");
      return bfield_info;
    }
    int time[7];
    err = AACGM_v2_GetDateTime(&time[0], &time[1], &time[2], &time[3], &time[4], &time[5], &time[6]);
    if(aacgm_lat != aacgm_lat || aacgm_lon != aacgm_lon || err != 0){
      report.error(("NaN in aacgm calculations."));
      return bfield_info;
    } else {
      aacgm_lon = inv_MLTConvert_v2(time[0], time[1], time[2], time[3], time[4], time[5], aacgm_lon);
    }

    bfield_info.lat = (aacgm_lat * cPI / 180.);
    bfield_info.lon = (aacgm_lon * cPI / 180.);

    //std::cout << "East: " << bfield_info.b[0] << " North: " << bfield_info.b[1] << " Vertical: " << bfield_info.b[2] << " Mlat: " << bfield_info.lat << " Mlon: " << bfield_info.lon << endl;

    if (DoDebug)
        report.exit(function);

    return bfield_info;
}

bfield_info_type get_bfield(precision_t lon,
                            precision_t lat,
                            precision_t alt,
                            bool DoDebug,
                            Planets planet) {

  std::string function = "get_bfield";
  static int iFunction = -1;

  if (DoDebug)
    report.enter(function, iFunction);

  if (lat > cPI / 2) {
    lat = cPI - lat;
    lon = lon + cPI;
  }

  if (lat < -cPI / 2) {
    lat = -(cPI + lat);
    lon = lon + cPI;
  }
  
  if (lon > cTWOPI)
    lon = lon - cTWOPI;
  
  if(lon < 0)
    lon = lon + cTWOPI;

  bfield_info_type bfield_info;

  if (input.get_bfield_type() == "none") {
    bfield_info.b[0] = 0.0;
    bfield_info.b[1] = 0.0;
    bfield_info.b[2] = 0.0;
    bfield_info.lat = lat;
    bfield_info.lon = lon;
  } else if (input.get_bfield_type() == "dipole") {
    bfield_info = get_dipole(lon, lat, alt, DoDebug, planet);
  } else if (input.get_bfield_type() == "aacgm") {
    bfield_info = get_aacgm(lon, lat, alt, DoDebug, planet);
  }

  if (DoDebug)
    report.exit(function);

  return bfield_info;
}

// -----------------------------------------------------------------------------
// This function finds the magnetic pole in either the north or south.
// Return values in radians!
// -----------------------------------------------------------------------------

arma_vec get_magnetic_pole(int IsNorth,
                           Planets planet) {

  arma_vec lonlat(2, fill::zeros);

  if (input.get_bfield_type() == "none") {
    // No magnetic field, so set location to the geo pole:
    lonlat(0) = 0.0;

    if (IsNorth)
      lonlat(1) = cPI / 2.0;

    else
      lonlat(1) = -cPI / 2.0;
  } else if (input.get_bfield_type() == "dipole") {
    // This is an approximation right now, due to the fact that the
    // pole on some planets (including Earth), have an offset, so the
    // pole location should be altitude dependent.  For many planets,
    // this is a very small error, but should be fixed at some point.

    precision_t magnetic_pole_rotation = planet.get_dipole_rotation();
    precision_t magnetic_pole_tilt = planet.get_dipole_tilt();

    if (IsNorth) {
      lonlat(0) = magnetic_pole_rotation;
      lonlat(1) = cPI / 2 - magnetic_pole_tilt;
    } else {
      lonlat(0) = cTWOPI - magnetic_pole_rotation;
      lonlat(1) = -cPI / 2 + magnetic_pole_tilt;
    }
  }

  return lonlat;
}