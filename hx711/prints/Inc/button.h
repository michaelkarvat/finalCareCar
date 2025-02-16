#ifndef BUTTON_H
#define BUTTON_H

#include "types.h"

// Initialize the user button (e.g. on PC13).
void BUTTON_init(void);

// Returns TRUE if the button was just pressed.
// After you call this once, the flag resets to FALSE internally.
BOOL BUTTON_pressed(void);

#endif // BUTTON_H
