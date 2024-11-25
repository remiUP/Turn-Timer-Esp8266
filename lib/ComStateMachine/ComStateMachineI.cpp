#include "ComStateMachineI.h"

void ComStateMachineI::setPayloadType(PayloadType payloadType) {
  this->payload.payloadType = payloadType;
};
void ComStateMachineI::setPayloadData(uint32_t data) {
  this->payload.data = data;
};

ReceivedPayload ComStateMachineI::getReceivedPayload() {
  if (this->payloadAvailable) {
    this->payloadAvailable = false;
    return this->receivedPayload;
  }
  ReceivedPayload nonePayload;
  nonePayload.payloadType = PayloadType::NONE;
  return nonePayload;
};