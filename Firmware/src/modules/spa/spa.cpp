/*  
 *  Description: Algoritimos de posicionamento solar SPA.
 *  Author: Adptado de X.X.X.X por Bruno G. F. Sampaio
 *  Date: 23/07/2024
*/

#include "spa.h"

// SPA instructions
struct datetime_buffer_t    spa_datetime    = { 0 };
struct STLocation           spa_loc         = { 0 };
struct STPosition           spa_pos         = { 0 };
struct STTime               spa_time        = { 0 };

void spa_att_position(void) {
  SolTrack(spa_time, spa_loc, &spa_pos, useDegrees, useNorthEqualsZero, computeRefrEquatorial, computeDistance);
}

void spa_att_location(double latitude, double longitude) {
  spa_loc.latitude = latitude;
  spa_loc.longitude = longitude;
  spa_loc.pressure = 101.1;
  spa_loc.temperature = 300.0;
}

void spa_att_datetime( struct datetime_buffer_t datetime) {
  spa_time.year = datetime.year;
  spa_time.month = datetime.month;
  spa_time.day = datetime.day;
  spa_time.hour = datetime.hour + 3;
  spa_time.minute = datetime.minute;
  spa_time.second = datetime.second;
}

double spa_get_zenith(void) {
  return spa_pos.altitudeRefract;
}

double spa_get_azimuth(void) {
  return spa_pos.azimuthRefract; 
}


void SolTrack(struct STTime time, struct STLocation location, struct STPosition *position, bool useDegrees, bool useNorthEqualsZero, bool computeRefrEquatorial, bool computeDistance) {
  struct STLocation llocation = location;  // Create local variable
  if (useDegrees) {
    llocation.longitude /= R2D;
    llocation.latitude /= R2D;
  }

  llocation.sinLat = sin(llocation.latitude);
  llocation.cosLat = sqrt(1.0 - llocation.sinLat * llocation.sinLat);  // Cosine of a latitude is always positive or zero

  position->julianDay = computeJulianDay(time.year, time.month, time.day, time.hour, time.minute, time.second);
  position->UT = (double)time.hour + (double)time.minute / 60.0 + (double)time.second / 3600.0;
  position->tJD = position->julianDay;
  position->tJC = position->tJD / 36525.0;
  position->tJC2 = position->tJC * position->tJC;

  computeLongitude(computeDistance, position);
  convertEclipticToEquatorial(position->longitude, position->cosObliquity, &position->rightAscension, &position->declination);
  convertEquatorialToHorizontal(llocation, position);

  if (computeRefrEquatorial) convertHorizontalToEquatorial(llocation.sinLat, llocation.cosLat, position->azimuthRefract, position->altitudeRefract, &position->hourAngleRefract, &position->declinationRefract);
  if (useNorthEqualsZero) setNorthToZero(&position->azimuthRefract, &position->hourAngleRefract, computeRefrEquatorial);
  if (useDegrees) convertRadiansToDegrees(&position->longitude, &position->rightAscension, &position->declination, &position->altitude, &position->azimuthRefract, &position->altitudeRefract, &position->hourAngleRefract, &position->declinationRefract, computeRefrEquatorial);
}


double computeJulianDay(uint8_t year, int month, uint8_t day, uint8_t hour, uint8_t minute, double second) {
  if (month <= 2) {
    year -= 1;
    month += 12;
  }
  uint8_t tmp1 = (int)floor(year / 100.0);
  uint8_t tmp2 = 2 - tmp1 + (int)floor(tmp1 / 4.0);
  double dDay = day + hour / 24.0 + minute / 1440.0 + second / 86400.0;
  double JD = floor(365.250 * (year - 2000)) - 50.5 + floor(30.60010 * (month + 1)) + dDay + tmp2;
  return JD;
}


void computeLongitude(int computeDistance, struct STPosition *position) {

  double l0 = fmod(4.895063168 + 628.331966786 * position->tJC + 5.291838e-6 * position->tJC2, TWO_PI);
  double m = fmod(6.240060141 + 628.301955152 * position->tJC - 2.682571e-6 * position->tJC2, TWO_PI);
  double c = fmod((3.34161088e-2 - 8.40725e-5 * position->tJC - 2.443e-7 * position->tJC2) * sin(m) + (3.489437e-4 - 1.76278e-6 * position->tJC) * sin(2 * m), TWO_PI);
  double odot = l0 + c;

  // Nutation, aberration:
  double omg = fmod(2.1824390725 - 33.7570464271 * position->tJC + 3.622256e-5 * position->tJC2, TWO_PI);  // Lon. of Moon's mean ascending node
  double dpsi = -8.338601e-5 * sin(omg);                                                                   // Nutation in longitude
  double dist = 1.0000010178;                                                                              // Mean distance to the Sun in AU
  if (computeDistance) {
    double ecc = 0.016708634 - 0.000042037 * position->tJC - 0.0000001267 * position->tJC2;  // Eccentricity of the Earth's orbit
    double nu = m + c;                                                                       // True anomaly
    dist = dist * (1.0 - ecc * ecc) / (1.0 + ecc * cos(nu));                                 // Geocentric distance of the Sun in AU
  }
  double aber = -9.93087e-5 / dist;  // Aberration

  // Obliquity of the ecliptic and nutation - do this here, since we've already computed many of the ingredients:
  double eps0 = 0.409092804222 - (2.26965525e-4 * position->tJC + 2.86e-9 * position->tJC2);  // Mean obliquity of the ecliptic
  double deps = 4.4615e-5 * cos(omg);                                                         // Nutation in obliquity

  // Save position parameters:
  position->longitude = fmod(odot + aber + dpsi, TWO_PI);  // Apparent geocentric longitude, referred to the true equinox of date
  // while(position->longitude > TWO_PI) position->longitude -= TWO_PI;

  position->distance = dist;                          // Distance (AU)
  position->obliquity = eps0 + deps;                  // True obliquity of the ecliptic
  position->cosObliquity = cos(position->obliquity);  // Need the cosine later on
  position->nutationLon = dpsi;                       // Nutation in longitude
}


