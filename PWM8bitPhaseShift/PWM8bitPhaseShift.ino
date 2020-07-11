#define PWM_ON_PIN 8
#define PWM1_PIN 6
#define PWM2_PIN 3

bool PWM_ON_PIN_value=false;
bool PWM_ON_PIN_value_old=false;
bool positiveEdgeDetected=false;
bool newParametersFlag=false;

uint8_t PWM1_PRESCALER=1;
uint8_t PWM1_TOP=128;
uint8_t PWM1_OFFSET=0;


uint8_t PWM2_PRESCALER=1;
uint8_t PWM2_TOP=128;
uint8_t PWM2_OFFSET=0;


void startPWM(uint8_t PWM1_PRESCALER, uint8_t PWM1_TOP, uint8_t PWM1_OFFSET, uint8_t PWM2_PRESCALER, uint8_t PWM2_TOP, uint8_t PWM2_OFFSET){
  /* Save global interrupt flag */
  unsigned char sreg = SREG;
  /* Disable interrupts */
  cli();
  
  /*Turn off counter and set to non-PWM mode*/
  
  TCCR0B=0;
  TCCR2B=0;
  

  
  /*Enable fast PWM, set offset, prescalers and top*/
  TCCR0A=_BV(COM0A1) | _BV(WGM01) | _BV(WGM00);
  TCCR2A=_BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCNT0=PWM1_OFFSET;
  TCNT2=PWM2_OFFSET;
  OCR0A=PWM1_TOP;
  OCR2B=PWM2_TOP;

  /*Enable Counters (setting prescaler) */
  TCCR0B=PWM1_PRESCALER;
  TCCR2B=PWM2_PRESCALER;
  
  /* Restore global interrupt flag */
  SREG = sreg;
}



void stopPWM(){
  TCCR0A=0;
  TCCR2A=0;
  digitalWrite(PWM1_PIN,LOW);
  digitalWrite(PWM2_PIN,LOW);
  }

void setup() {

Serial.begin(9600);
Serial.setTimeout(100);
Serial.print("Control command: PWM1_PRESCALER, PWM1_TOP, PWM1_OFFSET,PWM2_PRESCALER, PWM2_TOP, PWM2_OFFSET\n");
Serial.print("Use pin 8 for turning ON/OFF the modulation\n");
pinMode(PWM1_PIN,OUTPUT); //PMW1
pinMode(PWM2_PIN,OUTPUT); //PWM2
pinMode(PWM_ON_PIN,INPUT); //ON OFF switch
pinMode(9,OUTPUT); //Trigger monitor
}

void loop() {

  if(Serial.available()){
  
    String serialString =Serial.readString();
    char serialBuffer[100];
    uint8_t parametersArray[6];
    
    serialString.toCharArray(serialBuffer,100);
   
    char * truncatedString=strtok (serialBuffer,",");
    
    for (int i=0;i<6;i++){
        parametersArray[i]= (uint8_t) atoi(truncatedString);
        truncatedString=strtok (NULL,",");
        Serial.println(parametersArray[i]);
    }
    
    PWM1_PRESCALER=parametersArray[0];
    PWM1_TOP=parametersArray[1];
    PWM1_OFFSET=parametersArray[2];
    PWM2_PRESCALER=parametersArray[3];
    PWM2_TOP=parametersArray[4];
    PWM2_OFFSET=parametersArray[5];
    
    newParametersFlag=true;
  }

  PWM_ON_PIN_value_old=PWM_ON_PIN_value;
  PWM_ON_PIN_value=digitalRead(PWM_ON_PIN);
  positiveEdgeDetected=(PWM_ON_PIN_value!=PWM_ON_PIN_value_old)&PWM_ON_PIN_value;

  if(PWM_ON_PIN_value){
    
    if(newParametersFlag|positiveEdgeDetected){

      digitalWrite(9,HIGH);
      startPWM( PWM1_PRESCALER,PWM1_TOP,PWM1_OFFSET,PWM2_PRESCALER,PWM2_TOP,PWM2_OFFSET);
      newParametersFlag=false;
      
        
    }
  }
  else{
    
    stopPWM();
    digitalWrite(9,LOW);
  }  

 
}
