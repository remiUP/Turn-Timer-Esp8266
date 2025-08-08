#include "ComStateMachineMaster.h"

void ComStateMachineMaster::init(LedStateMachine *ledStateMachine,
                                 EventBroker *eventBroker) {
  this->ledStateMachine = ledStateMachine;
  this->eventBroker = eventBroker;

  if (esp_now_init() != 0) {
    this->comState = ComStateMaster::RADIO_UNRESPONSIVE;
    Serial.println("Unable to connect radio");
    this->ledStateMachine->setState(LedState::ERROR);
    return;
  }
  Serial.println("Radio connected");
  this->comState = ComStateMaster::SETUP_PAIRING;

  static auto OnRecvCb = [this](uint8_t *mac, uint8_t *incomingData,
                                uint8_t len) {
    this->OnDataRecv(mac, incomingData, len);
  };
  esp_now_register_recv_cb(
      [](uint8_t *mac, uint8_t *incomingData, uint8_t len) {
        OnRecvCb(mac, incomingData, len);
      });

  static auto OnSendCb = [this](uint8_t *mac_addr, uint8_t sendStatus) {
    this->OnDataSent(mac_addr, sendStatus);
  };
  esp_now_register_send_cb([](uint8_t *mac_addr, uint8_t sendStatus) {
    OnSendCb(mac_addr, sendStatus);
  });
}

void ComStateMachineMaster::OnDataRecv(uint8_t *mac, uint8_t *incomingData,
                                       uint8_t len) {
  Payload incomingPayload;
  memcpy(&incomingPayload, incomingData, sizeof(incomingPayload));
  receivedPayload.macAddress.setMacAddress(mac);
  receivedPayload.payloadType = incomingPayload.payloadType;
  receivedPayload.data = incomingPayload.data;
  payloadAvailable = true;
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Status: ");
  Serial.println(payload.payloadType);
  Serial.print("Mac: ");
  Serial.println(receivedPayload.macAddress);
  Serial.print("Data: ");
  Serial.println(payload.data);
}

void ComStateMachineMaster::OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }
}

int ComStateMachineMaster::getDeviceIndex(MacAddress macAddress) {
  for (uint8_t i = 0; i < config::MAX_NUMBER_DEVICES; i++) {
    if (this->address[i] == macAddress) {
      return i;
    }
  }
  return -1;
};

int ComStateMachineMaster::send(int deviceId) {
  int status =
      esp_now_send(address[deviceId], (uint8_t *)&payload, sizeof(payload));
  Serial.println("Sending");
  Serial.print("Sending sendingPayload type :");
  Serial.println(payload.payloadType);
  return status;
};

void ComStateMachineMaster::sendAll() {
  for (uint8_t i = 0; i < config::MAX_NUMBER_DEVICES; i++) {
    Serial.println("Sending");
    Serial.print("Sending payload type :");
    Serial.println(payload.payloadType);
    esp_now_send(address[i], (uint8_t *)&payload, sizeof(payload));
  }
}

void ComStateMachineMaster::setState(ComStateMaster state) {
  this->comState = state;
};

uint8_t ComStateMachineMaster::getNumberDevices() {
  uint8_t count = 0;
  for (auto device : address) {
    if (device.isNull() == false) {
      count++;
    }
  }
  return count;
}

void ComStateMachineMaster::update() {
  switch (comState) {
    case ComStateMaster::STANDBY:
      break;

    case ComStateMaster::SETUP_PAIRING: {
      payload.payloadType = PayloadType::APP;
      comState = ComStateMaster::WAITING_CRQST;
      ledStateMachine->setState(LedState::PAIRING);
      break;
    }
    case ComStateMaster::WAITING_CRQST: {
      Event event = eventBroker->getEvent();
      if (event.eventType == EventType::SHORT && event.deviceId == 0) {
        comState = ComStateMaster::SETUP_LISTENING;
        ledStateMachine->setState(LedState::PAIRED);
        break;
      }
      ReceivedPayload receivedPayload = getReceivedPayload();
      if (receivedPayload.payloadType == CRQST) {
        Serial.println("New connection");
        Serial.print("Mac is  : ");
        Serial.print(receivedPayload.macAddress);
        Serial.println();
        int deviceIndex = getDeviceIndex(receivedPayload.macAddress);
        if (deviceIndex != -1) {
          Serial.println("Device already known");
          comState = ComStateMaster::SEND_APP;
          return;
        } else {
          Serial.println("New device");
        }
        delay(10);
      }
      break;
    }
    case ComStateMaster::SEND_APP: {
      Serial.println("Sending APP");
      payload.payloadType = PayloadType::APP;
      if (esp_now_send(receivedPayload.macAddress, (uint8_t *)&payload,
                       sizeof(payload)) == 0) {
        Serial.println("Successfully sent APP");
        this->address[getNumberDevices()].setMacAddress(
            receivedPayload.macAddress);
        esp_now_add_peer(receivedPayload.macAddress, ESP_NOW_ROLE_COMBO, 1,
                         NULL, 0);
        ledStateMachine->setState(LedState::PAIRED);
      } else {
        Serial.println("Unable to send APP");
        ledStateMachine->setState(LedState::ERROR);
      }
      comState = ComStateMaster::SETUP_PAIRING;
      break;
    }
    case ComStateMaster::SETUP_LISTENING: {
      ledStateMachine->setState(LedState::PAIRED);
      Serial.println("Com setup");
      this->comState = ComStateMaster::LISTENING;
      Serial.println("Listening");
      break;
    }
    case ComStateMaster::LISTENING: {
      receivedPayload = getReceivedPayload();
      int receivedDeviceId = getDeviceIndex(receivedPayload.macAddress);
      if (receivedDeviceId == -1) {
        Serial.println("Unknown mac address");
        break;
      }
      Event event;
      event.deviceId = receivedDeviceId;
      if (payload.payloadType == PayloadType::BPRESS) {
        event.eventType = EventType::SHORT;
        eventBroker->addEvent(event);
      } else if (payload.payloadType == PayloadType::BLPRESS) {
        event.eventType = EventType::LONG;
        eventBroker->addEvent(event);
      } else if (payload.payloadType == PayloadType::REORDER) {
        event.eventType = EventType::DOUBLE;
        eventBroker->addEvent(event);
      }
      break;
    }
    default:
      break;
  }
}