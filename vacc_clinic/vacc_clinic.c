/*
 * Name: Josiah Ryan
 * Date 03/22/21
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "rendezvous.c"

#define NUM_VIALS 10
#define SHOTS_PER_VIAL 6
#define NUM_CLIENTS (NUM_VIALS * SHOTS_PER_VIAL)
#define NUM_NURSES 10
#define NUM_STATIONS NUM_NURSES
#define NUM_REGISTRATIONS_SIMULTANEOUSLY 4

/* global variables */
int stationsBoundedBuffer[NUM_NURSES];
int stationsIn = 0;
int stationsOut = 0;

int numRemainingVials = NUM_VIALS;
int vialSemaphore = 1;  //unlocked semaphore for modifying NUM_VIALS

int registrationSemaphore = NUM_REGISTRATIONS_SIMULTANEOUSLY;
struct rendezvous rendezvousArray[NUM_NURSES];  //see rendezvous.c for description


int get_rand_in_range(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

char *time_str;
char *curr_time_s() {
    time_t t;
    time(&t);
    time_str = ctime(&t);
    // replace ending newline with end-of-string.
    time_str[strlen(time_str) - 1] = '\0';
    return time_str;
}

// lower and upper are in seconds.
void walk(int lower, int upper) {
    int num = (rand() % (upper - lower + 1)) + lower;
    usleep(num * 10000);  // pauses thread for num seconds  1000000
}


// Info: called by client, it blocks when the bounded-buffer is full.  subsequent client calls also block on the "full" semaphores
// When the bounded-buffer is not full, it increments the station buffer's in pointer and returns...
// Returns: the number of the station the client went to 
int wait_for_station_assignment() {
    while ((stationsIn+1) % NUM_STATIONS == stationsOut) {  // if the semaphore is full, it waits
        //spin loop
    }
    stationsIn = (stationsIn + 1) % NUM_STATIONS;

    return ((stationsIn - 1 + NUM_STATIONS) % NUM_STATIONS); 
}

// called after 'wait_for_station_assignment() to release an item from the buffer ( +1 to the stations out pointer)
void signal_wait_queue(int station_id) {
    stationsOut = (stationsOut + 1) % NUM_STATIONS;
    vaccineRecieved(rendezvousArray, station_id);
}

// this protects the registration desk from overflow using the registraionSemaphore
void regDeskWait() {
    while (registrationSemaphore <= 0) {
        //spin loop
    }
    //registrationSemaphore is > 0, which means a registration desk is open
    registrationSemaphore -= 1;
}

// called after 'regDeskWait()' and client has left the registration desk.  This opens a new space for a new client
void regDeskRelease() {
    registrationSemaphore += 1;
}

// This is a protected modifier function for numRemainingVials.  This stops multiple nurses from modifying it at once.  
void takeVial() {
    while (vialSemaphore == 0) {
        //spin loop
    }
    vialSemaphore -= 1;
    numRemainingVials -= 1;
    vialSemaphore += 1;
}

// arg is the nurses station number.
// nurse thread.  
void *nurse(void *arg) {
    long int id = (long int)arg;
    fprintf(stderr, "%s: nurse %ld started\n", curr_time_s(), id);
    fprintf(stderr, "%s: nurse %ld is walking to get a vial\n", curr_time_s(), id);
    walk(1, 3); // walk a random amount of time between 1 and 3 seconds to get a vial of vaccine

    while (numRemainingVials > 0) {
        takeVial();
        fprintf(stderr, "%s: nurse %ld got vial. Num vials left = %d\n", curr_time_s(), id, numRemainingVials);
        int vaccsLeft = 6; // nurse gives 6 vaccinations per vial
        walk(1, 3); // walk back (again between 1 and 3 seconds)
        fprintf(stderr, "%s: nurse %ld back at their station\n", curr_time_s(), id);

        while (vaccsLeft > 0) {
            fprintf(stderr, "%s: nurse %ld signals the next client that he/she is available\n", curr_time_s(), id); 
            nurseArrivial(rendezvousArray, id);//  indicate to the queue of clients waiting for a station assignment that you are ready for the next client.
            fprintf(stderr, "%s: nurse %ld is ready to vaccinate the next client\n", curr_time_s(), id); 
            nurseWaitForClient(rendezvousArray, id); // wait for the client to indicate that they are ready to be vaccinated
            usleep(100);  //makes reinitializationRendevous work seemlessly
            reinitalizeRendezvous(rendezvousArray, id); // resets initial rendezvous values so next rendezvous can happen
            fprintf(stderr, "%s: nurse %ld sees client is ready. Giving shot now\n", curr_time_s(), id);
            
            walk(5, 5); // give the client a vaccination (which takes 5 seconds to administer)
            fprintf(stderr, "%s: nurse %ld gave client the shot\n", curr_time_s(), id);
            signal_wait_queue(id);  // allows for new client to enter the station assignment buffer
            vaccsLeft -= 1;  // nurse used one vaccine
        }
        // if there are no more vials, leave the clinic.
    }  
    fprintf(stderr, "%s: nurse %ld found all vials are gone: exiting\n", curr_time_s(), id);       
    pthread_exit(NULL);
}

//client thread
void *client(void *arg) {
    long int id = (long int)arg;
    fprintf(stderr, "%s: client %ld has arrived and is walking to register\n", curr_time_s(), id);
    walk(3, 10); //  after arrival (i.e., thread creation), walk a random amount between 3 and 10 seconds to the registration desk.
    fprintf(stderr, "%s: client %ld is waiting to register\n", curr_time_s(), id);

    regDeskWait(); //  wait for an opening at the registration desk.
    fprintf(stderr, "%s: client %ld is registering\n", curr_time_s(), id);
    walk(3,10); //  take a random amount of time between 3 and 10 seconds to register.

    regDeskRelease(); // client is done registering, and the registration desk is now open again
    fprintf(stderr, "%s: client %ld is done registering.  Now walking to the next queue\n", curr_time_s(), id);
    walk(3,10); //  walk a random amount of time between 3 and 10 seconds to get to the station-assignment queue.

    int station_id = wait_for_station_assignment();   //  wait for station assignment
    fprintf(stderr, "%s: client %ld got assigned to station %d: walking there now\n", curr_time_s(), id, station_id);
    walk(1, 2); //  walk a random amount of time between 1 and 2 seconds to get to the assigned nurse's station.

    clientArrivial(rendezvousArray, station_id); // indicate to the nurse that the client is ready for the vaccination
    fprintf(stderr, "%s: client %ld has arrived at nurse station %d, and is ready for their shot from nurse %d\n", curr_time_s(), id, station_id, station_id);
    clientWaitForNurse(rendezvousArray, station_id); // waits for nurse to arrive 

    fprintf(stderr, "%s: client %ld got the shot! It hurt, but it was a sacrifice they were willing to make!\n", curr_time_s(), id);
    fprintf(stderr, "%s: client %ld leaves the clinic!\n", curr_time_s(), id);
    pthread_exit(NULL); //  leave the clinic.
}


int main() {
    for (int i = 0; i < 10; i++) {  // initializes the rendezvous array
        reinitalizeRendezvous(rendezvousArray, i);
    }

    pthread_t ts[NUM_NURSES + NUM_CLIENTS];     //creates threads for all nurses and clients
    srand(time(0));                             //starts/creates time

    for (int i = 0; i < NUM_NURSES; i++)    {   
		pthread_create(&ts[i],NULL, nurse, i);  //assigns all nurse threads
    }

    for (int i = 0; i < NUM_CLIENTS; i++)    {  
		pthread_create(&ts[i], NULL, client, i);  //assigns all client threads
    }

    pthread_exit(0);
}
