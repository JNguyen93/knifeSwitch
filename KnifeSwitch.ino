#define arr_len( x )  ( sizeof( x ) / sizeof( *x ) )

const byte brake = 13;
const byte switch1 = 2;
const byte switch2 = 3;
const byte switch3 = 4;
const byte switch4 = 5;
const byte roomLights = 6;
const byte reed0 = 7;
const byte kswitch0 = 8;
const byte jack1 = 9;
const byte trainLights0 = 10;
const byte trainLights1 = 11;
const byte maglock = 12;
const byte magexit = A3;
const byte reed1 = A4;
const byte led0 = A0;
const byte led1 = A1;
const byte genLED = A5;
const byte jack0 = A2;
bool jackFlag = false;
boolean ksw0;
boolean sw1;
boolean sw2;
boolean sw3;
boolean sw4;
boolean flag0;
boolean flag1;
boolean flag2;
boolean flag3;
boolean penalty1;
boolean penalty2;
boolean penalty3;
boolean lightsOn;
boolean on;
long long lastDoorClose = 0;
int closeDelay = 12000;
boolean ksFlag = false;

boolean ksw0Values[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int counter = 0;
int ksw0Size = arr_len( ksw0Values );
int acceptablePercentage = 6;

void setup() {
  Serial.begin(9600);
  //Serial.println("Knife Switch Test");
  pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  pinMode(switch3, INPUT);
  pinMode(switch4, INPUT);
  pinMode(kswitch0, INPUT_PULLUP);
  pinMode(trainLights0, OUTPUT);
  pinMode(trainLights1, OUTPUT);
  pinMode(roomLights, OUTPUT);
  pinMode(maglock, OUTPUT);
  pinMode(genLED, OUTPUT);
  pinMode(reed0, INPUT_PULLUP);
  pinMode(reed1, INPUT_PULLUP);
  pinMode(brake, INPUT_PULLUP);
  pinMode(magexit, OUTPUT);
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(jack0, INPUT_PULLUP);
  pinMode(jack1, INPUT_PULLUP);
  digitalWrite(trainLights0, LOW);
  digitalWrite(trainLights1, LOW);
  digitalWrite(led0, LOW);
  digitalWrite(led1, LOW);
  digitalWrite(roomLights, LOW);
  digitalWrite(magexit, HIGH);
}

void loop() {
  sw1 = digitalRead(switch1);
  delay(10);
  sw2 = digitalRead(switch2);
  delay(10);
  sw3 = digitalRead(switch3);
  delay(10);
  sw4 = digitalRead(switch4);
  delay(10);

  //debug();

  if (sw3 && !penalty1 && !penalty2 && !penalty3) {
    flag0 = true;
  }
  else {
    flag0 = false;
  }
  if (sw1 && flag0 && !penalty1 && !penalty2 && !penalty3) {
    flag1 = true;
  }
  else if (sw1 && !flag0) {
    flag1 = false;
    penalty1 = true;
  }
  else if (!sw1) {
    flag1 = false;
    penalty1 = false;
  }
  if (sw2 && flag1 && !penalty1 && !penalty2 && !penalty3) {
    flag2 = true;
  }
  else if (sw2 && !flag1) {
    flag2 = false;
    penalty2 = true;
  }
  else if (!sw2) {
    flag2 = false;
    penalty2 = false;
  }
  if (sw4 && flag2 && !penalty1 && !penalty2 && !penalty3) {
    flag3 = true;
    lightsOn = true;
  }
  else if (sw4 && !flag2) {
    flag3 = false;
    penalty3 = true;
  }
  else if (!sw4) {
    flag3 = false;
    penalty3 = false;
  }

  if (!flag0 || !flag1 || !flag2 || !flag3) {
    lightsOn = false;
  }

  if (lightsOn) {
    digitalWrite(roomLights, HIGH);
    //Serial.println("Lights On!");
  }
  else {
    digitalWrite(roomLights, LOW);
  }

  ksw0 = digitalRead(kswitch0);
  ksw0Values[counter] = ksw0;
  delay(10);

  if (counter < 9 && counter >= 0) {
    counter++;
  }
  else {
    counter = 0;
  }

  int ksw0Percentage = 0;

  for (int i = 0; i < ksw0Size; i++) {
    if (ksw0Values[i] == false) {
      ksw0Percentage++;
    }
  }

  if (!digitalRead(jack1)){
     digitalWrite(led1, HIGH);
  }
  else{
    digitalWrite(led1, LOW);
    digitalWrite(genLED, LOW);
  }
  
  if(!digitalRead(jack0)){
    digitalWrite(led0, HIGH);
    if(!digitalRead(jack1)){
      digitalWrite(led1, HIGH);
      digitalWrite(genLED, HIGH);
      jackFlag = true;
    }
    else{
      digitalWrite(led1, LOW);
      digitalWrite(genLED, LOW);
      jackFlag = false;
    }
  }
  else{
    digitalWrite(led0, LOW);
  }
  
  if (ksw0Percentage >= acceptablePercentage && jackFlag) {
    digitalWrite(trainLights0, HIGH);
    digitalWrite(trainLights1, HIGH);
    if (!on) {
      delay(1000);
      digitalWrite(maglock, HIGH);
      delay(5000);
      digitalWrite(maglock, LOW);
      while (!digitalRead(reed0) || !digitalRead(reed1)) {
        if (millis() - lastDoorClose > closeDelay) {
          Serial.write(1);
          lastDoorClose = millis();
        }
        ksw0 = digitalRead(kswitch0);
        delay(10);
        if(ksw0){
          digitalWrite(maglock, HIGH);
          digitalWrite(trainLights0, LOW);
          digitalWrite(trainLights1, LOW);
          digitalWrite(genLED, LOW);
        }
        else{
          digitalWrite(maglock, LOW);
          digitalWrite(trainLights0, HIGH);
          digitalWrite(trainLights1, HIGH);
          digitalWrite(genLED, HIGH);
        }
      }
      Serial.write(2);
      on = true;
    }
    digitalWrite(maglock, LOW);
  }
  else {
    digitalWrite(trainLights0, LOW);
    digitalWrite(trainLights1, LOW);
    digitalWrite(maglock, LOW);
    on = false;
  }

  if(!digitalRead(brake)){
    //Serial.println(!digitalRead(brake));
    //Serial.println("Braking!");
    Serial.write(3);
    delay(20000);
    digitalWrite(magexit, LOW);
    delay(500);
    digitalWrite(magexit, HIGH);
  }
}

void debug() {
  Serial.print("Switch1: ");
  Serial.println(sw1);
  Serial.print("Switch2: ");
  Serial.println(sw2);
  Serial.print("Switch3: ");
  Serial.println(sw3);
  Serial.print("Switch4: ");
  Serial.println(sw4);

  Serial.print("Flag0: ");
  Serial.println(flag0);
  Serial.print("Flag1: ");
  Serial.println(flag1);
  Serial.print("Flag2: ");
  Serial.println(flag2);
  Serial.print("Flag3: ");
  Serial.println(flag3);

  Serial.print("Penalty1: ");
  Serial.println(penalty1);
  Serial.print("Penalty2: ");
  Serial.println(penalty2);
  Serial.print("Penalty3: ");
  Serial.println(penalty3);
}

