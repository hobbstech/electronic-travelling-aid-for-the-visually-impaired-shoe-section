#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define NIGHT_LED 31
#define NIGHT_LDR A0
#define LIGHT_THRESHOLD 300

#define FRONT_TRIG_PIN 25
#define FRONT_ECHO_PIN 24
#define LEFT_TRIG_PIN 23
#define LEFT_ECHO_PIN 22
#define RIGHT_TRIG_PIN 27
#define RIGHT_ECHO_PIN 26
#define LEFT_VIBRATOR_PIN 30
#define RIGHT_VIBRATOR_PIN 28
#define FRONT_VIBRATOR_PIN 29
#define SAFETY_DISTANCE 1.0

#define DHT11_PIN 32
#define DHT_TYPE DHT11

#define FLAME_SENSOR_PIN A2
#define FLAME_THRESHOLD 42

#define GAS_SENSOR_PIN A1
#define GASSES_THRESHOLD 300


DHT_Unified dht(DHT11_PIN, DHT_TYPE);

float temperature;
float humidity;
uint32_t delayMS;

void setup() {
  Serial.begin(9600);
  Serial.println("*********************** Setting Up System ******************************");

  setupPins();
  setupDht11();

  Serial.println("----------------------- Setting up done --------------------------------");
}

void loop() {

  Serial.println("*********************** Loop Start *************************************");

  boolean lightOut = checkLightIntensity();
  turnOnNightLight(lightOut);

  temperature = readDHT11Temprature(dht);
  humidity = readDHT11Humidity(dht);

  float frontDistance = measureFrontDistance();
  if (frontDistance < SAFETY_DISTANCE) {
    Serial.println("---> SAFETY FRONT DANGER!!!");
    vibrateFront();
    turnOffLeftVibrator();
    turnOffRightVibrator();
  } else {
    turnOffFrontVibrator();
  }

  float leftDistance = measureLeftDistance();
  if (leftDistance < SAFETY_DISTANCE) {
    Serial.println("---> SAFETY LEFT DANGER!!!");
    vibrateLeft();
    turnOffFrontVibrator();
    turnOffRightVibrator();
  } else {
    turnOffLeftVibrator();
  }

  float rightDistance = measureRightDistance();
  if (rightDistance < SAFETY_DISTANCE) {
    Serial.println("---> SAFETY RIGHT DANGER!!!");
    vibrateRight();
    turnOffLeftVibrator();
    turnOffFrontVibrator();
  } else {
    turnOffRightVibrator();
  }

  int flameValue = measureFlame();
  boolean flameDetected = flameValue < FLAME_THRESHOLD;
  if(flameDetected){
    Serial.println("---> Flame Detected");
  }

  int gasValue = measureGasses();
  boolean gassesDetected = gasValue > GASSES_THRESHOLD;
  if(flameDetected){
    Serial.println("---> Harmful gas Detected");
  }

  delay(2000);

  Serial.println("----------------------- Loop End ---------------------------------------\n\n");
}

void setupDht11() {
  dht.begin();
  sensor_t sensor;
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
}

void setupPins() {
  Serial.println("---> Setting up Pins");
  pinMode(NIGHT_LED, OUTPUT);
  pinMode(FRONT_TRIG_PIN, OUTPUT);
  pinMode(FRONT_ECHO_PIN, INPUT);
  pinMode(LEFT_TRIG_PIN, OUTPUT);
  pinMode(LEFT_ECHO_PIN, INPUT);
  pinMode(RIGHT_TRIG_PIN, OUTPUT);
  pinMode(RIGHT_ECHO_PIN, INPUT);
  pinMode(LEFT_VIBRATOR_PIN, OUTPUT);
  pinMode(RIGHT_VIBRATOR_PIN, OUTPUT);
  pinMode(FRONT_VIBRATOR_PIN, OUTPUT);
}

boolean checkLightIntensity() {
  int lightReadValue = analogRead(NIGHT_LDR);
  Serial.print("---> LDR readValue : ");
  Serial.println(lightReadValue);
  return lightReadValue > LIGHT_THRESHOLD;
}

void turnOnNightLight(boolean lightOut) {
  digitalWrite(NIGHT_LED, lightOut);
}

float readDHT11Humidity(DHT_Unified dht) {

  sensors_event_t event;
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    return 0;
  }
  float humid =  event.relative_humidity;
  Serial.print("---> Humidity : ");
  Serial.println(humid);
  return humid;

}

float readDHT11Temprature(DHT_Unified dht) {

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    return 0;
  }
  float temp = event.temperature;
  Serial.print("---> Temperature : ");
  Serial.println(temp);
  return temp;

}

double calculate(uint32_t duration) {
  double soundSpeed = 331.4 + (0.606 * temperature);// + (0.0124 * humidity);
  double distance = (soundSpeed * duration) / 1000000;
  return distance;
}

double measureFrontDistance() {
  digitalWrite(FRONT_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(FRONT_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(FRONT_TRIG_PIN, LOW);
  int duration = pulseIn(FRONT_ECHO_PIN, HIGH);
  double distance = calculate(duration);
  Serial.print("---> Front distance : ");
  Serial.println(distance);
  return distance;
}

double measureRightDistance() {
  digitalWrite(RIGHT_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(RIGHT_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(RIGHT_TRIG_PIN, LOW);
  int duration = pulseIn(RIGHT_ECHO_PIN, HIGH);
  double distance = calculate(duration);
  Serial.print("---> Right distance : ");
  Serial.println(distance);
  return distance;
}

double measureLeftDistance() {
  digitalWrite(LEFT_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(LEFT_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(LEFT_TRIG_PIN, LOW);
  int duration = pulseIn(LEFT_ECHO_PIN, HIGH);
  double distance = calculate(duration);
  Serial.print("---> Left distance : ");
  Serial.println(distance);
  return distance;
}

void vibrateFront() {
  digitalWrite(FRONT_VIBRATOR_PIN, HIGH);
}

void turnOffFrontVibrator() {
  digitalWrite(FRONT_VIBRATOR_PIN, LOW);
}

void vibrateRight() {
  digitalWrite(RIGHT_VIBRATOR_PIN, HIGH);
}

void turnOffRightVibrator() {
  digitalWrite(RIGHT_VIBRATOR_PIN, LOW);
}

void vibrateLeft() {
  digitalWrite(LEFT_VIBRATOR_PIN, HIGH);
}

void turnOffLeftVibrator() {
  digitalWrite(LEFT_VIBRATOR_PIN, LOW);
}

int measureFlame(){
  int flameValue = analogRead(FLAME_SENSOR_PIN);
  Serial.print("---> Flame : ");
  Serial.println(flameValue);
  return flameValue;
}

int measureGasses(){
  int gasValue = analogRead(GAS_SENSOR_PIN);
  Serial.print("---> Gas : ");
  Serial.println(gasValue);
  return gasValue;
}
