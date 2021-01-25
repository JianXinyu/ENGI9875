/**
 * @file   flight.h
 * @brief  Header file for flight types in ECE 8400/ENGI 9875 assignment 1
 */

#ifndef FLIGHT_H_
#define FLIGHT_H_

#include <stdbool.h>
#include <stdint.h>


struct aircraft;
struct airport;


/** A flight between two cities. */
struct flight {
    /** Null-terminated airline code of the airline operating the flight. */
    char		f_airline[3];

    /** Flight number. */
    uint16_t	f_number;

    /** Owning reference to the aircraft operating this flight. */
    struct aircraft	*f_aircraft;

    /** Number of stops this flight makes (length of f_stops). */
    unsigned int	f_stop_count;

    /** Airports this flight stops at (in order). */
    struct airport	*f_stops;
};


/**
 * Create a new flight.
 *
 * The airline and flight number will be copied into the new structure.
 * Stops (stored in a null-terminated array) are copied in without referencing
 * the airports, which are assumed to live at least as long as the flight.
 */
struct flight*	flight_create(const char *airline, uint16_t number,
                                 struct airport *stops[]);

/**
 * Release a flight.
 *
 * Takes no action when passed a NULL flight.
 */
void		flight_free(struct flight*);

/**
 * Change (or set) a flight's aircraft.
 *
 * This takes a reference to the aircraft and stores it. Any existing
 * equipment's refcount is decremented. A flight can have its equipment cleared
 * by passing NULL to this function.
 */
void		flight_change_equipment(struct flight*, struct aircraft*);

/**
 * Construct a human-readable flight number.
 *
 * For example, a flight that was created with airline "AC\0\0" and number 860
 * would have a human-readable flight number "AC860". Returns a
 * dynamically-allocated string that the caller is responsible for freeing.
 */
char*		flight_code(const struct flight*);

#endif