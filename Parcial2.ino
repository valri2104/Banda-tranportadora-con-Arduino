const int S1 = 30;
const int S2 = 31;
const int S3 = 32;
const int S4 = 33;
const int pin34 = 34;
const int pin35 = 35;
const int pausa = 36;
const int start = 37;
const int derecha = 22;
const int izquierda = 23;

bool corriendo = false;
bool programa = true;

void tiempo(int segundos) {
  unsigned int tinicial = millis();
  unsigned int tactual = millis();
  int switchState = digitalRead(pausa);

  while (((tactual - tinicial) < segundos) && switchState == LOW) {
    tactual = millis();
    switchState = digitalRead(pausa);
  }
}

int obtenerPosicionActual() {
  int posicionActual = -1;
  if (digitalRead(S1) == HIGH) {
    posicionActual = 1;
  } else if (digitalRead(S2) == HIGH) {
    posicionActual = 2;
  } else if (digitalRead(S3) == HIGH) {
    posicionActual = 3;
  } else if (digitalRead(S4) == HIGH) {
    posicionActual = 4;
  }
  return posicionActual;
}

void detenerBanda() {
  digitalWrite(derecha, LOW);
  digitalWrite(izquierda, LOW);
  Serial.println("Banda detenida");
}

void setup() {
  Serial.begin(9600);
  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(pin34, INPUT);
  pinMode(pin35, INPUT);
  pinMode(start, INPUT);
  pinMode(pausa, INPUT);
  pinMode(derecha, OUTPUT);
  pinMode(izquierda, OUTPUT);
  pinMode(luz, OUTPUT);
}

void loop() {
  while (programa) {
    // si se oprime start
    if (digitalRead(start) == HIGH) {
      corriendo = true;
    }
    // correr el programa principal
    while (corriendo) {
      int contador = 0;

      // si se oprime pausa
      if (digitalRead(pausa) == HIGH) {
        corriendo = false;  // se detiene el programa principal
        detenerBanda();
        tiempo(0);
      }

      // instruccion 11 -> ERROR
      if ((digitalRead(pin34) == HIGH) && (digitalRead(pin35) == HIGH)) {
        Serial.println("ERROR");
        digitalWrite(derecha, LOW);
        digitalWrite(izquierda, LOW);
        tiempo(500);
        corriendo = false;
        programa = false;

        // instruccion 00 -> va S1 -> S3 -> S2
      }
      if ((digitalRead(pin35) == LOW) && (digitalRead(pin34) == LOW) && (digitalRead(pausa) == LOW) && (digitalRead(start) == HIGH)) {
        while (!digitalRead(S1)) {
          digitalWrite(derecha, HIGH);
        }
        detenerBanda();
        tiempo(500);
        while (!digitalRead(S3)) {
          digitalWrite(izquierda, HIGH);
        }
        detenerBanda();
        tiempo(500);
        while (!digitalRead(S2)) {
          digitalWrite(derecha, HIGH);
        }
        detenerBanda();
        tiempo(5000);

        // instruccion 01 -> va S2 y regresa el sensor que inicio
      }
      if ((digitalRead(pin35) == LOW) && (digitalRead(pin34) == HIGH) && (digitalRead(pausa) == LOW) && (digitalRead(start) == HIGH)) {
        int pos = obtenerPosicionActual();
        int inicio = obtenerPosicionActual();
        int destino = 2;
        int direccion = pos - destino;
        while (pos != destino) {
          if (direccion < 0) {
            // Girar a la izquierda
            digitalWrite(derecha, LOW);
            digitalWrite(izquierda, HIGH);
          } else if (direccion > 0) {
            // Girar a la derecha
            digitalWrite(derecha, HIGH);
            digitalWrite(izquierda, LOW);
          }
          pos = obtenerPosicionActual();
        }
        detenerBanda();
        tiempo(500);
        direccion = pos - inicio;

        while (pos != inicio) {
          if (direccion < 0) {
            // Girar a la izquierda
            digitalWrite(derecha, LOW);
            digitalWrite(izquierda, HIGH);
          } else if (direccion > 0) {
            // Girar a la derecha
            digitalWrite(derecha, HIGH);
            digitalWrite(izquierda, LOW);
          }
          pos = obtenerPosicionActual();
        }
        detenerBanda();
        tiempo(4000);

        // instruccion 10 -> va S4 a S3 2 veces, prende y apaga pin(24) cada medio segundo
      }
      if ((digitalRead(pin35) == HIGH) && (digitalRead(pin34) == LOW) && (digitalRead(pausa) == LOW) && (digitalRead(start) == HIGH) && (contador != 2)) {
        while (contador != 2) {
          while (!digitalRead(S4)) {
            digitalWrite(izquierda, HIGH);
          }
          detenerBanda();
          tiempo(500);
          while (!digitalRead(S3)) {
            digitalWrite(derecha, HIGH);
          }
          detenerBanda();
          tiempo(500);
          contador++;
        }
        tiempo(5000);
      }
    }
  }
}