#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"


#define OPTION_NTC          0
#define OPTION_LDR          1
#define OPTION_DHT22        2
#define OPTION_GAS          3
#define OPTION_WIND_INT     4
#define OPTION_WIND_DIR     5 


#define DHTPIN              21      // Pin digital conectado al DHT22
#define DHTTYPE DHT22               // Tipo de sensor DHT22
#define SDA_PIN             23      // PIN datos bus I2C
#define SCL_PIN             22      // PIN reloj bus I2C
#define TEMP_PIN            2       // PIN lectura analógica de temperatura
#define LDR_PIN             4       // PIN lectura analógica de intensidad de iluminación  
#define CALEFACTOR_PIN      0       
#define ENFRIADOR_PIN       15
#define GAS_PIN             34
#define VERT_PIN            35
#define HOR_PIN             32
#define WIND_INTENSITY_PIN  33
#define BUTTON_PIN          26


const float BETA = 3950;              // Beta del NTC
const float SERIES_RESISTOR = 10000;  // Resistencia fija de 10KΩ en el divisor

// Button
const int timeThreshold = 200;
const int intPin = 2;
volatile int ISRCounter = -1;
int counter = 0;
long startTime = 0;

// Caracteristicas LDR
const float GAMMA = 0.7;
const float RL10 = 50;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);   //Inicialización del objeto lcd

int selection = -1;
int analogValueNTC = 0;
float celsius = 0.0;
int analogValueLDR = 0;
float voltage = 0;
float resistance = 0;
float lux = 0;
float humedad = 0;
float temperatura = 0;
int analogValueGAS = 0;
int analogValueWIND = 0;
int analogValueVERT = 0;
int analogValueHOR = 0;
int ledBright = 0;

const int ledCount = 10;    // the number of LEDs in the bar graph
int ledPinsArray[] = {
  27, 14, 12, 13, 9, 7, 8, 17, 18, 5
};   // an array of pin numbers to which LEDs are attached


void setup()
{
  pinMode(CALEFACTOR_PIN, OUTPUT);      // Declaración de pin en modo salida
  pinMode(ENFRIADOR_PIN, OUTPUT);       // Declaración de pin en modo salida
  Wire.begin(SDA_PIN, SCL_PIN);         // Configura pines I2C
  Serial.begin(115200);                 // Configura comunicación serie
  dht.begin();                          // Inicia sensor DHT22
  lcd.begin(16,2);                      // Metodo de inicialización del lcd
  lcd.backlight();                      // Inicio de backligt del lcd
  lcd.setCursor(5, 0);                  // Posicionamiento del cursor en la posición (0,0)
  lcd.print("CLIMATE");                 // Muestra de mensaje
  lcd.setCursor(3, 1);                  // Posicionamiento del cursor en la posición (0,1)
  lcd.print("MONITORING");              // Muestra de mensaje
  digitalWrite(CALEFACTOR_PIN, LOW);    // Inicialización de calefactor en modo OFF
  digitalWrite(ENFRIADOR_PIN, LOW);     // Inicialización de calefactor en modo OFF
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), debounceCount, FALLING);

  // loop over the pin array and set them all to output:
  for (int iLed = 0; iLed < ledCount; iLed++)
  {
    pinMode(ledPinsArray[iLed], OUTPUT);
  }
}

void loop()
{

  luxMonitoring();
  

  if (counter != ISRCounter)
  {
    counter = ISRCounter;
    selection = counter%6;

    switch(selection)
    {
      case OPTION_NTC:
        Serial.println("OPTION_NTC");
        batteryTempMonitoring();
      break;
      case OPTION_LDR:
        Serial.println("OPTION_LDR");
        ldrMonitoring();

      break;
      case OPTION_DHT22:
        Serial.println("OPTION_DHT22");
        dht22Monitoring();
      break;
      
      case OPTION_GAS:
        Serial.println("OPTION_GAS");
        AirQualityMonitoring();
      break;

      case OPTION_WIND_INT:
        Serial.println("OPTION_WIND_INT");
        windIntensityMonitoring();
      break;

      case OPTION_WIND_DIR:
        Serial.println("OPTION_WIND_DIR");
        dirMonitoring();
      break;

      default:
      Serial.println(selection);
      Serial.println("DEFAULT");
    }
  }
}

void debounceCount()
{
  if (millis() - startTime > timeThreshold)
  {
    ISRCounter++;
    startTime = millis();
  }
}

void luxMonitoring()
{
  analogValueLDR = analogRead(LDR_PIN); 
  ledBright = map(analogValueLDR,0,4096,0,ledCount);  //Mapea la salida de 12 bit del ADC entre 0 y ledCount
  // Recorre el array de leds
  for (int iLed = 0; iLed < ledCount; iLed++)
  {
    
    if (iLed < ledBright) {
      digitalWrite(ledPinsArray[iLed], HIGH);
    }
    // Apaga todos los ledsmenores del valor que indica ledBright
    else {
      digitalWrite(ledPinsArray[iLed], LOW);
    }
  }
}

