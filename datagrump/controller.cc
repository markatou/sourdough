#include <iostream>
#include <stdlib.h>
#include <math.h>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug )
{}

unsigned int the_window_size = 15;

uint64_t rtt_avg = 0;
double rtt_stdev = 0.0;

uint64_t prev_rtts[5] = {0, 0, 0, 0, 0};
uint64_t* ptr = prev_rtts;
int idx = 0; // keep track of last rtt we changed

double gain = 0.2;

/* Get current window size, in datagrams */
unsigned int Controller::window_size( void )
{
  /* Default: fixed window size of 100 outstanding datagrams */
  //unsigned int the_window_size = 50;

  if ( debug_ ) {
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

  if ( debug_ ) {
    cerr << "At time " << send_timestamp
	 << " sent datagram " << sequence_number << endl;
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

  uint64_t current_rtt = timestamp_ack_received - send_timestamp_acked;

  rtt_avg = (rtt_avg * 2 + current_rtt * 8) / 10;

  ptr[idx] = current_rtt;
  idx = (idx + 1) % 5;
  
  double current_stdev;

  // get standard deviation
  double sum = 0;
  double sq_sum = 0;
  for(int i = 0; i < 5; i++) {
     sum += ptr[i];
     sq_sum += ptr[i] * ptr[i];
  }

  double mean = sum / 5;
  double variance = (sq_sum / 5) - mean * mean;
  current_stdev = sqrt(variance); 

  rtt_stdev = (rtt_stdev * gain + current_stdev * (1.0 - gain));

  cerr << "current_rtt : " << current_rtt << endl;
  cerr << "rtt_avg: " << rtt_avg << endl;
  cerr << "current_stdev: " << current_stdev << endl;
  cerr << "rtt_stdev : " << rtt_stdev << endl;

  if (current_rtt >  rtt_avg + (uint64_t) (0.25 * rtt_stdev)) { the_window_size /= 2; }

  the_window_size++;

}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms( void )
{
  return 1000; /* timeout of one second */
}
