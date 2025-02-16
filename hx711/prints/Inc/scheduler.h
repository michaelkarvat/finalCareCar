#ifndef SCHEDULER_H
#define SCHEDULER_H

// The "half-period" of the blinking, in number of timer ticks.
// Since your timer ticks every 1 second,
// a value of 1 means the LED toggles every second (on/off).
#define SCHEDULER_BLINK_HALF_PERIOD_IN_SEC 1

// After this many seconds, we print "Boring...\n"
#define SCHEDULER_BORING_PERIOD_IN_SEC     5

// Start blinking the LED periodically.
void SCHEDULER_blink(void);

// Called every time TIMER2_expired() is TRUE (every 1 second).
// Handles blinking and prints "Boring..." after some period.
void SCHEDULER_handle(void);

// Stop blinking the LED.
void SCHEDULER_stopBlinking(void);

#endif // SCHEDULER_H
