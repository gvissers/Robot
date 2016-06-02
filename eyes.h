#ifndef EYES_H
#define EYES_H

#include "Arduino.h"

/**
 * Class for distance sensors
 *
 * Class Eyes handles the distance sensors on the robot. It provides functions
 * and handlers to perform distance measurements on a regular basis.
 */
class Eyes
{
public:
	/// Enumeration for the different IR distance sensors
	enum IRSensorNumber: uint8_t
	{
		IR_CENTER,
		IR_LEFT,
		IR_RIGHT,
		IR_COUNT
	};

    /// Constructor
    Eyes():
		_current_US_tick(1), _US_pulse_on(false),
		_current_IR_tick(1), _current_IR_number(IR_CENTER),
		 _US_last_distance(0), _IR_last_distance{0, 0, 0} {}

    /**
     * Handle timer tick
     *
     * Handle a tick from the timer. This function is called from the timer
     * interrupt handler. It will do a distance measurement using the ultrasound
     * sensor, every \c US_trigger_ticks time it is called.
     */
    void ultrasoundTick();
    /**
     * Handle ultrasound response
     *
     * Handle a response to the ultrasound trigger. This function is called
     * from an interrupt handler which is triggered by a change in status on
     * the echo pin of the ultrasound sensor. It calculates the time since the
     * last change, which is then converted to a distance which is stored.
     */
    void handleUltrasoundEcho();
	/**
	 * Handle timer tick
	 *
	 * Handle a tick from the timer. This function is called from the timer
	 * interrupt handler. It will do a distance measurement using an infrared
	 * sensor, every \c US_trigger_ticks time it is called, and will cycle
	 * through the different IR sensors.
	 */
	void infraredTick();
	/**
	 * Measure distance using IR
	 *
	 * Measure the current distance from infrared sensor \a which. This
	 * function is called periodically with alternating values for \a which.
	 * \param which The number of the IR sensor to measure
	 */
	void infraredMeasure(IRSensorNumber which);

    /// Return the value of the last successful measurement
    volatile uint16_t distance() const
	{
		uint16_t dc = _IR_last_distance[0],
			dl = _IR_last_distance[1],
			dr = _IR_last_distance[2];
		return min(dc, min(dl, dr));
	}

private:
    /// Current time tick. When it reaches zero, a new ultrasound pulse is sent
    uint16_t _current_US_tick;
    /// Whether the ultrasound trigger pulse is currently on
    bool _US_pulse_on;
    /// Start time (microseconds) of the last echo pulse
    uint32_t _US_echo_start;
	/// Current time tick. When it reaches zero, a new IR distance measurement is executed
	uint16_t _current_IR_tick;
	/// Which IR sensor to poll next
	IRSensorNumber _current_IR_number;
    /// Last successful distance reading from the ultrasound sensor
    volatile uint16_t _US_last_distance;
    /// Last successful distance reading from the infrared sensors (center, left, right)
    volatile uint16_t _IR_last_distance[IR_COUNT];

	/**
	 * Convert voltage to centimeters
	 *
	 * Convert a voltage from the infrared sensor to the corresponding distance
	 * in centimeters.
	 * \param v The voltage from the sensor
	 * \return The distance corresponding to \a v
	 */
	static unsigned int infraredVoltToCm(unsigned int v);

};

#endif // EYES_H
