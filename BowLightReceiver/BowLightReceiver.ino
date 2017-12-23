#include <VirtualWire.h>
#include <FastLED.h>
#define WS2812_CHIPSET     WS2811
#define WS2812_COLOR_ORDER GRB
#define WS2812_BRIGHTNESS 255
#define WS2812_NUM_LEDS 8
CRGB leds[WS2812_NUM_LEDS];

#define RX_PIN 10
#define LED_PIN 9
#define SIR_PIN 8
#define TIME_PIN 3
#define GRP_PIN 4
#define BLU_PIN 5

const unsigned long TwoMinutes = 120000;
const unsigned long FourMinutes = 240000;
unsigned long StartTime = 0;
unsigned long TimeToShoot = TwoMinutes;
unsigned long yellowPhase = 30000;
unsigned long CooldownTime = 10000;
unsigned long WarmupTime = 20000;
bool BlueMode = false;
bool GroupMode = false;

enum NormalState_t
{
  NORMAL_STATE_START,
  NORMAL_STATE_IDLE,
  NORMAL_STATE_PRE_SHOOT,
  NORMAL_STATE_SHOOT,
  NORMAL_STATE_SHOOT_PAUSE,
  NORMAL_STATE_SHOOT_GRP2,
  NORMAL_STATE_SHOOT_ENDING_GRP2,
  NORMAL_STATE_SHOOT_ENDING,
  NORMAL_STATE_END_GRP2,
  NORMAL_STATE_END

};
enum NormalState_t state = NORMAL_STATE_START;

char msg[8] = {'s', 't', 'a', 'r', 't', 'b', 'o', 'w'};
char msg2[8] = {'s', 't', 'o', 'p', 'b', 'o', 'w', 'l'};

