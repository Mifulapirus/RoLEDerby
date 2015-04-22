/****************************************
* RoLEDerby Player Board
* Version = 1.0
* Author: Jesús Dominguez
* Contact @JesusJDC
*         @hirikilabs
*
* Attached Devicesw:
*  - PIN 6 - WS2812
*****************************************/
/*************************
*PIN definition
*************************/
#define DATA_PORT          PORTD
#define DATA_DDR           DDRD						
#define DATA_PIN           6							

/************************
* WS2812 Stuff
*************************/
#define NUMBER_OF_PIXELS   128
static unsigned char *ptr;
static int numLeds = 128;
uint8_t bitmap[384];
int red = 0;
int green = 0;
int blue = 0;

/***********************
* Setup
***********************/
void setup() {
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  
  DATA_DDR |= (1 << DATA_PIN);
  Serial.begin(380400);
  ptr=bitmap;
}

/***********************
* Main Loop
***********************/
void loop() {
    setColor(red*digitalRead(3), green*digitalRead(4), blue*digitalRead(5));

    cli();
    ws2812_sendarray(bitmap, NUMBER_OF_PIXELS * 3); 
    sei();
    
    delay(1000);
}

/***********************
* WS2812 Functions
***********************/
void setColor(int r, int g, int b) {
  for (int i=0; i<numLeds; i++) {
    bitmap[3*i] = g;
    bitmap[3*i+1] = b;
    bitmap[3*i+2] = r;
  }
}

void ws2812_sendarray(uint8_t *data,uint16_t datlen) {
  uint8_t curbyte,ctr,masklo;
  uint8_t maskhi = _BV(DATA_PIN);
  masklo =~ maskhi & DATA_PORT;
  maskhi |= DATA_PORT;

  while (datlen--) {
    curbyte = *data++;
    asm volatile (
      "		ldi %0,8	\n\t"		// 0
      "loop%=:out %2, %3	\n\t"		// 1
      "lsl	%1		\n\t"		// 2
      "dec	%0		\n\t"		// 3
      "		rjmp .+0	\n\t"		// 5
      "		brcs .+2	\n\t"		// 6l / 7h
      "		out %2,%4	\n\t"		// 7l / -
      "		rjmp .+0	\n\t"		// 9
      "		nop		\n\t"		// 10
      "		out %2,%4	\n\t"		// 11
      "		breq end%=	\n\t"		// 12      nt. 13 taken
      "		rjmp .+0	\n\t"		// 14
      "		rjmp .+0	\n\t"		// 16
      "		rjmp .+0	\n\t"		// 18
      "		rjmp loop%=	\n\t"		// 20
      "end%=:			\n\t" 
      :	"=&d" (ctr)
      :	"r" (curbyte), "I" (_SFR_IO_ADDR(DATA_PORT)), "r" (maskhi), "r" (masklo)
    );
  }
}
