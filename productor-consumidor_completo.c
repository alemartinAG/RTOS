#include "board.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define SIZE 16

typedef struct{
    char * message;
    int temperature;
}Data;

/*****************************************************************************
 * Variables
 ****************************************************************************/

static const char characters[256] = "abcdefghijklmnopqrstuvwxyz0123456789 ";

xQueueHandle queue;

/*****************************************************************************
 * Functions
 ****************************************************************************/

/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();
}

/* Temperature producer Thread */
static void temp_producer(void *pvParameters) {

    Data * tempData;
	tempData = malloc(sizeof(Data *));

	tempData->message = NULL;

	while (1) {

		tempData->temperature = rand() % 100;

		xQueueSend(queue, (Data *) &tempData, 5);

		vTaskDelay(30 / portTICK_RATE_MS);
	}
}

/* Command producer Thread */
static void str_producer(void *pvParameters) {

    Data * mesgData;
	mesgData = malloc(sizeof(Data *));
	mesgData->message = malloc(SIZE);
	mesgData->temperature = NULL;

	char str[SIZE] = "";
	int tam;
	int element;

	while (1) {

		tam = (rand() % (SIZE-1)) + 1;

		for(int i=0; i<tam; i++){
			element = rand() % ((int) strlen(characters));
			str[i] = characters[element];
		}

		str[tam] = '\0';

		strcpy(mesgData->message, str);

		xQueueSend(queue, (Data *) &mesgData, 5);
		//TODO: should be non-periodic
		vTaskDelay((10 * ((rand() % 10) + 6) / portTICK_RATE_MS) );
	}
}

/* Consumer Thread */
static void consumer(void *pvParameters) {

	Data * dataPtr;

	while (1) {
		if(xQueueReceive(queue, &dataPtr, portMAX_DELAY)){

			if(dataPtr->message != NULL){
				printf("Command: %s\n", dataPtr->message);
			}
			if(dataPtr->temperature != NULL){
				printf("Core temperature: %2d\n", dataPtr->temperature);
			}
		}
	}
}

/*****************************************************************************
 * main
 ****************************************************************************/

int main(void)
{
	prvSetupHardware();

	vTraceEnable(TRC_START);

	/* Initialize the Queue */
	queue = xQueueCreate(10, sizeof(Data *));

	srand(time(NULL));


	/* Buffer producer threads */
	xTaskCreate(temp_producer, (signed char *) "vTaskP1",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 3UL),
				(xTaskHandle *) NULL);

	xTaskCreate(str_producer, (signed char *) "vTaskP2",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2UL),
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

