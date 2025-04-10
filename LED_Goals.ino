#include <Adafruit_NeoPixel.h>

const int BUTTON_PIN = 2;  // Button connected to D2
const int LED_PIN = 3;     // LED connected to D3
const int NEO_PIXEL_PIN = 6; // NeoPixel strip connected to D6
const int NUM_PIXELS = 22; // Number of LEDs in the strip
const int DEBOUNCE = 100; // 100ms debounce delay
const int AUTO_INCREMENT_DELAY = 115200000; // 32 hours to auto inrcement LED
const int RAINBOW_STROBE_DELAY = 75;

Adafruit_NeoPixel strip(NUM_PIXELS, NEO_PIXEL_PIN, NEO_GRB + NEO_KHZ800);

static bool prev_button_state = HIGH;
static int pixels_on = 0;
static int hue = 0; // Hue value for gradual color change
static unsigned long prev_auto_increment = 0; // Time of last increment
static unsigned long last_hue_update = 0;
static unsigned long last_button_time = 0;
static unsigned long prev_hb_time = 0;
static unsigned long curr_time = 0;

void await_reset()
{
    static int pixel_offset = 0;
    static int window_size = 5;
    static int num_pixels_lit = 0;
    unsigned long strobe_time = millis();
    bool button_state = digitalRead(BUTTON_PIN);
    
    digitalWrite(LED_PIN, HIGH);

    // Wait for button press to reset system. Rainbow strobe while we wait.
    while (!(button_state = digitalRead(BUTTON_PIN))) {

        if (millis() - strobe_time >= RAINBOW_STROBE_DELAY) {
            strobe_time = millis();
            hue = (hue + 10) % 256;

            // Sliding window of pixels logic
            if (num_pixels_lit < window_size) {
                num_pixels_lit++;
            } else {
                pixel_offset = (pixel_offset + 1) % NUM_PIXELS;
            }
        }

        for (int i = 0; i < NUM_PIXELS; i++) {
            int actual_pixel = (i + pixel_offset) % NUM_PIXELS;  // Wrap-around logic

            // If the pixel is within the window, set its color
            if (i < num_pixels_lit) {
                uint32_t color = strip.ColorHSV(hue * 256, 255, 255);  // Hue applied here
                strip.setPixelColor(actual_pixel, color);
            }
        }
        
        strip.show();
    }

    // Restart program
    setup();
}

void startupSequence() 
{
    for (int i = 0; i < NUM_PIXELS; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 255)); // Blue
        strip.show();
        delay(50);
    }
    delay(300);
}

void setup()
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);  // Use internal pull-up
    pinMode(LED_PIN, OUTPUT);
    pixels_on = 0;
    strip.begin();
    digitalWrite(LED_PIN, LOW);
    startupSequence();
    strip.clear();
    strip.show();
}

void loop()
{
    // Base case, if the pixels are filled, we do nothing until the button is pressed
    if (pixels_on >= NUM_PIXELS) {
        await_reset();
    }
  
    // millis() is 32-bits, will overflow at 4294967295 milliseconds
    //
    // We don't need to worry about the rollover because the comparrison of the threshold
    // will also overflow due to the use of unsigned ints and the comparrison will still
    // work.
    curr_time = millis();
    
    // Handle LED pulsing (heartbeat)
    heartbeat();
    
    // Handle button press to manually increment NeoPixel count
    bool button_state = digitalRead(BUTTON_PIN);
    if (button_state == LOW && prev_button_state == HIGH) { // Detect button press
        if (curr_time - last_button_time > DEBOUNCE) {
            incrementPixelCount();
            prev_auto_increment = curr_time;
            last_button_time = curr_time;
        }
    }
    prev_button_state = button_state;
    
    // Automatically increment pixel count
    if (curr_time - prev_auto_increment >= AUTO_INCREMENT_DELAY) {
        prev_auto_increment = curr_time;
        incrementPixelCount();
    }
    
    // Gradually cycle through colors (slower strobe effect)
    if (curr_time - last_hue_update >= 2000) { // 2 second color transition
        last_hue_update = curr_time;
        hue = (hue + 1) % 256; // Cycle through color spectrum
    }

    // Update pixels with new values
    updateNeoPixels(hue);
}

static bool blink_routine = false;
static long last_blink = 0;
static int blink_count = 0;
static bool led_on = false;

// heartbeat to show device activity, without using delay()
void heartbeat()
{
    if (curr_time - prev_hb_time >= 2500) {
        blink_routine = true;
        last_blink = curr_time;
        prev_hb_time = curr_time;
    }

    if (blink_routine == true) {
        if (curr_time - last_blink >= 100) {
            led_on = !led_on;
            digitalWrite(LED_PIN, led_on ? HIGH : LOW);
            last_blink = curr_time;
            blink_count++;
        }

        if (blink_count >= 4) {
          blink_routine = false;
          blink_count = 0;
        }
    }
}

void incrementPixelCount() {
    pixels_on = (pixels_on + 1) % (NUM_PIXELS + 1);
}

void updateNeoPixels(int hue) {
    uint32_t color = strip.ColorHSV(hue * 256, 255, 255); // Convert hue to RGB
    for (int i = 0; i < pixels_on; i++) {
        strip.setPixelColor(i, color);
    }
    strip.show();
}
