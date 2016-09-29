
// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void);

// Get the current time
// Pass this value to GetElapsedTime
int GetCurrentTime(void);

// Assumes clock is at 50MHz
// Get the elapsed time from the start time
// Returns elapsed time in milliseconds
int GetElapsedInMs(int startTime);
