#include <iostream>
#include <stdlib.h>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug )
{}

unsigned int the_window_size = 50;
int64_t last_rtt = 0;

// value in range [1, 150] that determines if window size will be increased or decreased
//  if random number drawn from same range is less than window_change_prob then we decrease
//  window size (and vice versa)
int window_change_prob = 75;

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
  int64_t delta_rtt =  current_rtt - last_rtt;

  // if we're seeing delay increase, want smaller window (and vice versa)
  if (delta_rtt > 0) { window_change_prob++; }
  else { window_change_prob--; }

  if (window_change_prob < 1) { window_change_prob = 10; }
  if (window_change_prob > 150) { window_change_prob = 140; }

  // get random number in range [1, 150]
  int rand_draw = (rand() % 150) + 1;

  // if rand_draw less than window_change_prob, decrease window (and vice versa)
  if (rand_draw <= window_change_prob) { the_window_size /= 2 ; }
  else { the_window_size++ ; }

  last_rtt = current_rtt;
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms( void )
{
  return 1000; /* timeout of one second */
}
