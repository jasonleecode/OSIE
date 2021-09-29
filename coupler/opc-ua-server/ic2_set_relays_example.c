// Example controling MOD-IO's relays' state 
// Based on https://www.kernel.org/doc/Documentation/i2c/dev-interface
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main()
{
   int file;
   int adapter_nr = 1; /* probably dynamically determined */
   char filename[20];

   // step 1: open device
   snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
   file = open(filename, O_RDWR);
   if (file < 0) {
     /* ERROR HANDLING; you can check errno to see what went wrong */
     printf("Error openind i2c device.");
     exit(1);
   }
  
   // step 2: address the slave by its address 
   int addr = 0x58; /* The I2C address */
   if (ioctl(file, I2C_SLAVE, addr) < 0) {
    /* ERROR HANDLING; you can check errno to see what went wrong */
    printf("Error addressing i2c slave.");
    exit(1);
   }

   // step 3: write command over I2c
   __u8 reg = 0x10; /* Device register to access */
   __s32 res;
   char buf[10];
   buf[0] = reg;
   buf[1] = 0x05; //0x00 -all off, 0x0F - all 4 on
   buf[2] = 0x65; // seems irrelevant the value
   if (write(file, buf, 3) != 3) {
     /* ERROR HANDLING: i2c transaction failed */
     printf("Error writing to i2c slave."); 
   }

   printf("OK\n");
   return 0;
}
