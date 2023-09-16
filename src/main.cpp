#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);  //Construyo objeto de la clase DHT que representa el sensor



const char * ssid = "JUANDAVID";
const char * pass = "123456789";
const char *host = "dweet.io";
const int port = 80; // Puerto por defecto del servicio web



void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, pass); // Inicializar modulo WIFI y que se vincule a la red indicada en el ssid
  Serial.print("Intentando Conexion");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Conectado");
  Serial.print("Direccion IP: ");
  Serial.println(WiFi.localIP());

  pinMode(2, OUTPUT);    // Configurar pin 2 como salida
  digitalWrite(2, HIGH); // Encender Led
}

void loop()
{
  WiFiClient cliente; // Objeto que realiza peticiones a un socket servidor

  if (!cliente.connect(host, port))
  {
    Serial.println("Conexion Fallida al servidor");
    delay(1000);
    return;
  }

  // Si llega a este linea es porque si hubo conexion con el servidor
  // Ahora tenemos un flujo de transmision y otro de recepcion
  float humedad = dht.readHumidity(); //leo la humedad relativa
  float temperatura = dht.readTemperature(); //leo la temperatura en grados celcius
  String request = String("GET ") +"/dweet/for/Juan20?"+"Humedad=" + String(humedad) +"&"+"Temperature=" + String(temperatura) + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";

  // Enviamos la peticion en  protocolo http al servidor
  cliente.print(request);
  unsigned long inicio = millis();
  while (cliente.available() == 0)
  {
    if (millis() - inicio > 8000)
    {
      Serial.println("Tiempo de espera del servidor agotado");
      cliente.stop();
      return;
    }
  }
  
  // Si se llega a este punto es porque se recibieron datos del servidor (cliente.available() !=0)
  while (cliente.available()){
    String linea = cliente.readStringUntil('\r'); // Lea un string hasta que encuentre el caracter
    Serial.println(linea);
  }

    if(isnan(humedad) || isnan(temperatura)) {
    Serial.println("Error al leer el sensor DHT22");
    return;
  }

  Serial.println("Fin de conexion al servidor");
  cliente.stop();
  delay(5000); 
}