#include <LowPower.h>

#include <VirtualWire.h>


#define SEND_PIN 10
#define BUTTON_PIN 9
#define NUM_TRIES 5

void setup() {
  // put your setup code here, to run once:
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  vw_set_tx_pin(SEND_PIN);
  vw_setup(2000);       // Bits per sec
  char msg[8] = {'s', 't', 'a', 'r', 't', 'b', 'o', 'w'};
  char msg2[8] = {'s', 't', 'o', 'p', 'b', 'o', 'w', 'l'};
  Serial.write("startsigbowlight");
  for (int i = 0; i < NUM_TRIES; ++i)
  {
    vw_send((uint8_t *)msg, 8);
    Serial.println("startbow");
    vw_wait_tx(); // Wait until the whole message is gone
    delay(200);
  }
  delay(2000);

  for (int i = 0; i < NUM_TRIES; ++i)
  {
    vw_send((uint8_t *)msg2, 8);
    Serial.println("stopbowl");
    vw_wait_tx(); // Wait until the whole message is gone
    delay(200);
  }
  
}

void loop() {
}
