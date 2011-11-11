
#define LED1   LATEbits.LATE5
#define SD     LATEbits.LATE4
#define OE     LATEbits.LATE3
#define DC_EN  LATEbits.LATE2
#define CANSTB LATEbits.LATE1

void PINs_Init();
void PINs_test();
void LED1_test();
void LED1_toggle();
