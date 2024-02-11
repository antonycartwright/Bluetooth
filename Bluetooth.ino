const int ThermistorPin = A2;
const float thermR1 = 12000;
const float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

float logR2, thermR2, Tc;

const int VoltagePin = A0;
const float vDivR1 = 12;
const float vDivR2 = 4.7;
const float vScaleFactor = (vDivR1 + vDivR2) / vDivR2;
const float vPerADCCount = 0.0048828125;
float voltage;

bool transmit = false;

String currentTransmission;
String previousTransmission;

const int MAX_MESSAGE_LENGTH = 32;

void setup() {
  Serial.begin(9600);
}

void loop() {

  thermR2 = thermR1 * (1023.0 / (float)analogRead(ThermistorPin) - 1.0);
  logR2 = log(thermR2);
  Tc = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2)) - 273.15;

  voltage = ((analogRead(VoltagePin) + 0.5) * vPerADCCount) * vScaleFactor;

  currentTransmission = String(voltage).substring(0, 4) + "-" + String(Tc).substring(0, 4);

  if (currentTransmission != previousTransmission) {

    if (transmit) {
      Serial.println(currentTransmission);

      previousTransmission = currentTransmission;
    }
  }

  while (Serial.available() > 0) {
    //Create a place to hold the incoming message
    static char message[MAX_MESSAGE_LENGTH];
    static unsigned int message_pos = 0;
    //Read the next available byte in the serial receive buffer
    char inByte = Serial.read();
    //Message coming in (check not terminating character) and guard for over message size
    if (inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1)) {
      //Add the incoming byte to our message
      message[message_pos] = inByte;
      message_pos++;

      String incoming = String(message);

      if (incoming.indexOf("+++++C1") > -1) {
        //Serial.println("-- Connected!");
        //Do nothing?
        transmit = false;
      }

      if (incoming.indexOf("Start Updates") > -1) {
        //Serial.println("-- Start Updates");
        transmit = true;
      }

      if (incoming.indexOf("+-----C1") > -1) {
        //Serial.println("-- Disconnected!");
        transmit = false;
      }

      if (incoming.indexOf("Stop Updates") > -1) {
        //Serial.println("-- Stop Updates!");
        transmit = false;
      }
    } else {
      //Add null character to string
      message[message_pos] = '\0';
      message_pos = 0;
    }
  }

  delay(100);
}