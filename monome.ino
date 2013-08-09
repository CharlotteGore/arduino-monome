#include <LedControl.h>

LedControl lc = LedControl(51, 52, 53);

volatile byte cols[8]={
 0,0,0,0,0,0,0,0
};

volatile byte debounce[8]={
0,0,0,0,0,0,0,0
};

static byte notes[8] = {
  42,
  44,
  46,
  66,
  68,
  70,
  85,
  87
};

static byte channels[8] = {
  3,
  3,
  3,
  2,
  2,
  2,
  1,
  1
};

void setup(){
  lc.shutdown(0, false);
  lc.setIntensity(0,8);
 lc.clearDisplay(0);
 
 //lc.setLed(0,2,7,true);
  for(int i = 0; i < 8; i++){
    lc.setRow(0,i,cols[i]);
  }
  

    DDRA = DDRA | B11111100;  
    pinMode(38, INPUT);
  
  cli();
  
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
  
	OCR1A = 15624;
	TCCR1B |=(1 << WGM12);
	TCCR1B |=(1 << CS11) | (1 << CS10);
	TIMSK1 |= (1 << OCIE1A);
  sei();

  Serial.begin(115200);
  Serial1.begin(31250);
}


void loop(){
  
   boolean val = 1;
   byte cc;
  for(int i = 0; i < 8; i++){
    
    for(int j = 0;j < 8; j++){
      cc = B10010000 | j;
      if(bitRead(cols[i], j)){
        
        Serial1.write(cc);
        Serial1.write(notes[j]);
        Serial1.write(0x7F);

         
      }else{        
        
        Serial1.write(cc);
        Serial1.write(notes[j]);
        Serial1.write(0x00);
      }
    }  
    
      for(int j = 0; j < 12; j++){
        lc.setRow(0,i,B11111111 ^ cols[i]);
        delay(20);
        lc.setRow(0,i,B11111111);
        delay(20);
      }
      
     lc.setRow(0,i, cols[i]);
  }

}

ISR(TIMER1_COMPA_vect){
  
  boolean val;
 
   for(byte k = 0; k < 8; k++){
      // j is the row
      for(byte j = 0; j < 8; j++){
        PORTA = (j << 5) | (k << 2);
        delayMicroseconds(1);
        val = digitalRead(38);
        
        if(val==0){
          
            if( !bitRead(debounce[k], 7 -j) ){
              
             bitSet(debounce[k], 7 - j); 
             
             val = bitRead(cols[k], 7 - j);
            
              if(val){
                
                bitClear(cols[k], 7 -j);
                            
              }else{
            
                 bitSet(cols[k], 7 - j);
                
              }
              
              lc.setRow(0,k, cols[k]);
              
            }
           
        }else{
            //cols[k] = (0 << j);
            bitClear(debounce[k], 7 - j);
        }
        
      }
      
    }
   
  }
  
