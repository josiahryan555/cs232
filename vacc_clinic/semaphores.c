//semaphores: a buffer of NUM_STATIONS integers and an in and out "pointer" -- really they are also integers

//global variables
int stationsBoundedBuffer[vacc_clinic.NUM_STATIONS];
int in = 0;
int out = 0;


//int wait_for_station_assignment()


//called by client, it blocks when the bounded-buffer is empty.  subsequent client calls also block on the "full" semaphores
void signal_wait_queue(int station_id) {
    while (in+1 == out % NUM_STATIONS) {  // if the semaphore is locked (some else is using it), it waits
        //spin loop
    }

}