void setAllLEDs(CRGB color)
{
  for (int i = 0; i < WS2812_NUM_LEDS; ++i)
  {
    leds[i] = color;
  }
  FastLED.show();
}
void setLEDs(CRGB color, uint8_t firstled, uint8_t lastled)
{
  for (int i = firstled; i <= lastled; ++i)
  {
    leds[i] = color;
  }
  FastLED.show();
}
void runningLight(const uint16_t delaytime, const uint8_t times, CRGB Color)
{
  setAllLEDs(CRGB::Black);
  leds[0] = Color;
  FastLED.show();
  delay(delaytime);
  for (uint8_t j = 0; j < times; ++j)
  {
    for (uint8_t i = 1; i < WS2812_NUM_LEDS; ++i)
    {
      leds[i - 1] = CRGB::Black;
      leds[i] = Color;
      FastLED.show();
      delay(delaytime);
    }
    for (uint8_t i = WS2812_NUM_LEDS; i > 0; --i)
    {
      leds[i] = CRGB::Black;
      leds[i - 1] = Color;
      FastLED.show();
      delay(delaytime);
    }
  }
  setAllLEDs(CRGB::Black);
}
void TurnOnAnimation()
{
  const uint16_t delaytime = 50;
  runningLight(delaytime, 1, CRGB::Red);
  runningLight(delaytime, 1, CRGB::Yellow);
  runningLight(delaytime, 1, CRGB::Green);
  runningLight(delaytime, 1, CRGB::Blue);
  if (BlueMode)
    setAllLEDs(CRGB::Blue);
  else
    setAllLEDs(CRGB::Red);
}
void setup() {
  // put your setup code here, to run once:
  pinMode(RX_PIN, INPUT);
  vw_set_rx_pin(RX_PIN);
  vw_setup(2000);
  vw_rx_start();       // Start the receiver PLL running
  FastLED.addLeds<WS2812_CHIPSET, LED_PIN, WS2812_COLOR_ORDER>(leds, WS2812_NUM_LEDS).setCorrection( TypicalSMD5050 );
  FastLED.setBrightness( WS2812_BRIGHTNESS );
  pinMode(TIME_PIN, INPUT_PULLUP);
  pinMode(GRP_PIN, INPUT_PULLUP);
  pinMode(BLU_PIN, INPUT_PULLUP);

  if (digitalRead(TIME_PIN) == HIGH)
  {
    TimeToShoot = TwoMinutes;
    single_beep();
  }
  else
  {
    TimeToShoot = FourMinutes;
    double_beep();
  }

  if (digitalRead(BLU_PIN) == LOW)
    BlueMode = true;

  delay(1000);
  if (digitalRead(GRP_PIN) == LOW)
  {
    GroupMode = true;
    triple_beep();
  }
  TurnOnAnimation();


}
void beep(uint16_t len)
{
  digitalWrite(SIR_PIN, HIGH);
  delay(len);
  digitalWrite(SIR_PIN, LOW);
}
void single_beep()
{
  beep(200);
}
void double_beep()
{
  beep(150);
  delay(150);
  beep(200);
}
void triple_beep()
{
  beep(150);
  delay(150);
  beep(150);
  delay(150);
  beep(250);
}
void stop_beep()
{
  beep(50);
  delay(100);
  beep(50);
  delay(100);
  beep(50);
  delay(100);
  beep(50);
  delay(100);
  beep(100);
}
bool stateMachineMachineNormalMode(const unsigned long currentTime, const unsigned long StartTime, const unsigned long TimeToShoot, const unsigned long yellowPhase, const unsigned long CooldownTime)
{
  static enum NormalState_t state = NORMAL_STATE_START;
  bool StateMachineRunning = false;
  const unsigned long StartTimeGrp2 = StartTime + TimeToShoot + WarmupTime;
  if (StartTime == 0 && !(state == NORMAL_STATE_IDLE || state == NORMAL_STATE_START))
  {
    Serial.println("STOP SHOOTING");
    state = NORMAL_STATE_IDLE;
    stop_beep();
    FastLED.setBrightness( WS2812_BRIGHTNESS );
    if (BlueMode)
      setAllLEDs(CRGB::Blue);
    else
      setAllLEDs(CRGB::Red);

  }
  switch (state)
  {
    case NORMAL_STATE_IDLE:
      if (StartTime > currentTime) //Start time is scheduled
      {
        state = NORMAL_STATE_PRE_SHOOT;
        FastLED.setBrightness( WS2812_BRIGHTNESS );
        setAllLEDs(CRGB::Red);
        double_beep();
      }
      break;
    case NORMAL_STATE_PRE_SHOOT:
      StateMachineRunning = true;
      if (StartTime <= currentTime)
      {
        state = NORMAL_STATE_SHOOT;
        FastLED.setBrightness( WS2812_BRIGHTNESS );
        setAllLEDs(CRGB::Green);
        single_beep();
      }
      break;
    case NORMAL_STATE_SHOOT:
      StateMachineRunning = true;
      if ((StartTime + TimeToShoot - yellowPhase) <= currentTime)
      {

        state = NORMAL_STATE_SHOOT_ENDING;
        FastLED.setBrightness( WS2812_BRIGHTNESS );
        setAllLEDs(CRGB::Yellow);
      }
      break;
    case NORMAL_STATE_SHOOT_ENDING:
      StateMachineRunning = true;
      if ((StartTime + TimeToShoot) <= currentTime)
      {
        FastLED.setBrightness( WS2812_BRIGHTNESS );
        setAllLEDs(CRGB::Red);
        if (GroupMode)
        {
          state = NORMAL_STATE_SHOOT_PAUSE;
          double_beep();
        }
        else
        {
          state = NORMAL_STATE_END;
          triple_beep();
        }
      }
      break;
    case NORMAL_STATE_END:
      if ((StartTime + TimeToShoot + CooldownTime) <= currentTime)
      {
        state = NORMAL_STATE_IDLE;
        FastLED.setBrightness( WS2812_BRIGHTNESS );
        if (BlueMode)
          setAllLEDs(CRGB::Blue);
        else
          setAllLEDs(CRGB::Red);

      }
      break;
    case NORMAL_STATE_SHOOT_PAUSE:
      StateMachineRunning = true;
      if ((StartTime + TimeToShoot + WarmupTime) <= currentTime)
      {
        state = NORMAL_STATE_SHOOT_GRP2;
        FastLED.setBrightness( WS2812_BRIGHTNESS );
        setAllLEDs(CRGB::Green);
        single_beep();
      }
      break;
    case NORMAL_STATE_SHOOT_GRP2:
      StateMachineRunning = true;
      if ((StartTimeGrp2 + TimeToShoot - yellowPhase) <= currentTime)
      {

        state = NORMAL_STATE_SHOOT_ENDING_GRP2;
        FastLED.setBrightness( WS2812_BRIGHTNESS );
        setAllLEDs(CRGB::Yellow);
      }
      break;
    case NORMAL_STATE_SHOOT_ENDING_GRP2:
      StateMachineRunning = true;
      if ((StartTimeGrp2 + TimeToShoot) <= currentTime)
      {
        FastLED.setBrightness( WS2812_BRIGHTNESS );
        setAllLEDs(CRGB::Red);
        state = NORMAL_STATE_END_GRP2;
        triple_beep();
      }
      break;
    case NORMAL_STATE_END_GRP2:
      if ((StartTimeGrp2 + TimeToShoot + CooldownTime) <= currentTime)
      {
        state = NORMAL_STATE_IDLE;
        FastLED.setBrightness( WS2812_BRIGHTNESS );
        if (BlueMode)
          setAllLEDs(CRGB::Blue);
        else
          setAllLEDs(CRGB::Red);

      }
      break;
    default: //NORMAL_STATE_START
      state = NORMAL_STATE_IDLE;
      FastLED.setBrightness( WS2812_BRIGHTNESS );
      if (BlueMode)
        setAllLEDs(CRGB::Blue);
      else
        setAllLEDs(CRGB::Red);
      break;
  }
  return StateMachineRunning;
}
void loop()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    int i;

    // Message with a good checksum received, dump it.
    Serial.print("Got: ");

    for (i = 0; i < buflen; i++)
    {
      Serial.print(buf[i], HEX);
      Serial.print(' ');
    }
    Serial.println();
    bool msgStartValid = true;
    bool msgStopValid = true;
    for (int i = 0; i < 8; ++i)
    {
      if (msg[i] != buf[i])
      {
        Serial.println("InvalidStartMessage");
        msgStartValid = false;
      }
      if (msg2[i] != buf[i])
      {
        Serial.println("InvalidStopMessage");
        msgStopValid = false;
      }
    }
    if (msgStartValid)
    {
      if (!stateMachineMachineNormalMode(millis(), StartTime, TimeToShoot, yellowPhase, CooldownTime))
      {
        StartTime = millis() + WarmupTime;
        Serial.println("START");
        Serial.println(StartTime);
      }
    }
    if (msgStopValid)
    {
      StartTime = 0;
      Serial.println("STOP");
    }
  }
  stateMachineMachineNormalMode(millis(), StartTime, TimeToShoot, yellowPhase, CooldownTime);
  //Serial.print(".");
}
