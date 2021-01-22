/**
 * @file   airport.h
 * @brief  Header file for airport type in ECE 8400/ENGI 9875 assignment 1
 */

#ifndef AIRPORT_H_
#define AIRPORT_H_

#include <stdint.h>

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
int		ap_add_flight(struct flight*);

/** Increment a airport's refcount. */
void		ap_hold(struct airport*);

/** Decrement a airport's refcount (optionally freeing it). */
void		ap_release(struct airport*);

/** Retrieve the current reference count of a airport. */
unsigned int	ap_refcount(const struct airport*);

#endif