void convertEclipticToEquatorial(double longitude, double cosObliquity, double *rightAscension, double *declination) {
  double sinLon = sin(longitude);
  double sinObl = sqrt(1.0 - cosObliquity * cosObliquity);           // Sine of the obliquity of the ecliptic will be positive in the forseeable future
  *rightAscension = STatan2(cosObliquity * sinLon, cos(longitude));  // 0 <= azimuth < 2pi
  *declination = asin(sinObl * sinLon);
}


void convertEquatorialToHorizontal(struct STLocation location, struct STPosition *position) {
  double gmst = 1.75336856 + fmod(0.017202791805 * position->tJD, TWO_PI) + 6.77e-6 * position->tJC2 + (double)(position->UT / R2H);
  position->agst = fmod(gmst + position->nutationLon * position->cosObliquity, TWO_PI);  // Correction for equation of the equinoxes -> apparent Greenwich sidereal time
  double sinAlt = 0.0;

  // Azimuth does not need to be corrected for parallax or refraction, hence store the result in the 'azimuthRefract' variable directly:
  eq2horiz(location.sinLat, location.cosLat, location.longitude, position->rightAscension, position->declination, position->agst, &position->azimuthRefract, &sinAlt);
  double alt = asin(sinAlt);
  double cosAlt = sqrt(1.0 - sinAlt * sinAlt);

  // Correct for parallax:
  alt -= 4.2635e-5 * cosAlt;
  position->altitude = alt;

  // Correct for atmospheric refraction:
  double dalt = 2.967e-4 / tan(alt + 3.1376e-3 / (alt + 8.92e-2));
  dalt *= location.pressure / 101.0 * 283.0 / location.temperature;
  alt += dalt;
  position->altitudeRefract = alt;
}


void eq2horiz(double sinLat, double cosLat, double longitude, double rightAscension, double declination, double agst, double *azimuth, double *sinAlt) {
  double ha = agst + longitude - rightAscension;  // Local Hour Angle
  double sinHa = sin(ha);
  double cosHa = cos(ha);
  double sinDec = sin(declination);
  double cosDec = sqrt(1.0 - sinDec * sinDec);  // Cosine of a declination is always positive or zero
  double tanDec = sinDec / cosDec;

  *azimuth = STatan2(sinHa, cosHa * sinLat - tanDec * cosLat);  // 0 <= azimuth < 2pi
  *sinAlt = sinLat * sinDec + cosLat * cosDec * cosHa;          // Sine of the altitude above the horizon
}

void convertHorizontalToEquatorial(double sinLat, double cosLat, double azimuth, double altitude, double *hourAngle, double *declination) {
  // Multiply used variables:
  double cosAz = cos(azimuth);
  double sinAz = sin(azimuth);  // For symmetry
  double sinAlt = sin(altitude);
  double cosAlt = sqrt(1.0 - sinAlt * sinAlt);  // Cosine of an altitude is always positive or zero
  double tanAlt = sinAlt / cosAlt;
  *hourAngle = STatan2(sinAz, cosAz * sinLat + tanAlt * cosLat);   // Local Hour Angle:  0 <= hourAngle < 2pi
  *declination = asin(sinLat * sinAlt - cosLat * cosAlt * cosAz);  // Declination
}


void setNorthToZero(double *azimuth, double *hourAngle, bool computeRefrEquatorial) {
  *azimuth = *azimuth + PI;
  if (*azimuth > TWO_PI)
    *azimuth -= TWO_PI;
  if (computeRefrEquatorial) {
    *hourAngle = *hourAngle + PI;
    if (*hourAngle > TWO_PI)
      *hourAngle -= TWO_PI;
  }
}


void convertRadiansToDegrees(double *longitude, double *rightAscension, double *declination, double *altitude, double *azimuthRefract, double *altitudeRefract, double *hourAngle, double *declinationRefract, int computeRefrEquatorial) {
  *longitude *= R2D;
  *rightAscension *= R2D;
  *declination *= R2D;
  *altitude *= R2D;
  *azimuthRefract *= R2D;
  *altitudeRefract *= R2D;
  if (computeRefrEquatorial) {
    *hourAngle *= R2D;
    *declinationRefract *= R2D;
  }
}


double STatan2(double y, double x) {
  if (x > 0.0) return atan(y / x);
  else if (x < 0.0)
    if (y >= 0.0) return atan(y / x) + PI;
    else return atan(y / x) - PI;
  else if (y > 0.0) return PI / 2.0;
  else if (y < 0.0) return -PI / 2.0;
  else return 0.0;
}
