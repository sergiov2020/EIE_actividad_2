#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTPIN 21       // Pin digital conectado al DHT22
#define DHTTYPE DHT22   // Tipo de sensor DHT22
#define SDA_PIN 23
#define SCL_PIN 22
#define TEMP_PIN 2
#define LDR_PIN 4
#define CALEFACTOR_PIN 0
#define GAS_PIN 34
#define VERT_PIN 35
#define HOR_PIN 32
#define WIND_INTENSITY_PIN 33

const float BETA = 3950;              // Beta del NTC
const float SERIES_RESISTOR = 10000;  // Resistencia fija de 10KΩ en el divisor


// Caracteristicas LDR
const float GAMMA = 0.7;
const float RL10 = 50;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);   //Inicialización del objeto lcd

void setup()
{
  pinMode(CALEFACTOR_PIN, OUTPUT);    // Declaración de pin en modo salida
  Wire.begin(SDA_PIN, SCL_PIN);       // Configura pines I2C
  Serial.begin(115200);               // Configura comunicación serie
  dht.begin();                        // Inicia sensor DHT22
  lcd.begin(16,2);                    // Metodo de inicialización del lcd
  lcd.backlight();                    // Inicio de backligt del lcd
  lcd.setCursor(0, 0);                // Posicionamiento del cursor en la posición (0,0)
  lcd.print("Monitorizacion");        // Muestra de mensaje
  lcd.setCursor(0, 1);                // Posicionamiento del cursor en la posición (0,1)
  lcd.print("de clima");              // Muestra de mensaje
  delay(1000);                        // Delay de 1 segundo
  lcd.clear();                        // Borrado de pantalla  
  digitalWrite(CALEFACTOR_PIN, LOW);  // Inicialización de calefactor en modo OFF
}

void loop()
{

  int analogValueNTC = analogRead(TEMP_PIN);              // Lectura analógica del valor de la NTC
  int analogValueLDR = analogRead(LDR_PIN);               // Lectura analógica del valor del sensor LDR
  int analogValueGAS = analogRead(GAS_PIN);               // Lectura analógica del valor del sensor de calidad de aire
  int analogValueVERT = analogRead(VERT_PIN);             // Lectura analógica del valor ed componente vertical dir. viento
  int analogValueHOR = analogRead(HOR_PIN);               // Lectura analógica del valor ed componente horizontal dir. viento
  int analogValueWIND = analogRead(WIND_INTENSITY_PIN);   // Lectura analógica del valor de la simulación de intensidad de viento


  //LDR
  // Procesado de la medida del LDR
  float voltage = analogValueLDR * 3.3 / 4095.0;          // Pasar cuentas a voltios
  float resistance = 2000.0 * voltage / (3.3 - voltage);  // Con resistencia de 2K
  float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1.0 / GAMMA))/10;  // Cálculo de lux (para LDR común)

  //Mostrar en el LCD la información de la iluminación
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LUX:");
  lcd.setCursor(0, 1);
  lcd.print(lux);
  delay(2000);

  //NTC
  float celsius = 1 / (log(1 / (4096. / analogValueNTC - 1)) / BETA + 1.0 / 298.15) - 273.15; // procesado de la señal para mostrar Grados Celsius

  //Mostrar en el LCD la información de temperatura de la bateria
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp.Bateria:");
  lcd.setCursor(0, 1);
  lcd.print(celsius);
  delay(2000);
  
  // Gestión del uso del calefactor con una pequeña histeresis para evitar ncendidos y apagados continuos
  if(celsius <= 18)
  {
    digitalWrite(CALEFACTOR_PIN, HIGH);
  }
  else if(celsius >= 22)
  {
    digitalWrite(CALEFACTOR_PIN, LOW);
  }


  // DHT22
  float humedad = dht.readHumidity();
  float temperatura = dht.readTemperature(); // Celsius
  
  //Mostrar en el LCD la información de temperatura y humedad exterior
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp. EXT:");
  lcd.setCursor(0, 1);
  lcd.print(temperatura);
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hum. EXT:");
  lcd.setCursor(0, 1);
  lcd.print(humedad);
  delay(2000);


  //Mostrar en el LCD la información de calidad de aire
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GAS:");
  lcd.setCursor(0, 1);
  lcd.print(analogValueGAS);
  delay(2000);

  //Mostrar en el LCD la información de intensidad del viento

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WIND INTENSITY:");
  lcd.setCursor(0, 1);
  lcd.print(analogValueWIND);
  delay(2000);

  //Mostrar en el LCD la información de las componentes vertical/horizontal para la simulación de dirección del viento.
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Comp. VERT:");
  lcd.setCursor(6, 0);
  lcd.print(analogValueVERT);
  lcd.setCursor(0, 1);
  lcd.print("Comp. HOR:");
  lcd.setCursor(6, 1);
  lcd.print(analogValueHOR);
  delay(2000);

  // Analisis de las componentes de la dirección del viento para mostrar usando puntos cardinales
  if(analogValueVERT == 2048 && analogValueHOR == 2048)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("NO VIENTO");
  }
  else if(analogValueVERT == 2048 && analogValueHOR == 0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("VIENTO");
    lcd.setCursor(0, 1);
    lcd.print("DIR. ESTE");
  }
  else if(analogValueVERT == 2048 && analogValueHOR == 4095)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("VIENTO");
    lcd.setCursor(0, 1);
    lcd.print("DIR. OESTE");
  }
  else if(analogValueVERT == 0 && analogValueHOR == 2048)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("VIENTO");
    lcd.setCursor(0, 1);
    lcd.print("DIR. SUR");
  }
  else if(analogValueVERT == 4095 && analogValueHOR == 2048)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("VIENTO");
    lcd.setCursor(0, 1);
    lcd.print("DIR. NORTE");
  }
  else if(analogValueVERT == 4095 && analogValueHOR == 0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("VIENTO");
    lcd.setCursor(0, 1);
    lcd.print("DIR. NORESTE");
  }
  else if(analogValueVERT == 4095 && analogValueHOR == 4095)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("VIENTO");
    lcd.setCursor(0, 1);
    lcd.print("DIR. NOROESTE");
  }
  else if(analogValueVERT == 0 && analogValueHOR == 4095)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("VIENTO");
    lcd.setCursor(0, 1);
    lcd.print("DIR. SURORESTE");
  }
  else if(analogValueVERT == 0 && analogValueHOR == 0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("VIENTO");
    lcd.setCursor(0, 1);
    lcd.print("DIR. SURESTE");
  }
  delay(2000);

}