// Cargar Libreria WiFi
#include <WiFi.h>

// Inicializar variables para el temporizador
double ti = 0;
double dt = 0;
int ll = 0;

// Credenciales WiFi
const char* ssid = "Multivac";
const char* password = "hola1234";

// Configurar el puerto del servidor en 80
WiFiServer server(80);

// Inicialziar variable para almacenar la solicitud HTTP
String header;

// Inicializar variables auxiliares para guardar el estado
// el estado del output actual
String measuringTime = "off";
double lastMeasuredTime = 0;

// Definir pines para el input
const int LDT26 = 32;
const int LDT25 = 35;

const int LDT[] = {32, 35};

// Tiempo actual
unsigned long currentTime = millis();
// Tiempo anterior
unsigned long previousTime = 0;
// Timeout en milisegundos (2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  // Iniciar comunicacion Serial para debugueo
  Serial.begin(115200);

  // Conectar a la red WiFi con las credenciales definidas
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Mostrar Direccion IP local y comenzar WebServer
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  // Escuchar clientes que solicitan acceso
  WiFiClient client = server.available();   

  // Si un cliente se conecta,
  if (client) {    
    // Actualizar valores del tiempo de coneccion                       
    currentTime = millis();
    previousTime = currentTime;
    // Imprimir un mensaje en el puerto serial
    Serial.println("New Client.");   
    // Inicializar un String para almacenar la informaicon del cliente
    String currentLine = "";
    // Iterar mientras el cliente estè conectado
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /start/on") >= 0) {
              Serial.println("starting time measurement");
              measuringTime = "on";
              lastMeasuredTime = timer();
              Serial.println("time measurement finished");
            } else if (header.indexOf("GET /start/off") >= 0) {
              Serial.println("Waiting for instruction");
              measuringTime = "off";
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>Temporizador Velocistas</h1>");
            client.println("<h1>IRE - 2022</h1>");

            // Display current state, and ON/OFF buttons LDT GPIOs
            client.println("<p>LDT 1 state " + String(analogRead(LDT25)) + "</p>");
            Serial.println(String(analogRead(32)));
            client.println("<p>LDT 2 state " + String(analogRead(LDT26)) + "</p>");
            Serial.println(String(analogRead(36)));

            // Start taking time
            if (measuringTime == "off") {
              client.println("<p><a href=\"/start/on\"><button class=\"button\">START</button></a></p>");
            } else {
              client.println("<p><a href=\"/start/off\"><button class=\"button button2\">RESET</button></a></p>");
            }

            client.println("<h1>Last measured time: " + String(lastMeasuredTime) + "[ms]</h1>");


            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

// Funcion para iniciar el temporizador LASER
double timer() {
  checkLaser(0);
  ti = millis();
  delay(3000);
  checkLaser(1);
  dt = millis() - ti;
  return dt;
}

// Iteracion sobre el sensor LASER
void checkLaser(int i) {
  ll = analogRead(LDT[i]);
  Serial.print("valor LDT: ");  Serial.println(i);
  Serial.println("");
  Serial.println(ll);
  Serial.println("");
  while (ll > 1000) {
    ll = analogRead(LDT[i]);
  }
}
