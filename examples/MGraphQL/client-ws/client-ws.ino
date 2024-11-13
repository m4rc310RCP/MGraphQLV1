void setup(){
  Serial.begin(115200);
}

void loop(){
	unsigned long currentMillis = millis();
	if (millis() % 1500 == 0){
    Serial.print(".");
	}
}
