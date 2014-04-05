#include  "stdio.h"
#include "hal.h"
#include "string.h"
#include "GLOBALES.h"

typedef struct DATA_COMM DATA_COMM;
struct DATA_COMM
{
	int 	tangage;
	int 	roulis;
	int 	lacet;
	int 	altitude;
	float battery;
	float signal;
};

void test( void );
void sendData (DATA_COMM *data );
