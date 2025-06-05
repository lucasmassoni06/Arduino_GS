#include <DHT.h>
#include <LiquidCrystal_I2C.h>

#define dhtPin 7
#define dhttype DHT22
#define buzzerPin 8
#define rainSensorPin A0

// Pinos dos LEDs
int ledRed = 10;
int ledYellow = 9;
int ledGreen = 11;

// Inicializa o LCD 20x4 I2C
LiquidCrystal_I2C lcd(0x27, 20, 4);

DHT dht(dhtPin, dhttype);

// Controle de alternância
unsigned long lastChange = 0;
const int changeInterval = 5000; // 5 segundos
bool showRain = true;

void setup() {
  lcd.init();
  lcd.backlight();
  dht.begin();
  
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  int humidity = dht.readHumidity();
  int rainIntensity = map(analogRead(rainSensorPin), 0, 1023, 0, 9);
  
  // Alterna as telas a cada 5 segundos
  if (millis() - lastChange > changeInterval) {
    showRain = !showRain;
    lastChange = millis();
    lcd.clear();
  }
  
  if (showRain) {
    displayRainInfo(rainIntensity);
  } else {
    displayHumidityInfo(humidity);
  }
  
  controlLEDs(humidity);
  
  if (rainIntensity >= 7) {
    playAlert();
  }
  
  delay(1000);
}

void displayRainInfo(int intensity) {
  // Linha 1: Título abreviado
  lcd.setCursor(0, 0);
  lcd.print("INFO CHUVA");
  
  // Linha 2: Barra de progresso
  lcd.setCursor(0, 1);
  lcd.print("[");
  int bars = map(intensity, 0, 9, 0, 18);
  for (int i = 0; i < 18; i++) {
    lcd.write(i < bars ? 255 : ' ');
  }
  lcd.print("]");
  
  // Linha 3: Valor numérico
  lcd.setCursor(0, 2);
  lcd.print("Nivel ");
  lcd.print(intensity);
  lcd.print("/9");
  
  // Linha 4: Status direto
  lcd.setCursor(0, 3);
  if (intensity == 0) {
    lcd.print("Sem chuva      ");
  } else if (intensity <= 3) {
    lcd.print("Fraca         ");
  } else if (intensity <= 6) {
    lcd.print("Moderada      ");
  } else {
    lcd.print("FORTE!        ");
  }
}

void displayHumidityInfo(int humidity) {
  // Linha 1: Título abreviado
  lcd.setCursor(0, 0);
  lcd.print("INFO UMIDADE");
  
  // Linha 2: Barra de progresso
  lcd.setCursor(0, 1);
  lcd.print("[");
  int bars = map(humidity, 0, 100, 0, 18);
  for (int i = 0; i < 18; i++) {
    lcd.write(i < bars ? 255 : ' ');
  }
  lcd.print("]");
  
  // Linha 3: Valor numérico
  lcd.setCursor(0, 2);
  lcd.print("Valor ");
  lcd.print(humidity);
  lcd.print("%");
  
  // Linha 4: Status direto
  lcd.setCursor(0, 3);
  if (humidity > 60) {
    lcd.print("ALTA          ");
  } else if (humidity > 20) {
    lcd.print("Normal        ");
  } else {
    lcd.print("BAIXA         ");
  }
}

void controlLEDs(int humidity) {
  digitalWrite(ledRed, LOW);
  digitalWrite(ledYellow, LOW);
  digitalWrite(ledGreen, LOW);
  
  if (humidity > 60) {
    digitalWrite(ledRed, HIGH);
  } else if (humidity > 20) {
    digitalWrite(ledGreen, HIGH);
  } else {
    digitalWrite(ledYellow, HIGH);
  }
}

void playAlert() {
  tone(buzzerPin, 262, 250);
  delay(250);
  tone(buzzerPin, 2093, 400);
  delay(400);
  noTone(buzzerPin);
}