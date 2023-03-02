#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * Simple C program which tests system latency by writing every cycle 1 or 0
 * to respective GPIO (configured to STMP157-OLinuXino-LIME2 with STMP15X-SHIELD)
 */
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

int main() {
	int fd;
	struct gpiohandle_request led;
	struct gpiohandle_data data;

	 /* Schema
	 * GND: pin 2
	 *    : pin 9
	 * */

	fd = open("/dev/gpiochip1", O_RDWR);
	if(fd < 0) {
		perror("Error opening gpiochip");
		return -1;
	}

	/* Setup LED to output */
	led.flags = GPIOHANDLE_REQUEST_OUTPUT;
	strcpy(led.consumer_label, "LED");
	memset(led.default_values, 0, sizeof(led.default_values));
	led.lines = 1;
	led.lineoffsets[0] = 7;

	if(ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &led) < 0) {
		perror("Error setting GPIO to output");
		close(fd);
		return -1;
	}

	/* Let's set the LED */
	int i;
	int b = 1;
	for (i=1; i<100000000; ++i) {
	  data.values[0] = b;
	  if (b) b = 0;
	  else b = 1;
	  if(ioctl(led.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) 
		perror("Error setting GPIO to 1");
          //printf("b=%d\n", b);
	  //sleep(2);
        }
	// close fds
	close(fd);
	close(led.fd);
	return 0;
}
