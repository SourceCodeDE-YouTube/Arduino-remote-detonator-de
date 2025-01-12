// Pins für Relais und Piezo
const int relayPin = 22;
const int piezoPin = 25;

// Variablen
String bluetoothInput = "";
int countdown = 0;
bool isActive = false;
unsigned long lastMillis = 0;

void setup() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Relais aus

  pinMode(piezoPin, OUTPUT); // Piezo als Ausgang

  // Serielle Verbindung für HC-06
  Serial.begin(9600); // Bluetooth-Modul an RX0/TX1
}

void loop() {
  // Daten über Bluetooth empfangen
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      processCommand(bluetoothInput);
      bluetoothInput = ""; // Eingabe zurücksetzen
    } else {
      bluetoothInput += c;
    }
  }

  // Countdown-Logik
  if (isActive && countdown > 0) {
    unsigned long currentMillis = millis();

    // Sekündliches Piepen für mehr als 5 Sekunden verbleibend
    if (countdown > 5 && currentMillis - lastMillis >= 1000) {
      lastMillis = currentMillis;
      playTone(1000, 100); // 1000 Hz für 100 ms
      countdown--;

      // Countdown in der Bluetooth-Konsole anzeigen
      Serial.println("Countdown: " + String(countdown) + " Sekunden");
    }

    // Schnelles Piepen in den letzten 5 Sekunden
    else if (countdown <= 5 && currentMillis - lastMillis >= 1000) {
      lastMillis = currentMillis;
      for (int i = 0; i < 5; i++) { // 5 Pieptöne pro Sekunde
        playTone(1500, 100); // 1500 Hz für 100 ms
        Serial.println("WARNUNG: Rakete zündet in " + String(countdown) + " Sekunden!");
        delay(100);          // Pause zwischen den Tönen
      }
      countdown--;

      // Countdown in der Bluetooth-Konsole anzeigen
      Serial.println("Countdown: " + String(countdown) + " Sekunden");

      // Relais einschalten, wenn Countdown endet
      if (countdown == 0) {
        digitalWrite(relayPin, HIGH); // Relais ein
        Serial.println("Zündung!");
        delay(2000);                 // 2 Sekunden warten
        digitalWrite(relayPin, LOW); // Relais aus
        isActive = false;
        Serial.println("Relais deaktiviert!");
      }
    }
  }
}

void processCommand(String command) {
  // Überprüfen, ob der Befehl gültig ist
  if (command.startsWith("On ")) {
    countdown = command.substring(3).toInt();
    if (countdown > 0) {
      isActive = true;
      lastMillis = millis(); // Zeit resetten
      String response = "Countdown gestartet: " + String(countdown) + " Sekunden!";
      Serial.println(response); // Antwort senden
    } else {
      Serial.println("Ungültige Zahl erhalten.");
    }
  } else {
    String unknownCommand = "Unbekannter Befehl: " + command;
    Serial.println(unknownCommand);
  }
}

// Funktion zum Piepen
void playTone(int frequency, int duration) {
  tone(piezoPin, frequency, duration); // Frequenz und Dauer
  delay(duration);                     // Warten, bis Ton gespielt wurde
  noTone(piezoPin);                    // Ton stoppen
}
