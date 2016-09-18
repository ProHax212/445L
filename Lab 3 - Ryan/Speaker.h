//Speaker.h

// Starts the alarm
void Start_Alarm(void);

// Stops the alarm
void Stop_Alarm(void);

// Initializes the speaker module
// The period sets the period of the sqaure wave for sound output
void Speaker_Init(void);

// Change the alarm to the next available alarm
void Change_Alarm(int x);
