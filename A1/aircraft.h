/**
 * @file   aircraft.h
 * @brief  Header file for aircraft type(s) in ECE 8400 / ENGI 9875
 */

#ifndef AIRCRAFT_H_
#define AIRCRAFT_H_

enum aircraft_kind {
    /* A fixed-wing aircraft */
    AC_FIXED,

    /* Gyrocopter */
    AC_GYRO,

    /* A rotary-wing aircraft, i.e., a helicopter */
    AC_ROTARY,

    /* A tilt-rotor aircraft, e.g., an Osprey */
    AC_TILT,
};


/** A type that represents a single aircraft. */
struct aircraft;


/**
 * Create a new aircraft object.
 *
 * @param   kind       kind of aircraft
 * @param   reg        registration code, of form "X-XXXX" (e.g., "C-ABCD")
 */
struct aircraft*	ac_create(enum aircraft_kind kind, const char *reg);

/** Increase an aircraft's reference count. */
void			ac_hold(struct aircraft *);

/** Decrease and possibly free an aircraft. */
void			ac_release(struct aircraft *);

/** Retrieve an aircraft's refcount. */
unsigned int		ac_refcount(const struct aircraft *);

/** Retrieve an aircraft's type (fixed-wing, rotary, etc.). */
enum aircraft_kind	ac_kind(const struct aircraft *);

/**
 * Retrieve an aircraft's registration code.
 *
 * This string is owned by the structure, not the caller, and should not be
 * used beyond the lifetime of the aircraft object.
 */
const char*		ac_registration(const struct aircraft *);

#endif