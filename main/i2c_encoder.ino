#include <QMC5883LCompass.h>

QMC5883LCompass compass;

void init_compass( void ){
  // Init 
  compass.init();
  // calibrate 
  compass.setCalibrationOffsets(-367.00, 465.00, -46.00);
  compass.setCalibrationScales(1.08, 0.91, 1.02);
}

int32_t read_compass( void ){
  compass.read();
  // int32_t xyz[3];
  // xyz[0] = 
  // xyz[1] = compass.getY();
  // xyz[2] = compass.getZ();
  return compass.getX();
}
