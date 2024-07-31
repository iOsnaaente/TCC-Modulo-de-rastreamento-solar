/*  
 *  Description: Algoritimos de posicionamento solar SPA.
 *  Author: Adptado de X.X.X.X por Bruno G. F. Sampaio
 *  Date: 23/07/2024
*/

#ifndef __SPA_H_
#define __SPA_H_

#include "stdio.h"
#include "math.h"


// Constants
#define PI          3.1415926535897932384626433832795
#define MPI         3.14159265358979323846e6            // One Megapi...
#define R2D         57.2957795130823208768              // Radians to degrees conversion factor
#define R2H         3.81971863420548805845              // Radians to hours conversion factor
#define HALF_PI     1.5707963267948966192313216916398
#define TWO_PI      6.283185307179586476925286766559
#define DEG_TO_RAD  0.017453292519943295769236907684886
#define RAD_TO_DEG  57.295779513082320876798154814105
#define EULER       2.718281828459045235360287471352


// Variáveis
const bool computeRefrEquatorial    = false;
const bool useNorthEqualsZero       = true;
const bool computeDistance          = false;
const bool useDegrees               = true;


// Datetime do sistema
struct datetime_buffer_t {
  uint16_t year;
  uint16_t month;
  uint16_t day;
  uint16_t hour;
  uint16_t minute;
  uint16_t second;
  bool dt_sync;
};


// Structs:
struct STRiseSet {
  double riseTime, transitTime, setTime;
  double riseAzimuth, transitAltitude, setAzimuth;
};

struct STTime {
  int year, month, day;
  int hour, minute;
  double second;
};

struct STPosition {
  double julianDay, tJD, tJC, tJC2, UT;
  double longitude, distance;
  double obliquity, cosObliquity, nutationLon;
  double rightAscension, declination, hourAngle, agst;
  double altitude, altitudeRefract, azimuthRefract;
  double hourAngleRefract, declinationRefract;
};

struct STLocation {
  double longitude, latitude;
  double sinLat, cosLat;
  double pressure, temperature;
};


// SPA instructions
struct datetime_buffer_t    spa_datetime    = { 0 };
struct STLocation           spa_loc         = { 0 };
struct STPosition           spa_pos         = { 0 };
struct STTime               spa_time        = { 0 };

static inline void spa_att_position(void);
void spa_att_location(double latitude, double longitude);
void spa_att_datetime( struct datetime_buffer_t datetime);
double spa_get_zenith(void);
double spa_get_azimuth(void);
void SolTrack(struct STTime time, struct STLocation location, struct STPosition *position, bool useDegrees, bool useNorthEqualsZero, bool computeRefrEquatorial, bool computeDistance);
double computeJulianDay(uint8_t year, int month, uint8_t day, uint8_t hour, uint8_t minute, double second);
void computeLongitude(int computeDistance, struct STPosition *position);
void convertEclipticToEquatorial(double longitude, double cosObliquity, double *rightAscension, double *declination);
void convertEquatorialToHorizontal(struct STLocation location, struct STPosition *position);
void eq2horiz(double sinLat, double cosLat, double longitude, double rightAscension, double declination, double agst, double *azimuth, double *sinAlt);
void convertHorizontalToEquatorial(double sinLat, double cosLat, double azimuth, double altitude, double *hourAngle, double *declination);
void setNorthToZero(double *azimuth, double *hourAngle, bool computeRefrEquatorial);
void convertRadiansToDegrees(double *longitude, double *rightAscension, double *declination, double *altitude, double *azimuthRefract, double *altitudeRefract, double *hourAngle, double *declinationRefract, int computeRefrEquatorial);
double STatan2(double y, double x);


#endif // __SPA_H_ 