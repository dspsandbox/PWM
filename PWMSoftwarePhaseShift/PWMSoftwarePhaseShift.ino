#define PWM_ARRAY_LEN 100
#define PWM_ON_PIN 8
#define PWM_ON_PIN_MONITOR 9
#define PWM1_PIN 3
#define PWM2_PIN 4

bool PWM_ON_PIN_value=false;


uint8_t PWM1_time=2;
uint8_t PWM2_time=2;
uint8_t PWM_period=4;
uint8_t PWM_offset=2;

uint8_t PWMArray1[PWM_ARRAY_LEN];
uint8_t PWMArray2[PWM_ARRAY_LEN];

int i;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(100);
  Serial.print("Control command: PWM1_time, PWM2_time, PWM_period, PWM_offset\n");
  Serial.print("Time resolution: approx. 3.3us \n");
  Serial.print("Use pin 8 for turning ON/OFF the modulation\n");
  
  pinMode(PWM1_PIN,OUTPUT); //PMW1
  pinMode(PWM2_PIN,OUTPUT); //PWM2
  pinMode(PWM_ON_PIN,INPUT); //ON OFF switch
  pinMode(PWM_ON_PIN_MONITOR,OUTPUT); //ON OFF switch monitor
  
 
    for (i=0;i<PWM_period;i++){ 
      if(i<PWM1_time){
      PWMArray1[i]=1;
      }
      else{
      PWMArray1[i]=0;  
      }
    
      if((i>=PWM_offset)&&(i<(PWM2_time+PWM_offset))){
      PWMArray2[i]=1;
      }
      else{
      PWMArray2[i]=0;  
      }
   }
}


void loop() {

  if(Serial.available()){
  
    String serialString =Serial.readString();
    char serialBuffer[100];
    uint16_t parametersArray[4];
    
    serialString.toCharArray(serialBuffer,100);
   
    char * truncatedString=strtok (serialBuffer,",");
    
    for (i=0;i<4;i++){
        parametersArray[i]= (uint16_t) atoi(truncatedString);
        truncatedString=strtok (NULL,",");
        Serial.println(parametersArray[i]);
    }
    
    PWM1_time=parametersArray[0];
    PWM2_time=parametersArray[1];
    PWM_period=parametersArray[2];
    PWM_offset=parametersArray[3];
    
    for (i=0;i<PWM_period;i++){ 
      if(i<PWM1_time){
      PWMArray1[i]=1;
      }
      else{
      PWMArray1[i]=0;  
      }
    
      if((i>=PWM_offset)&&(i<(PWM2_time+PWM_offset))){
      PWMArray2[i]=1;
      }
      else{
      PWMArray2[i]=0;  
      }
   }
   
    
  }

  PWM_ON_PIN_value=(PINB>>0)&1; //Reading PWM_ON_PIN
  if(PWM_ON_PIN_value){
    PORTB|=(1<<1); //Setting PWM_ON_PIN_monitor to 1
    unsigned char sreg = SREG;
    cli();
    i=0;
    LOOP:
      PORTD=((PWMArray1[i]<<3)|(PWMArray2[i]<<4));
      i++;
      if (i==PWM_period){
        i=0;}
      PWM_ON_PIN_value=(PINB>>0)&1; //Reading PWM_ON_PIN
      
      if(PWM_ON_PIN_value){
          goto LOOP;}
    
    SREG=sreg;
  }
  else{
     PORTD=0;
     PORTB&=(0xFF-(1<<1));
  }  
   
}
