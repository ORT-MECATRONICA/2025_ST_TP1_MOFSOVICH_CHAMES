#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

unsigned long tiempo;
const long intervalo = 300;
int led = 26;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
int limite = 0;

#define SDA_PIN 21
#define DHTPIN 23
#define SCL_PIN 22
int pinbot1 = 34;
int pinbot2 = 35;

#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
int opcion = 0;
#define pantallaajuste 0
#define pantallamedicion 1
#define cambioajuste 2
int prevBot1 = HIGH;
int prevBot2 = HIGH;
sensors_event_t event;
bool estadoanteriorajuste = false;
void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(pinbot1, INPUT);
  pinMode(pinbot2, INPUT);

  dht.begin();

  sensor_t sensor;
  dht.temperature().getSensor(&sensor);

  delayMS = sensor.min_delay / 1000;

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Lectura de temperatura:");
  display.display();
}

void loop() {
  if (event.temperature > limite) {
    digitalWrite(2, HIGH);
  } else digitalWrite(2, LOW);
  unsigned long tiempoahora = millis();
  if (tiempoahora - tiempo >= intervalo) {
    tiempo = tiempoahora;
    dht.temperature().getEvent(&event);

    if (isnan(event.temperature)) {
      display.println("Error leyendo temp");
    }
  }

  int bot1 = digitalRead(pinbot1);
  int bot2 = digitalRead(pinbot2);

  switch (opcion) {
    case pantallaajuste:
      if (bot1 == LOW || bot2 == LOW) {

        opcion = cambioajuste;
      }

      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Limite: ");
      display.print(limite);
      display.println(" C");
      display.setCursor(0, 10);
      display.print("Temp: ");
      display.print(event.temperature);
      display.println(" C");
      display.setCursor(0, 20);
      display.print("Bot1: +");
      display.setCursor(0, 30);
      display.print("Bot2: -");
      display.display();
      break;

    case pantallamedicion:
      
      if(bot1 == HIGH ||bot2 == HIGH){
        if(prevBot2 == LOW &&prevBot1 == LOW){
          opcion = pantallaajuste;
        }
      }
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Temp: ");
      display.print(event.temperature);
      display.println(" C");
      display.setCursor(0, 20);
      display.print("Ambos botones: ajuste");
      display.display();
      break;
    case cambioajuste:

        if (bot1 == HIGH) {

          if (prevBot1 == LOW) {
            limite++;
            opcion = pantallaajuste;
          }
        }
        if (bot2 == HIGH) {
          if (prevBot2 == LOW) {
            limite--;
            opcion = pantallaajuste;
          }
        }
      
      if(bot1 == HIGH ||bot2 == HIGH){
        if(prevBot2 == LOW &&prevBot1 == LOW){
          opcion = pantallamedicion;
        }
      }

      break;
  }
  prevBot1 = bot1;
  prevBot2 = bot2;
}
