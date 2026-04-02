#include <Bluepad32.h>

// MAC-Adresse, die erlaubt ist (z. B. PS4, Xbox oder Switch Pro Controller)
const bd_addr_t allowed_addr = {0xA0, 0x5A, 0x5E, 0xC9, 0x13, 0x75};

GamepadPtr myGamepad = nullptr;

// Vergleiche MAC-Adressen
bool isAddrAllowed(const bd_addr_t& addr) {
  return memcmp(addr, allowed_addr, 6) == 0;
}

void onConnectedGamepad(GamepadPtr gp) {
  const bd_addr_t& mac = gp->getProperties().btaddr;

  Serial.printf("Verbindung von: %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  if (isAddrAllowed(mac)) {
    Serial.println("Erlaubter Controller verbunden.");
    myGamepad = gp;
  } else {
    Serial.println("Nicht erlaubter Controller – wird getrennt.");
    delay(2000);  // Warten auf vollständige Verbindung (vermeidet GAP-Fehler)
    if (gp->isConnected()) {
      gp->disconnect();
    }
  }
}

void onDisconnectedGamepad(GamepadPtr gp) {
  Serial.println("Controller getrennt.");
  if (gp == myGamepad) {
    myGamepad = nullptr;
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Bluepad32 starten
  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);

  // Eigene MAC-Adresse anzeigen
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("ESP32 Bluetooth-MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  // Keine Allowlist-Funktionen – MAC-Filterung erfolgt im Callback
}

void loop() {
  BP32.update();

  if (myGamepad && myGamepad->isConnected()) {
    if (myGamepad->a()) {
      Serial.println("Taste A gedrückt!");
    }
  }
}
