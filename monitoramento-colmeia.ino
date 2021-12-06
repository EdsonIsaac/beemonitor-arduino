#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <HX711.h>

#define DHTPIN 4
#define DHTTYPE DHT11

#define pinDT  13
#define pinSCK  15

DHT dht(DHTPIN, DHT11);
HX711 scale;

const char* ssid = "Edson Isaac";
const char* password = "isaacedsonlima";

const char codigo[] = "0001";

long tempoDelay = 60000 * 15;
unsigned long ultimaLeitura = 0;
unsigned long tempoDecorrido = 0;

float temperatura, umidade, peso;

void setup () {
  Serial.begin(115200);
  setupWifi();
  setupSensors();

  delay(1000);
  sendData();
}

void loop() {
  tempoDecorrido = millis();

  if (tempoDecorrido < ultimaLeitura) {
    ultimaLeitura = 0;
  }

  if ((tempoDecorrido - ultimaLeitura) >= tempoDelay) {
    sendData();
    ultimaLeitura = tempoDecorrido;
  }
}

void sendData() {
 
  if (WiFi.status() == WL_CONNECTED) {

    temperatura = dht.readTemperature(); 
    umidade = dht.readHumidity();
    peso = scale.get_units(5);

    Serial.println("Temperatura: " + String(temperatura) + " | Umidade: " + String(umidade) + " | Peso: " + String(peso));
    
    WiFiClientSecure client;
    HTTPClient http;
    
    client.setInsecure();
    client.connect("https://beemonitor-backend.herokuapp.com", 443);
    http.begin(client, "https://beemonitor-backend.herokuapp.com/colmeias/update?codigo=" + String(codigo) + "&temperatura=" + String(temperatura) + "&umidade=" + String(umidade) + "&peso=" + String(peso));
    
    if (http.GET() > 0) {
      Serial.println(http.getString());
    }
    
    http.end();
  } else {
    setupWifi();
    sendData();
  }
}

void setupSensors() {
  dht.begin();
  scale.begin(pinDT, pinSCK);
  scale.set_scale(-422.50);
  scale.tare();
}

void setupWifi () {
  Serial.println("Conectando a Rede: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}