void batteryTempMonitoring()
{
  analogValueNTC = analogRead(TEMP_PIN);              // Lectura analógica del valor de la NTC
  celsius = 1 / (log(1 / (4096. / analogValueNTC - 1)) / BETA + 1.0 / 298.15) - 273.15; // Procesado de la señal para mostrar Grados Celsius

  //Mostrar en el LCD la información de temperatura de la bateria
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Batt.Temp:");
  lcd.setCursor(5, 1);
  lcd.print(celsius);

  // Gestión del uso del calefactor mediante el algoritmo de 3 posiciones sin histeresis pero con zona muerta
  if(celsius <= 23)
  {
    digitalWrite(CALEFACTOR_PIN, HIGH);
    digitalWrite(ENFRIADOR_PIN, LOW);
    Serial.println("HEATING...");
  }
  else if(celsius > 23 && celsius < 27)
  {
    digitalWrite(CALEFACTOR_PIN, LOW);
    digitalWrite(ENFRIADOR_PIN, LOW);
    Serial.println("IDLE");
  }
  else if(celsius >= 27)
  {
    digitalWrite(CALEFACTOR_PIN, LOW);
    digitalWrite(ENFRIADOR_PIN, HIGH);
    Serial.println("COOLING...");
  }
}

void ldrMonitoring()
{
  analogValueLDR = analogRead(LDR_PIN);                                  // Lectura analógica del valor del sensor LDR
  // Procesado de la medida del LDR
  voltage = analogValueLDR * 3.3 / 4095.0;                                // Pasar cuentas a voltios
  resistance = 2000.0 * voltage / (3.3 - voltage);                        // Con resistencia de 2K
  lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1.0 / GAMMA))/10;  // Cálculo de lux (para LDR común)

  //Mostrar en el LCD la información de la iluminación
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Light Intensity:");
  lcd.setCursor(3, 1);
  lcd.print(lux);
  lcd.setCursor(10, 1);
  lcd.print("lux");
}

void dht22Monitoring()
{
  humedad = dht.readHumidity();
  temperatura = dht.readTemperature(); // Celsius
  
  //Mostrar en el LCD la información de temperatura y humedad exterior
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.setCursor(6, 0);
  lcd.print(temperatura);
  lcd.setCursor(11, 0);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Humd:");
  lcd.setCursor(6, 1);
  lcd.print(humedad);
  lcd.setCursor(12, 1);
  lcd.print("%");
}

void AirQualityMonitoring()
{
  analogValueGAS = analogRead(GAS_PIN);   // Lectura analógica del valor del sensor de calidad de aire
  //Mostrar en el LCD la información de calidad de aire
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("AIR QUALITY:");
  lcd.setCursor(5, 1);
  lcd.print(analogValueGAS);
  lcd.setCursor(10, 1);
  lcd.print("ppm");
}

void windIntensityMonitoring()
{
  analogValueWIND = analogRead(WIND_INTENSITY_PIN);   // Lectura analógica del valor de la simulación de intensidad de viento
  //Mostrar en el LCD la información de intensidad del viento
  lcd.clear();
  if(analogValueWIND == 0)
  {
    lcd.setCursor(4,0);
    lcd.print("NO WIND");
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("WIND INT: ");
    lcd.setCursor(10, 0);
    lcd.print(analogValueWIND);
  }

}

void dirMonitoring()
{
  analogValueWIND = analogRead(WIND_INTENSITY_PIN);   // Lectura analógica del valor de la simulación de intensidad de viento
  analogValueVERT = analogRead(VERT_PIN);             // Lectura analógica del valor ed componente vertical dir. viento
  analogValueHOR = analogRead(HOR_PIN);               // Lectura analógica del valor ed componente horizontal dir. viento
  
  lcd.clear();

  if(analogValueWIND != 0)
  {
    lcd.setCursor(1, 0);
    // Analisis de las componentes de la dirección del viento para mostrar usando puntos cardinales
    if(analogValueVERT == 2048 && analogValueHOR == 2048)
    {
      lcd.print("DIR WIND ERROR");
    }
    else if(analogValueVERT == 2048 && analogValueHOR <= 2048)
    {
      lcd.print("DIR. EAST");
    }
    else if(analogValueVERT == 2048 && analogValueHOR >= 2048)
    {
      lcd.print("DIR. WEST");
    }
    else if(analogValueVERT <= 2048 && analogValueHOR == 2048)
    {
      lcd.print("DIR. SOUTH");
    }
    else if(analogValueVERT >= 2048 && analogValueHOR == 2048)
    {
      lcd.print("DIR. NORTH");
    }
    else if(analogValueVERT >= 2048 && analogValueHOR <= 2408)
    {
      lcd.print("DIR. NORTHEAST");
    }
    else if(analogValueVERT >= 2048 && analogValueHOR >= 2048)
    {
      lcd.print("DIR. NORTHWEST");
    }
    else if(analogValueVERT <= 2408 && analogValueHOR >= 2048)
    {
      lcd.print("DIR. SOUTHWEST");
    }
    else if(analogValueVERT <= 2408 && analogValueHOR <= 2408)
    {
      lcd.print("DIR. SOUTHEAST");
    }
  }
  else
  {
    lcd.setCursor(2, 0);
    lcd.print("NO WIND DIR");
  }
}
