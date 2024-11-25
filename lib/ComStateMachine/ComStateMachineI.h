#pragma once
#include <ESPNowW.h>
#include <MacAddress.h>

#include "EventBroker.h"
#include "LedStateMachine.h"

enum PayloadType {
  NONE,     // No payload
  CRQST,    // Connection Request
  APP,      // Assign Pipe
  BPRESS,   // Button press
  BLPRESS,  // Button long press
  LEDT,     // Led Toggle
  LEDON,    // Led on
  LEDOFF,   // Led off
  REORDER,  // Reorder light
};

struct Payload {
  PayloadType payloadType;
  uint32_t data;
};

static_assert(sizeof(Payload) < 250,
              "Payload size is too big, must be less than 255 bytes");

struct ReceivedPayload {
  PayloadType payloadType;
  MacAddress macAddress;
  uint32_t data;
};

class ComStateMachineI {
 protected:
  bool payloadAvailable = false;
  Payload payload;
  ReceivedPayload receivedPayload;
  uint16_t previous;
  EventBroker *eventBroker;
  LedStateMachine *ledStateMachine;
  ReceivedPayload getReceivedPayload();

 public:
  virtual void update() = 0;
  virtual void init(LedStateMachine *, EventBroker *) = 0;
  void setPayloadType(PayloadType);
  void setPayloadData(uint32_t);
};