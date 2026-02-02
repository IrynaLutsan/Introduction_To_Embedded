#include <LiquidCrystal.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>
#include "finit_state_machine.h"

typedef enum class BUTTON {
  NONE,
  UP,
  DOWN,
  RIGHT,
  LEFT,
  CENTER
} button_t;

enum CLOCK_FSM_SIGNALS {
  BUTTON_UP_SIG = FSM_USER_SIG,
  BUTTON_DOWN_SIG,
  BUTTON_RIGHT_SIG,
  BUTTON_LEFT_SIG,
  BUTTON_CENTER_SIG,
  TIME_TICK_SIG
};

typedef struct clock_fsm_tag {
  FSM super;
} clock_fsm_t;

static void clock_ctor(clock_fsm_t *clock);
static FSM_State clock_main(clock_fsm_t *me, FSM_Event const *e);
static FSM_State clock_setup(clock_fsm_t *me, FSM_Event const *e);
static FSM_State stop_watch(clock_fsm_t *me, FSM_Event const *e);
static FSM_State timer(clock_fsm_t *me, FSM_Event const *e);
static FSM_State alarm_clock(clock_fsm_t *me, FSM_Event const *e);

static button_t get_button_from_sensor (int sensorValue);
static String get_button_name(button_t button);

LiquidCrystal lcd (12,11,5,4,3,2);

int buzzerPin = 8;

ThreeWire myWire(7, 6, 9); 
RtcDS1302<ThreeWire> Rtc(myWire);
clock_fsm_t clock_fsm;

void setup() {
  Serial.begin(115200, SERIAL_8N1);
  lcd.begin(16,2);
  pinMode(buzzerPin,OUTPUT);

  Rtc.Begin();

  // Check if the RTC is write-protected
  if (Rtc.GetIsWriteProtected()) {
      Rtc.SetIsWriteProtected(false);
  }

  // Check if the RTC is running
  if (!Rtc.GetIsRunning()) {
      Rtc.SetIsRunning(true);
  }
  clock_ctor(&clock_fsm);
  FSM_init((FSM*)&clock_fsm);
  // SET THE TIME: This line sets the RTC to the date/time the sketch was compiled
  // Run this ONCE, then comment it out and re-upload so you don't reset time every reboot.
  //RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  //Rtc.SetDateTime(compiled);
}

void loop() {
  FSM_Event event;
  delay(50);
  event.sig = TIME_TICK_SIG;
  FSM_dispatch((FSM*)&clock_fsm, &event);
  static button_t last_pressed_button = BUTTON::NONE;
  int sensorValue = analogRead(A0);
  button_t button = get_button_from_sensor(sensorValue);
  if ((button == BUTTON::NONE) && (last_pressed_button != BUTTON::NONE)){
      switch(last_pressed_button)
      {
        case BUTTON::DOWN:
        event.sig = BUTTON_DOWN_SIG;
        break;
        case BUTTON::UP:
        event.sig = BUTTON_UP_SIG;
        break;
        case BUTTON::RIGHT:
        event.sig = BUTTON_RIGHT_SIG;
        break;
        case BUTTON::LEFT:
        event.sig = BUTTON_LEFT_SIG;
        break;
        case BUTTON::CENTER:
        event.sig = BUTTON_CENTER_SIG;
        break;
      }
      FSM_dispatch((FSM*)&clock_fsm, &event);
  }
  last_pressed_button = button;
}

static button_t get_button_from_sensor (int sensorValue){
  if (sensorValue == 0){
    return BUTTON::DOWN;
  }else if (sensorValue < 500) {
    return BUTTON::RIGHT;
  }else if (sensorValue < 700) {
    return BUTTON::UP;
  }else if (sensorValue < 800) {
    return BUTTON::LEFT;
  }else if (sensorValue < 1000) {
  return BUTTON::CENTER;
  }else{
    return BUTTON::NONE;
  }
}

static String get_button_name(button_t button){
  switch(button){
    case BUTTON::DOWN:
    return "Down";
    case BUTTON::UP:
    return "Up";
    case BUTTON::RIGHT:
    return "Right";
    case BUTTON::LEFT:
    return "Left";
    case BUTTON::CENTER:
    return "Center";
    case BUTTON::NONE:
    return "None";
  }
}

static void clock_ctor(clock_fsm_t *clock){
  FSM_ctor (&clock -> super, (FSM_StateHandler)&clock_main);
}

