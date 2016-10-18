#include <iostream>

#include "controller.hh"
#include "timestamp.hh"
#include <math.h>  

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug )
{}

/* Get current window size, in datagrams */
unsigned int the_window_size = 14;
unsigned int tmp = 0;
unsigned int tmp2 = 0;
unsigned int time_last_datagram_was_sent = 0;

unsigned int Controller::window_size( void )
{
  /* Default: fixed window size of 100 outstanding datagrams */
 

  if ( true ) {
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
  /* Simple AIMD   
  if ( send_timestamp > 1000 + time_last_datagram_was_sent ) {
     the_window_size = the_window_size/2 + 1;
  } else {
    the_window_size = the_window_size + 1;
  } 
  time_last_datagram_was_sent = send_timestamp;


  if ( send_timestamp > 990 + time_last_datagram_was_sent ) {
     the_window_size = the_window_size/2 + 1;
     cerr << "At time " << send_timestamp << " had to reduce window due to Time-out" << endl;
     time_last_datagram_was_sent = send_timestamp;

  }*/

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

   
  /*AIMD*/
  tmp = send_timestamp_acked-recv_timestamp_acked;
  the_window_size = the_window_size +2 ;
  if ( tmp < tmp2 - 3) {
    the_window_size = the_window_size/2+1;
  } else {
     the_window_size = the_window_size + 1;
  }     
  cerr << "New Time " << tmp << " Old time " << tmp2 << endl;
  tmp2 = tmp;

}



/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms( void )
{
  return 1000; /* timeout of one second */
}
