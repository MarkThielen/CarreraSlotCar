#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "carrera.h"
#include "ttyTools.h"

char *portname = "/dev/ttyUSB0";
char *CARRERA_TIMING_QUERY = "\"?";
char *CARRERA_CU_FIRMWARE = "\"0";


int get4Bits(char c) { return (int)(c & 0xf);}

/* unsigned int getTimer (struct STRUCT_CARRERA_LAPINFO *scl ) { */

/*   return  */
/*     (get4Bits(scl->timer[1]) << 28) + */
/*     (get4Bits(scl->timer[0]) << 24) + */
/*     (get4Bits(scl->timer[3]) << 20) + */
/*     (get4Bits(scl->timer[2]) << 16) + */
/*     (get4Bits(scl->timer[5]) << 12) + */
/*     (get4Bits(scl->timer[4]) << 8) + */
/*     (get4Bits(scl->timer[7]) << 4) + */
/*     (get4Bits(scl->timer[6])); */

/* } */


unsigned int getTimer (struct STRUCT_CARRERA_LAPINFO scl ) {
  return 
    (get4Bits(scl.timer[1]) << 28) +
    (get4Bits(scl.timer[0]) << 24) +
    (get4Bits(scl.timer[3]) << 20) +
    (get4Bits(scl.timer[2]) << 16) +
    (get4Bits(scl.timer[5]) << 12) +
    (get4Bits(scl.timer[4]) << 8) +
    (get4Bits(scl.timer[7]) << 4) +
    (get4Bits(scl.timer[6]));
}

void resetCarStati(struct STRUCT_CAR_STATUS *car_stati) {

int carno=0;
 for (carno=0; carno <= 7; carno ++){
   car_stati[carno].car_number=carno;
   car_stati[carno].active=0;
   car_stati[carno].current_laptime=0;
   car_stati[carno].fastest_laptime=0;
   car_stati[carno].laps=0;
 }

}




// -------------------------------------------------------------
//
// -------------------------------------------------------------




void main (int args, char* argv[]){

 struct STRUCT_CARRERA_RESPONSE *carrera_response = 
   (struct STRUCT_CARRERA_RESPONSE *)malloc(sizeof (struct STRUCT_CARRERA_RESPONSE));


 // open tty
 int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);

 // display error if open no successful
 if (fd < 0)
   {
     printf        ("error %d opening %s: %s", errno, portname, strerror (errno));
     return;
   }
 
 // set serial connection parameters
 set_interface_attribs (fd, B19200 , 0);  // set speed to 115,200 bps, 8n1 (no parity)
 set_blocking (fd, 0);                // set no blocking

 unsigned int prev_timer=0;
 unsigned int laps=0;

 // create array of cars on track
 struct STRUCT_CAR_STATUS car_stati[8];
 struct STRUCT_CAR_STATUS *pcar_stati = &car_stati;

 resetCarStati(pcar_stati);

 while (1){

   unsigned int timer = 0, laptime = 0;

   // send request to CU
   write (fd, CARRERA_TIMING_QUERY, strlen(CARRERA_TIMING_QUERY));       

   // reset memory structure
   memset(carrera_response,0,sizeof(struct STRUCT_CARRERA_RESPONSE));

   // read data from tty
   int n = read (fd, carrera_response, sizeof (struct STRUCT_CARRERA_RESPONSE) + 1);

   //printf("Buffer : %s : %i %c \n",carrera_response,n, carrera_response->car_number );


   
   // -----------------------------------------------------------------------
   // - Calculate and show laptime of last car crossing the finish line 
   // -----------------------------------------------------------------------
   if (get4Bits(carrera_response->car_number) >= CARRERA_MIN_CAR_NUMBER 
       && get4Bits(carrera_response->car_number) <= CARRERA_MAX_CAR_NUMBER) {

     unsigned int car_number = get4Bits(carrera_response->car_number);

     // set car information in car status array
     if ( car_stati[car_number].active == 0 )
       car_stati[car_number].active = 1;

     car_stati[car_number].laps++;

     
     // set car number
     car_stati[car_number].car_number = car_number;

     // get current CU timer
     timer = getTimer (((union DATA)carrera_response->data).scl);

     // -----------------------------------------------------
     // previous timer should be set after one lap, if yes
     // we are able to calculate a meaningful lap time
     // -----------------------------------------------------
     if(prev_timer > 0) {
       
       // laptime is substracting the current timer with the previous
       // timer
       laptime = timer - prev_timer;
 
       // set current laptime
       car_stati[car_number].current_laptime = laptime;

       // if last lap time was fastest set fastest laptime to current 
       if ( (car_stati[car_number].current_laptime < 
	     car_stati[car_number].fastest_laptime) || car_stati[car_number].fastest_laptime == 0 )
	 car_stati[car_number].fastest_laptime = 
	   car_stati[car_number].current_laptime;
       
       
       /* // show Car number, current timer and laptime */
       /* printf ("Car #%c T(I): %u LT = %u \n", */
       /* 	  carrera_response->car_number, timer,laptime); */

     }
     // ----------------------------------------------------------
     // set previous time to current to calculate next lap time
     // ----------------------------------------------------------
     prev_timer = timer;
   } 

   // ---------------------------------------------------------------
   // - collect tank mode & status, start light status, tower mode 
   // ---------------------------------------------------------------
   else if (get4Bits(carrera_response->car_number) == CARRERA_TRACK_STATUS_FLAG) {



     /* printf("Track Status : (Fuel Mode)%i Fuel Status (7)%i (0)%i (4)%i (1)%i (5)%i (2)%i (6)%i (3)%i (FuelBM)%i%i (SLS)%i\n", */
     /* 	    get4Bits(((union DATA)carrera_response->data).scts.fuel_mode), */
     /* 	    get4Bits(((union DATA)carrera_response->data).scts.fuel_status[0]), */
     /* 	    get4Bits(((union DATA)carrera_response->data).scts.fuel_status[1]), */
     /* 	    get4Bits(((union DATA)carrera_response->data).scts.fuel_status[2]), */
     /* 	    get4Bits(((union DATA)carrera_response->data).scts.fuel_status[3]), */
     /* 	    get4Bits(((union DATA)carrera_response->data).scts.fuel_status[4]), */
     /* 	    get4Bits(((union DATA)carrera_response->data).scts.fuel_status[5]), */
     /* 	    get4Bits(((union DATA)carrera_response->data).scts.fuel_status[6]), */
     /* 	    get4Bits(((union DATA)carrera_response->data).scts.fuel_status[7]), */
     /* 	    get4Bits(((union DATA)carrera_response->data).scts.fuel_bitmask[0]), */
     /* 	    get4Bits(((union DATA)carrera_response->data).scts.fuel_bitmask[1]), */
     /* 	    get4Bits(((union DATA)carrera_response->data).scts.start_light_status) */

     /* 	    ); */


     

   }

 usleep (50000);

 // display current track status
 int cars=0;
 for (cars=0; cars <= 7 ;cars ++) {
   if ( car_stati[cars].active == 1)
      // show Car number, current timer and laptime
       printf ("(Car#) %u (LastLap) %u (FastestLap) %u (#Laps) %u\n", 
	       car_stati[cars].car_number, 
	       car_stati[cars].current_laptime , 
	       car_stati[cars].fastest_laptime,
	       car_stati[cars].laps);
   

 }


 }





} 





