/**
 * @file   airport.h
 * @brief  Header file for airport type in ECE 8400/ENGI 9875 assignment 1
 */

#ifndef AIRPORT_H_
#define AIRPORT_H_

#include <stddef.h>

#include "flight.h"


/** Opaque airport type. */
struct airport;


/** Create a new airport. */
struct airport*	ap_create(const char *icao_code);

/** Retrieve a airport's ICAO code. */
const char*	ap_code(const struct airport*);

/**
 * Add a flight to an airport's list of flights.
 *
 * @returns     0 on success, -1 if flight is invalid (NULL pointer or does
 *              not include this airport as a stop) or on failure to
 *              allocate memory
 */
int		ap_add_flight(struct airport*, struct flight*);

/**
 * Copy pointers to flights into a caller-provided array.
 *
 * @param   ap      The airport to query
 * @param   fpp     Array of flight pointers to copy flight data into
 * @param   n       Number of elements in the passed-in array. If too small
 *                  to hold all of the flights, this will be set to the minimum
 *                  size required.
 */
void		ap_flights(struct airport *ap, struct flight **fpp, size_t *n);

/** Increment a airport's refcount. */
void		ap_hold(struct airport*);

/** Decrement a airport's refcount (optionally freeing it). */
void		ap_release(struct airport*);

/** Retrieve the current reference count of a airport. */
unsigned int	ap_refcount(const struct airport*);

#endif