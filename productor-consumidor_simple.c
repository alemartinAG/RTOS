#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*****************************************************************************
 * Variables
 ****************************************************************************/

static char buffer[256] = "b";

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
static void producer(void *pvParameters) {

	while (1) {

		strcat(buffer, "a");
		printf("%s\n", buffer);
		/* About a 6Hz period */
		vTaskDelay(10 / portTICK_RATE_MS);
	}
}

/* Consumer Thread */
static void consumer(void *pvParameters) {

	while (1) {

		strcpy(buffer, "b");
		printf("%s\n", buffer);
		/* About a 1Hz period */
		vTaskDelay(80 / portTICK_RATE_MS);
	}
}

/*****************************************************************************
 * main
 ****************************************************************************/

int main(void)
{
	prvSetupHardware();

	/* Buffer producer thread */
	xTaskCreate(producer, (signed char *) "vTaskP1",
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

