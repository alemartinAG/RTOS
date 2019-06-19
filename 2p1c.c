#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define SIZE 16

/*****************************************************************************
 * Variables
 ****************************************************************************/

static const char buffer[256] = "abcdefghijklmnopqrstuvwxyz0123456789 ";

xQueueHandle xQueue = NULL;

/*****************************************************************************
 * Otras Funciones
 ****************************************************************************/

/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();
}

/* Producer Thread */
static void temp_producer(void *pvParameters) {

	int temperatura;
	char tempe[SIZE] = "";

	while (1) {

		temperatura = rand() % 100;
		itoa(temperatura, tempe, 10);
		//sprintf(tempe, "%d", temperatura);
		printf("temp: %s\n", tempe);

		xQueueSend(xQueue, tempe, 0);
		/* About a 6Hz period*/
		vTaskDelay(configTICK_RATE_HZ / 6);
	}
}

/* Producer Thread */
static void str_producer(void *pvParameters) {

	int tam;
	char str[SIZE] = "";
	int element;

	while (1) {

		tam = rand() % SIZE-1;
		memset(str, 0, SIZE-1);

		for(int i=0; i<tam; i++){
			element = rand() % (int) strlen(buffer);
			strcat(str, &buffer[element]);
			printf("%c", buffer[element]);
		}

		xQueueSend(xQueue, str, 0);
		printf("\n");
		//TODO: should be non-periodic
		vTaskDelay(configTICK_RATE_HZ / (rand() % 6));
	}
}

/* Consumer Thread */
static void consumer(void *pvParameters) {

	void * ptr;

	while (1) {
		if(xQueueReceive(xQueue, ptr, 0) == pdTRUE){
			printf("%s\n", ptr);
		}
	}
}

/*****************************************************************************
 * main
 ****************************************************************************/

int main(void)
{
	prvSetupHardware();

	srand(time(NULL));

	xQueue = xQueueCreate(10, sizeof(void *));

	/* Buffer producer thread */
	xTaskCreate(temp_producer, (signed char *) "vTaskP1",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

		xTaskCreate(str_producer, (signed char *) "vTaskP2",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	/* Buffer consumer thread */
	xTaskCreate(consumer, (signed char *) "vTaskC1",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}
