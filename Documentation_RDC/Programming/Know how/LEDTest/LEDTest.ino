#include <Bluepad32.h>  //Muss heruntergeladen werden in im Boards manager 
#include "reg_write.h"  //Muss in den Arduino Projektordner kopiert werden

// MAC-Adresse, die erlaubt ist (z. B. PS4, Xbox oder Switch Pro Controller)
const bd_addr_t allowed_addr = {0xA0, 0x5A, 0x5E, 0xC9, 0x13, 0x75}; //MAC Adresse des Controllers mitgeben
uint8_t ledVariable = 0b00000001; //Startwert: 1 am niedrigstwertigen Bit

GamepadPtr myGamepad = nullptr; // Zeiger auf das aktuell verbundene Gamepad, Anfangswert nullptr bedeutet: Es ist noch kein Gamepad verbunden.

// Vergleiche MAC-Adressen
bool isAddrAllowed(const bd_addr_t& addr) {
  return memcmp(addr, allowed_addr, 6) == 0;
}

void onConnectedGamepad(GamepadPtr gp) {
  const bd_addr_t& mac = gp->getProperties().btaddr; //Bluetooth-MAC-Adresse des Controllers gp und speichert sie als Referenz in mac.

  Serial.printf("Verbindung von: %02X:%02X:%02X:%02X:%02X:%02X\n", 
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]); //Gibt die MAC-Adresse des verbundenen Controllers aus.

  //
if (isAddrAllowed(mac)) {
    // Wenn die MAC-Adresse in der Liste der erlaubten Controller ist,
    // dann Gamepad akzeptieren und merken
    Serial.println("Erlaubter Controller verbunden.");
    myGamepad = gp;
} else {
    // Wenn das Gamepad nicht auf der "Whitelist" steht:
    Serial.println("Nicht erlaubter Controller – wird getrennt.");

    delay(2000);  // Kurz warten, um sicherzustellen, dass die Verbindung vollständig aufgebaut ist.
                  // Das verhindert Verbindungsfehler (z. B. GAP-Fehler) beim direkten Trennen.

    if (gp->isConnected()) {
        // Falls das Gamepad tatsächlich noch verbunden ist: aktiv trennen
        gp->disconnect();
    }
}

}

void onDisconnectedGamepad(GamepadPtr gp) {
  // Diese Funktion wird aufgerufen, wenn ein Gamepad die Verbindung trennt

  Serial.println("Controller getrennt.");

  // Prüfen, ob das getrennte Gamepad das aktuell genutzte (myGamepad) war
  if (gp == myGamepad) {
    // Wenn ja, Zeiger zurücksetzen (kein Controller mehr verbunden)
    myGamepad = nullptr;
  }
}


void setup() {
  Serial.begin(115200);

  //Pins als Outputs festlegen
  pinMode( dataPin, OUTPUT);
  pinMode( latchPin, OUTPUT);
  pinMode( clockPin, OUTPUT);

  // Bluepad32 starten
  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);

  // Eigene MAC-Adresse anzeigen
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("ESP32 Bluetooth-MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  // Keine Allowlist-Funktionen – MAC-Filterung erfolgt im Callback
}

void loop() {
  BP32.update(); //Führt ein Update verbundenen Bluetooth-Controller durch

  if (myGamepad && myGamepad->isConnected()) { //// Prüft, ob ein Gamepad verbunden ist.
    if (myGamepad->buttons() == 0x0002) { //Taste 3 oder Taste X
      
      //Zyklische Verschiebung: Schiebe alle Bits nach links und füge die höchste Position zurück hinzu
      ledVariable = ( ledVariable << 1) | ( ledVariable >> 7);
      
      delay( 200); //Wartezeit um den Effekt sichtbar zu machen  
    }
  }
  __register_write_leds__( ledVariable); //Aufrufen der Funktion um die Register zu beschreiben und die LEDs an- und auszuschalten
}
