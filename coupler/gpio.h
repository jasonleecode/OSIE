#include <linux/gpio.h>

// the current GPI state (used for debuging)
static unsigned int CURRENT_GPIO_STATE = 0;

/*
*  variable representing the measurement mode over GPIO
*  0: disabled
*  1: enabled for keep-alive subsystem
*  2: enabled for first i2c0.relay0
*/
static unsigned int CURRENT_GPIO_MODE = 0;

static int setGPIO() {
  /*
   * Set GPIO state (useful for debuging with logical analyzer)
   */
  int fd;
  struct gpiohandle_request led;
  struct gpiohandle_data data;

  /* Schema for STMP15x-Shield
   * GND         : pin 2
   * Channel 0   : pin 9
   */

  fd = open("/dev/gpiochip1", O_RDWR);
  if(fd < 0) {
    perror("Error opening gpiochip");
    return -1;
  }

  /* Setup GPIO to output */
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

  // revert previous state
  if (CURRENT_GPIO_STATE) CURRENT_GPIO_STATE = 0;
  else CURRENT_GPIO_STATE = 1;

  // set actual GPI value
  data.values[0] = CURRENT_GPIO_STATE;
  if(ioctl(led.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0)
    perror("Error setting GPIO to 1");

  close(fd);
  close(led.fd);
  return 0;
}


