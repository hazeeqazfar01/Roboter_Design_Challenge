// Register Variablen
const uint8_t dataPin = 25; // SER
const uint8_t latchPin = 26; // RCLK
const uint8_t clockPin = 27; // SRCLK

//Statische Variablen der Treiber- und LED-Zustände
static uint16_t val_dri;  
static uint8_t val_led;
uint32_t val_out = 0;

//Schreiben der Register bei Änderung der Treiber-Zustände
void __register_write_drivers__(uint16_t bit_val_drivers) {

val_dri = bit_val_drivers;  //Schreiben der statischen Variable
val_out = ((val_dri & 0xFFFF) << 8) | (val_led & 0xFF);  //Zusammenfügen der Bytes um alle Register zu beschreiben

  digitalWrite(latchPin, LOW);    //Beschreiben ermöglichen durch ziehen des Latch Pins auf low
    for (int i = 0; i < 24; i++) {  //Schleife zum einschieben der einzelnen Bits
      digitalWrite(clockPin, LOW);
      digitalWrite(dataPin, val_out & 1);
      val_out >>= 1;
      digitalWrite(clockPin, HIGH);
      //Serial.println("Register Bitvalue");
      //Serial.println(val_out, BIN);
    }
    digitalWrite(latchPin, HIGH);   //Schreiben der Zustände auf die Ausgänge durch ziehen des Latch Pins auf high
}

//Schreiben der Register bei Änderung der LED-Zustände
void __register_write_leds__(uint8_t bit_val_leds) {

val_led = bit_val_leds; //Schreiben der statischen Variable
val_out = ((val_dri & 0xFFFF) << 8) | (val_led & 0xFF);  //Zusammenfügen der Bytes um alles Register zu beschreiben

  digitalWrite(latchPin, LOW);  //Beschreiben ermöglichen durch ziehen des Latch Pins auf low
  for (int j = 0; j < 24; j++){ //Schleife zum einschieben der einzelnen Bits
      digitalWrite(clockPin, LOW);  //Fester LOW Zustand des Clockpins um Datenübertragung zu ermöglichen
      digitalWrite(dataPin, val_out & 1); //Überprüfen ob das zu Übertragene Bit 0 oder 1 ist und anschließend ausgeben an das Register
      val_out >>= 1;  //"Weiterschieben" der Bits
      digitalWrite(clockPin, HIGH); //Signal dafür, dass das Bit übetragen wurde und ein neues folgt
      //Serial.println("Register LED Bitvalue");  //Darstellung im Serial-Monitor
      //Serial.println(val_out, BIN);
      }
    digitalWrite(latchPin, HIGH); //Schreiben der Zustände auf die Ausgänge durch ziehen des Latch Pins auf high
}