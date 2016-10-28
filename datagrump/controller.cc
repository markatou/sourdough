#include <iostream>

#include "controller.hh"
#include "timestamp.hh"
#include <math.h>  
#include <map>
#include <list>
#include <algorithm>

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug )
{}
 
/* Get current window size, in datagrams */
unsigned int the_window_size = 14;
unsigned int currentRTT = 0; 
float  ewma = 0; 
unsigned int timeOut = 0;
float stdev = 1;
int time_last_datagram_was_sent = 0;

map<unsigned int, unsigned int> m;

unsigned int Controller::window_size( void )
{
  /* Default: fixed window size of 100 outstanding datagrams */
 

  if ( false ) {
    cerr << "At time " << timestamp_ms()
	 << " window size is " << the_window_size << endl;
  }

  return the_window_size;
}

/* A datagram was sent */
void Controller::datagram_was_sent( const uint64_t sequence_number,
				    /* of the sent datagram */
				    const uint64_t send_timestamp )
                                    /* in milliseconds */
{
  /* Default: take no action */
  if ( false ) {
    cerr << "At time " << send_timestamp
	 << " sent datagram " << sequence_number  
         << " last datagrap " << time_last_datagram_was_sent << endl;
  }
}

/* An ack was received */
void Controller::ack_received( const uint64_t sequence_number_acked,
			       /* what sequence number was acknowledged */
			       const uint64_t send_timestamp_acked,
			       /* when the acknowledged datagram was sent (sender's clock) */
			       const uint64_t recv_timestamp_acked,
			       /* when the acknowledged datagram was received (receiver's clock)*/
			       const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */

{
  /* Default: take no action */

  if ( debug_ ) {
    cerr << "At time " << timestamp_ack_received
	 << " received ack for datagram " << sequence_number_acked
	 << " (send @ time " << send_timestamp_acked
	 << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
	 << endl;
  }


   
  /* Try to find a better signal*/
  //int i = rand() % 100;  
  currentRTT = abs ( (int) timestamp_ack_received - (int) send_timestamp_acked);
  
  
  if ( currentRTT > (uint) (9 + ewma) ) { 
    the_window_size = (uint) the_window_size/2;
  } 
  
  the_window_size = the_window_size  + 1; 
   
  float error;
  if ( currentRTT > ewma) {
    error = currentRTT -ewma;
  } else {
    error = ewma - currentRTT;
  }
  stdev =  (float) (1*stdev + 9*error)/10;
  ewma = (float) (90*currentRTT+10*ewma)/100;

 // cerr << " ave is " << ewma << " dev is " << stdev <<" and the error " << error<< endl;
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms( void )
{ 
  if (timeOut ==  0) {
     timeOut = 1000;
  } else {
     timeOut = (90*timeOut + 10*(ewma))/100;
  }
  return timeOut; /* timeout of one second */
}
