// Definicion pines para sensores LASER
const int LDT[] = {32, 35};
// Inicializacion variable luminosidad laser
int ll = 0;
// Inicializacion variables temporizador
double ti = 0, dt = 0;
double vuelta = 0;

// Configuracion inicial
void setup() {
  // Inicializacion comunicacion serial
  Serial.begin(9600);
  delay(200);
}

// loop loop loop
void loop() { 
  Serial.println("iniciando temporizador");
  delay(200);
  // Iniciar Temporizador
  vuelta = timer();
  Serial.println(" --- --- ---");
  Serial.println("tiempo de vuelta:");
  Serial.println(vuelta);
  Serial.println(" ");
  delay(3000);  
}

// Funcion que llama al temporizador
double timer() {
  checkLaser(0);
  ti = millis();
  delay(3000);
  checkLaser(1);
  dt = millis() - ti;
  return dt;
}

// Funcion que hace el conteo del tiempo
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
