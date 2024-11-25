#pragma once
#include <EspNowW.h>
#include <MacAddress.h>

#include "ComStateMachineI.h"
#include "EventBroker.h"
#include "LedStateMachine.h"

#define MAX_NUMBER_DEVICES 5

enum ComStateMaster {
  STANDBY,
  RADIO_UNRESPONSIVE,
  SETUP_PAIRING,
  WAITING_CRQST,
  SEND_APP,
  SETUP_LISTENING,
  LISTENING,
  CONNECTED,
};

class ComStateMachineMaster : public ComStateMachineI {
 private:
  MacAddress address[MAX_NUMBER_DEVICES];
  ComStateMaster comState = ComStateMaster::STANDBY;
  int getDeviceIndex(MacAddress macAddress);
  EventBroker *eventBroker;

 public:
  void init(LedStateMachine *, EventBroker *);
  void update();
  int send(int deviceId);
  void sendAll();
  void setState(ComStateMaster);
  uint8_t getNumberDevices();
  void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len);
  void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus);
};