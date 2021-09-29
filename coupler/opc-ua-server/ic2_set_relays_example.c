/*
  Example controling MOD-IO's relays' state 
  Based on https://www.kernel.org/doc/Documentation/i2c/dev-interface
  
  To compile:
    root@olimex-rt:~/osie/coupler/opc-ua-server# gcc ic2_set_relays_example.c -o ic2_set_relays_example
    root@olimex-rt:~/osie/coupler/opc-ua-server# ./ic2_set_relays_example
*/

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
   int addr = 0x58; /* The I2C slave's address */
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
   buf[1] = 0x07; // the most right bits represent 4 relays' state: for example 00001111 - means all ON
                  // send as a hexadecimal number
   buf[2] = 0x65; // seems irrelevant the value
   if (write(file, buf, 3) != 3) {
     /* ERROR HANDLING: i2c transaction failed */
     printf("Error writing to i2c slave."); 
   }

   printf("OK\n");
   return 0;
}
