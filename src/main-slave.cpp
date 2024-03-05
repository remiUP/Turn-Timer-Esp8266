#include <Arduino.h>
#include <ESPNowW.h>
#include <esp8266wifi.h>
#include "LedStateMachine.h"
#include <Bounce2.h>
uint8_t receiver_mac[] = {0x8c, 0xaa, 0xb5, 0x69, 0x6b, 0x43};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message
{
	bool status;
} struct_message;

// Create a struct_message called myData
struct_message myData;

unsigned long lastTime = 0;
unsigned long timerDelay = 2000; // send readings timer
LedStateMachine ledStateMachine;

Bounce button = Bounce();

bool ledStatus = false;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
	Serial.print("Last Packet Send Status: ");
	if (sendStatus == 0)
	{
		Serial.println("Delivery success");
	}
	else
	{
		Serial.println("Delivery fail");
	}
}

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
	memcpy(&myData, incomingData, sizeof(myData));
	Serial.print("Bytes received: ");
	Serial.println(len);
	Serial.print("Status: ");
	Serial.println(myData.status);
	ledStatus = myData.status;
}

void setup()
{
	// Init Serial Monitor
	Serial.begin(9600);
	pinMode(LED_BUILTIN, OUTPUT);

	// Set device as a Wi-Fi Station
	WiFi.mode(WIFI_STA);
	// print mac address
	Serial.print("MAC Address: ");
	Serial.println(WiFi.macAddress());

	// Init ESP-NOW
	if (esp_now_init() != 0)
	{
		Serial.println("Error initializing ESP-NOW");
		return;
	}
	ledStateMachine.init();

	button.attach(D1, INPUT_PULLUP);
	button.interval(50);

	// Once ESPNow is successfully Init, we will register for Send CB to
	// get the status of Trasnmitted packet
	esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
	esp_now_register_send_cb(OnDataSent);
	esp_now_register_recv_cb(OnDataRecv);

	// Register peer
	Serial.println(esp_now_add_peer(receiver_mac, ESP_NOW_ROLE_COMBO, 1, NULL, 0));
}

void loop()
{

	button.update();
	if (button.fell())
	{
		myData.status = true;
		esp_now_send(receiver_mac, (uint8_t *)&myData, sizeof(myData));
		Serial.println("Button pressed");
	}
	else if (button.rose())
	{
		myData.status = false;
		esp_now_send(receiver_mac, (uint8_t *)&myData, sizeof(myData));
		Serial.println("Button released");
	}

	if (ledStatus)
	{
		ledStateMachine.setState(LedState::ON);
	}
	else
	{
		ledStateMachine.setState(LedState::OFF);
	}

	ledStateMachine.update();
}
