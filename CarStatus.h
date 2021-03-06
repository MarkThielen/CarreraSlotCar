#pragma once

#include "pb/CarStatusMessage.pb.h"


class CarStatus {

private:
  
  SlotHub::CarStatusMessage *csm;

  unsigned int previous_timer;

 
public:

  CarStatus();
  
  CarStatus(unsigned int car_number);

  CarStatus(CarStatus *carStatus);

  CarStatus(SlotHub::CarStatusMessage *new_csm);

  ~CarStatus();

  void updateCarStatusFromMessage(SlotHub::CarStatusMessage *new_csm);
  


  void updateCarStatus(CarStatus *carStatus);

  unsigned int getPosition();
  void setPosition(unsigned int new_position);

  unsigned int getCarNumber();
  void setCarNumber(unsigned int carno);

  unsigned int getCurrentLapTime();
  void setCurrentLapTime(unsigned int laptime);

  unsigned int getFastestLapTime();
  void setFastestLapTime(unsigned int laptime);

  unsigned int getLaps();
  void setLaps(unsigned int laps);
  void incrementLaps();

  unsigned int getFuelStatus();
  void setFuelStatus(unsigned int fuelStatus);

  unsigned int getPitStops();
  void setPitStops(unsigned stops);
  void incrementPitStops();

  bool getActive();
  void setActive(bool flag);
 
  bool getInPit();
  void setInPit(bool flag);

  unsigned int getCurrentTimer();
  void setCurrentTimer(unsigned int timer);

  void updateTimeAndLapStatistics(unsigned int timer);
  void updatePitStopStatistics(bool currentlyInPits);

  SlotHub::CarStatusMessage *getCarStatusMessage();

 private:

  unsigned int getPreviousTimer();
  void setPreviousTimer(unsigned int timer);

};