static FSM_State clock_main(clock_fsm_t *me, FSM_Event const *e){
  switch (e->sig){
    case FSM_ENTRY_SIG:
    break;
    case FSM_EXIT_SIG:
    lcd.clear();
    break;
    case BUTTON_UP_SIG:
    return FSM_TRAN(stop_watch);
    break;
    case BUTTON_DOWN_SIG:
    return FSM_TRAN(alarm_clock);
    break;
    case BUTTON_RIGHT_SIG:
    break;
    case BUTTON_LEFT_SIG:
    break;
    case BUTTON_CENTER_SIG:
    return FSM_TRAN(clock_setup);
    break;
    case TIME_TICK_SIG:
    {
      RtcDateTime now = Rtc.GetDateTime();

      char datestring[20];
      snprintf_P(datestring, 
              countof(datestring),
              PSTR("DATE: %02u.%02u.%04u"),
              now.Month(), now.Day(), now.Year());
      lcd.setCursor(0, 0);
      lcd.print(datestring);

      snprintf_P(datestring, 
              countof(datestring),
              PSTR("TIME: %02u:%02u:%02u"),
              now.Hour(), now.Minute(), now.Second());
      lcd.setCursor(0, 1);
      lcd.print(datestring);
    }
    break;
  }
  return FSM_IGNORED();
}

static FSM_State clock_setup(clock_fsm_t *me, FSM_Event const *e){
  switch (e->sig){
    case FSM_ENTRY_SIG:
    lcd.setCursor(0, 0);
    lcd.print("CLOCK SETUP     ");
    break;
    case FSM_EXIT_SIG:
    lcd.clear();
    break;
    case BUTTON_UP_SIG:
    break;
    case BUTTON_DOWN_SIG:
    break;
    case BUTTON_RIGHT_SIG:
    break;
    case BUTTON_LEFT_SIG:
    break;
    case BUTTON_CENTER_SIG:
    return FSM_TRAN(clock_main);
    break;
    case TIME_TICK_SIG:
    break;
  }
  return FSM_IGNORED();
}

static FSM_State alarm_clock(clock_fsm_t *me, FSM_Event const *e){
  switch (e->sig){
    case FSM_ENTRY_SIG:
    lcd.setCursor(0, 0);
    lcd.print("ALARM CLOCK   ");
    break;
    case FSM_EXIT_SIG:
    lcd.clear();
    break;
    case BUTTON_UP_SIG:
    return FSM_TRAN(clock_main);
    break;
    case BUTTON_DOWN_SIG:
    return FSM_TRAN(timer);
    break;
    case BUTTON_RIGHT_SIG:
    break;
    case BUTTON_LEFT_SIG:
    break;
    case BUTTON_CENTER_SIG:
    
    break;
    case TIME_TICK_SIG:
    break;
  }
  return FSM_IGNORED();
}

static FSM_State timer(clock_fsm_t *me, FSM_Event const *e){
  switch (e->sig){
    case FSM_ENTRY_SIG:
    lcd.setCursor(0, 0);
    lcd.print("TIMER            ");
    break;
    case FSM_EXIT_SIG:
    lcd.clear();
    break;
    case BUTTON_UP_SIG:
    return FSM_TRAN(alarm_clock);
    break;
    case BUTTON_DOWN_SIG:
    return FSM_TRAN(stop_watch);
    break;
    case BUTTON_RIGHT_SIG:
    break;
    case BUTTON_LEFT_SIG:
    break;
    case BUTTON_CENTER_SIG:
    
    break;
    case TIME_TICK_SIG:
    break;
  }
  return FSM_IGNORED();
}

static FSM_State stop_watch(clock_fsm_t *me, FSM_Event const *e){
  switch (e->sig){
    case FSM_ENTRY_SIG:
    lcd.setCursor(0, 0);
    lcd.print("STOP WATCH     ");
    break;
    case FSM_EXIT_SIG:
    lcd.clear();
    break;
    case BUTTON_UP_SIG:
    return FSM_TRAN(timer);
    break;
    case BUTTON_DOWN_SIG:
    return FSM_TRAN(clock_main);
    break;
    case BUTTON_RIGHT_SIG:
    break;
    case BUTTON_LEFT_SIG:
    break;
    case BUTTON_CENTER_SIG:
    
    break;
    case TIME_TICK_SIG:
    break;
  }
  return FSM_IGNORED();
}



