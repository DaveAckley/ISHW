#include <Timer.h>

/* This variable will be modified from within a callback.
 So that the compiler does not optimize it, it needs to
 be declared volatile
 */
static volatile int count = 10;

bool myTimerCallback(void)
{
    count--;
    Serial.println("Timer tick...");
    return count != 0;
}

bool printHelloMessage(void)
{
    Serial.println("Press any key to start.");
    return true;
}

void fatalError(void)
{
    Serial.println("Fatal error!");
    // Endless loop
    for(;;);
}

void setup()
{
    int r;
    Serial.begin(115200);
    Timers.begin();

    // Print message each second.
    r = Timers.periodic(1000, printHelloMessage);
    if (r<0) {
        fatalError();
    }
    while (!Serial.available());
    Serial.read();
    // Cancel timer
    Timers.cancel();

    Serial.println("Starting sketch...");

    r = Timers.periodic(1000, myTimerCallback);
    if (r<0) {
        fatalError();
    }
}

void loop()
{
    Serial.print("Waiting for 10 ticks...\n");
    while (count) {
        /* Do nothing, while count is not zero */
    }
    Serial.println("All done");
    // Endless loop
    for(;;);
}
