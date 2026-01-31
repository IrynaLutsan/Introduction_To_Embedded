typedef enum class BUTTON {
  NONE,
  UP,
  DOWN,
  RIGHT,
  LEFT,
  CENTER
} button_t;

static button_t get_button_from_sensor (int sensorValue);
static String get_button_name(button_t button);

void setup() {
  Serial.begin(115200, SERIAL_8N1);
}

void loop() {
  int sensorValue = analogRead(A0);
  Serial.print("ADC value: ");
  Serial.println(sensorValue);
  button_t button = get_button_from_sensor(sensorValue);
  Serial.println(get_button_name(button));
  delay(200);
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
