#include <hidboot.h>
#include <usbhub.h>

// ---------------------- Configuration ------------------------
#define TIMEOUT_MS 10000           // Timeout for barcode scan
#define ERROR_REPEAT_COUNT 5       // Number of error LED blinks
#define RELAY 5                    // Relay control pin
#define ERROR_LED 6                // Error LED pin

// ---------------------- Global Variables ---------------------
String initial_scan = "";
String check_input = "";
String currentInput = "";
bool isErrorState = false;

// ---------------------- USB & Keyboard -----------------------
USB Usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> HidKeyboard(&Usb);

class KeyboardHandler : public KeyboardReportParser {
public:
    void OnKeyDown(uint8_t mod, uint8_t key) override;
};

KeyboardHandler DoKeyActions;

void KeyboardHandler::OnKeyDown(uint8_t mod, uint8_t key) {
    uint8_t c = OemToAscii(mod, key);
    if (c) {
        currentInput += (char)c;
        Serial.print((char)c);  // Echo the scanned char
    }
}

// ---------------------- Setup -----------------------
void setup() {
    Serial.begin(9600);

    pinMode(RELAY, OUTPUT);
    pinMode(ERROR_LED, OUTPUT);
    digitalWrite(RELAY, LOW);
    digitalWrite(ERROR_LED, LOW);

    if (Usb.Init() == -1) {
        Serial.println("USB initialization failed");
        while (true);  // Halt on USB error
    }

    HidKeyboard.SetReportParser(0, &DoKeyActions);
    Serial.println("Barcode Verification System Ready");
}

// ---------------------- Loop -----------------------
void loop() {
    Usb.Task();
    
    // First scan (initial setup)
    if (initial_scan.length() == 0) {
        
        Serial.println("Program has initiated!");
        digitalWrite(RELAY, HIGH);
        initial_scan = waitForBarcodeScan("SCAN FIRST BARCODE", TIMEOUT_MS);
        digitalWrite(RELAY, LOW);

        if (initial_scan.length() == 0) return;  // Timeout or no scan
    }

    // Check scan (verification phase)
    if (check_input.length() == 0) {
        check_input = waitForBarcodeScan("VERIFY BARCODE", TIMEOUT_MS);
        if (check_input.length() == 0) return;
    }

    initial_scan.trim();
    check_input.trim();

    // Compare scanned inputs
    if (initial_scan != check_input) {
        triggerError();     // Mismatch
    } else {
        Serial.println("\nVerification Successful!");
        clearInput();

    }
}

// ---------------------- Helper: Barcode Scan -----------------------
String waitForBarcodeScan(String prompt, unsigned long timeoutMs) {
    Serial.println(prompt);
    currentInput = "";

    unsigned long startTime = millis();
    while (millis() - startTime < timeoutMs) {
        Usb.Task();

        if (currentInput.length() > 0 && 
           (currentInput.endsWith("\n") || currentInput.endsWith("\r"))) {
            currentInput.trim();
            break;
        }

        delay(10);
    }

    if (currentInput.length() == 0)
        Serial.println("Timeout: No input received");

    return currentInput;
}

// ---------------------- Helper: Trigger Error -----------------------
void triggerError() {
    isErrorState = true;
    digitalWrite(RELAY, HIGH);

    for (int i = 0; i < ERROR_REPEAT_COUNT; i++) {
        Serial.println("ERROR: BARCODE VERIFICATION FAILED");
        digitalWrite(ERROR_LED, HIGH);
        delay(500);
        digitalWrite(ERROR_LED, LOW);
        delay(500);
    }

    stopProgram();  // Lock system
}

// ---------------------- Helper: Stop Program -----------------------
void stopProgram() {
    Serial.println("Program halted due to error state.");

    for (int i = 0; i < 10; i++) {
        digitalWrite(ERROR_LED, HIGH);
        delay(1000);
        digitalWrite(ERROR_LED, LOW);
        delay(1000);
    }
    clearAllData();
    loop();
}

// ---------------------- Helper: Clear All Cached Data -----------------------
void clearAllData() {
    initial_scan = "";
    check_input = "";
    currentInput = "";
    isErrorState = false;
    Serial.println("System reset: Ready for next verification.");
}
void clearInput() {
    check_input = "";
    currentInput = "";
    isErrorState = false;
    Serial.println("Ready for next verification.");
}