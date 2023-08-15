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

    precision_t alt_km = alt / 1000.0;
    bfield_info_type bfield_info;

    double rtp[3]; //r (km), theta (co-latitude in radians), phi (longitude in radians)
    double geocentric[3]; // radial distance from center of Earth [RE], angle from north pole [radians],
;                         // azimuthal angle [radians]
    double brtp[3]; // x, y, z essentially (br, btheta, bphi)

    //convert from geodetic (lat, lon, alt) to geocentric
    double deg_lat = (lat / cTWOPI) * 360.0; //convert to degrees
    double deg_lon = (lon / cTWOPI) * 360.0; //convert to degrees
    geod2geoc(deg_lat, deg_lon, alt_km, geocentric);

    //set IGRF input variables: r (geocentric distance, km), theta (co-lat, rad), phi (lon, rad)
    rtp[0] = geocentric[0]; //taking in radial distance from center of Earth
    rtp[1] = (cPI / 2.0) - lat; //co-latitude, given latitude
    rtp[2] = lon;

    //IGRF conversion
    IGRF_compute(rtp, brtp);

    bfield_info.b[0] = brtp[0];
    bfield_info.b[1] = brtp[1];
    bfield_info.b[2] = brtp[2];

    //AACGM conversion
    double aacgm_lat;
    double aacgm_lon;
    AACGM_v2_Convert(deg_lat, deg_lon, alt_km, &aacgm_lat, &aacgm_lon, &geocentric[0], ALLOWTRACE);
    std::cout << deg_lat << ", " << aacgm_lat << ", " << (aacgm_lat / 360.0) * cTWOPI << endl;
    bfield_info.lon = (aacgm_lon / 360.0) * cTWOPI; //convert back to radians
    bfield_info.lat = (aacgm_lat / 360.0) * cTWOPI; //convert back to radians
    
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