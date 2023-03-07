/* Rendezvous struct and functions
 * This is an implementaion of a binary semaphore for rendezvous
 * 
 * the rendezvousArray is an array of rendezvous, one for each nurse.  This way the client and nurse know where and how to communicate with
 *   each other about their rendezvous
 * 
 * Name: Josiah Ryan
 * Date 03/24/21
 */

// the structure holds the arrival values of two communicating threads: nurse and client, 
// and a recievedVaccine value to release the client only when the shot is finished
struct rendezvous {
    int nurseArrived;
    int clientArrived;
    int recievedVaccine; 
};

// called by client, indicates that the client is ready to rendezvous
// recieve: the array of rendezvous, and the number in the array of the rendezvous (the nurses/stations number)
void clientArrivial(struct rendezvous rendezvousArray[], long int stationNumber) {
    rendezvousArray[stationNumber].clientArrived = 1;
}

// called by client after it has signaled it's arrival, this forces the client to wait until the nurse arives and is done giving the vaccine
// recieve: the array of rendezvous, and the number in the array of the rendezvous (the nurses/stations number)
void clientWaitForNurse(struct rendezvous rendezvousArray[], long int stationNumber) {
    while (rendezvousArray[stationNumber].nurseArrived == 0 || rendezvousArray[stationNumber].recievedVaccine == 0) {
        // spin loop
    }
}

// called by nurse, indicates that the nurse is ready to rendezvous
// recieve: the array of rendezvous, and the number in the array of the rendezvous (the nurses/stations number)
void nurseArrivial(struct rendezvous rendezvousArray[], long int stationNumber) {
    rendezvousArray[stationNumber].nurseArrived = 1;
}

// called by nurse after they have signaled their arrival, this forces the nurse to wait until the client arrives
// recieve: the array of rendezvous, and the number in the array of the rendezvous (the nurses/stations number)
void nurseWaitForClient(struct rendezvous rendezvousArray[], long int stationNumber) {
    while (rendezvousArray[stationNumber].clientArrived == 0) {
        // spin loop
    }
}

// called after a rendezvous.  It resets all the values to their initial state
// recieve: the array of rendezvous, and the number in the array of the rendezvous (the nurses/stations number)
void reinitalizeRendezvous(struct rendezvous rendezvousArray[], long int stationNumber) {
    rendezvousArray[stationNumber].clientArrived = 0;
    rendezvousArray[stationNumber].nurseArrived = 0;
    rendezvousArray[stationNumber].recievedVaccine = 0;
}

// called by nurse after they finish giving vaccine, this will releases the client from the busy wait loop
// recieve: the array of rendezvous, and the number in the array of the rendezvous (the nurses/stations number)
void vaccineRecieved(struct rendezvous rendezvousArray[], long int stationNumber) {
    rendezvousArray[stationNumber].recievedVaccine = 1;
}