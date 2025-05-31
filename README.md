# Arduino Barcode Verification System

A robust Arduino-based barcode verification system that uses USB HID keyboard input to scan and verify barcodes with relay control and error handling.

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Circuit Diagram](#circuit-diagram)
- [Installation](#installation)
- [Usage](#usage)
- [Code Structure](#code-structure)
- [Configuration](#configuration)
- [Character Parser Implementation](#character-parser-implementation)
- [API Reference](#api-reference)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

## 🔍 Overview

This Arduino project implements a two-stage barcode verification system:
1. **Initial Scan**: Captures the reference barcode
2. **Verification Scan**: Compares subsequent scans against the reference

The system provides visual feedback through LEDs and controls external devices via relay output.

## ✨ Features

- **USB HID Keyboard Support**: Works with standard USB barcode scanners
- **Two-Stage Verification**: Initial scan followed by verification
- **Timeout Protection**: Configurable scan timeout (default: 10 seconds)
- **Error Handling**: Visual error indication with LED blinking
- **Relay Control**: External device control based on verification results
- **Serial Monitoring**: Real-time feedback via Serial Monitor
- **Auto-Reset**: System automatically resets after error sequences

## 🔧 Hardware Requirements

### Components
- Arduino Uno/Nano/Pro Mini (or compatible)
- USB Host Shield (for USB barcode scanner connectivity)
- USB Barcode Scanner (HID keyboard compatible)
- Relay Module (5V)
- LED (for error indication)
- Resistor (220Ω for LED)
- Jumper wires
- Breadboard (optional)

### Pin Configuration
| Component | Arduino Pin |
|-----------|-------------|
| Relay Control | Pin 5 |
| Error LED | Pin 6 |
| USB Host Shield | Standard SPI pins |

## 🔌 Circuit Diagram

```
Arduino Uno    USB Host Shield    Barcode Scanner
    |               |                    |
    +-- SPI --------+                    |
    |                                    |
    +-- Pin 5 ---- Relay Module         |
    |                                    |
    +-- Pin 6 ---- LED + 220Ω Resistor  |
                                         |
USB Host Shield ---- USB Cable ---------+
```

## 📦 Installation

### Prerequisites
1. Arduino IDE (version 1.8.0 or higher)
2. USB Host Shield Library 2.0

### Library Installation
```bash
# Install via Arduino Library Manager:
# 1. Open Arduino IDE
# 2. Go to Sketch > Include Library > Manage Libraries
# 3. Search for "USB Host Shield Library 2.0"
# 4. Install by Oleg Mazurov
```

### Code Upload
1. Clone this repository:
   ```bash
   git clone https://github.com/Sp-177/Adruino.git
   ```
2. Open `barcode_verification.ino` in Arduino IDE
3. Select your Arduino board and port
4. Upload the code

## 🚀 Usage

### Basic Operation
1. **Power On**: System initializes and displays "Barcode Verification System Ready"
2. **First Scan**: System prompts "SCAN FIRST BARCODE" - scan your reference barcode
3. **Verification**: System prompts "VERIFY BARCODE" - scan the barcode to verify
4. **Results**:
   - **Match**: "Verification Successful!" - system ready for next verification
   - **Mismatch**: Error sequence with LED blinking and relay activation

### Serial Monitor Output
```
Barcode Verification System Ready
Program has initiated!
SCAN FIRST BARCODE
123456789  // Your scanned barcode
VERIFY BARCODE
123456789  // Verification scan
Verification Successful!
Ready for next verification.
```

## 🏗️ Code Structure

### Main Components

```cpp
// Configuration constants
#define TIMEOUT_MS 10000
#define ERROR_REPEAT_COUNT 5
#define RELAY 5
#define ERROR_LED 6

// Global state variables
String initial_scan = "";
String check_input = "";
String currentInput = "";
bool isErrorState = false;

// USB and keyboard handling
USB Usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> HidKeyboard(&Usb);
```

### Key Functions
- `setup()`: Initializes USB, pins, and serial communication
- `loop()`: Main program logic for scanning and verification
- `waitForBarcodeScan()`: Handles barcode input with timeout
- `triggerError()`: Manages error state and visual feedback
- `clearAllData()`: Resets system state

## ⚙️ Configuration

### Adjustable Parameters

```cpp
#define TIMEOUT_MS 10000           // Scan timeout (milliseconds)
#define ERROR_REPEAT_COUNT 5       // Number of error LED blinks
#define RELAY 5                    // Relay control pin
#define ERROR_LED 6                // Error LED pin
```

### Customization Options
- **Timeout Duration**: Modify `TIMEOUT_MS` for different scan timeouts
- **Error Indication**: Adjust `ERROR_REPEAT_COUNT` for error LED blinks
- **Pin Assignment**: Change pin definitions for different Arduino layouts

## 🔤 Character Parser Implementation

The system uses a custom keyboard report parser to handle USB HID input from barcode scanners:

```cpp
class KeyboardHandler : public KeyboardReportParser {
public:
    void OnKeyDown(uint8_t mod, uint8_t key) override;
};

void KeyboardHandler::OnKeyDown(uint8_t mod, uint8_t key) {
    uint8_t c = OemToAscii(mod, key);  // Convert scan code to ASCII
    if (c) {
        currentInput += (char)c;        // Append to current input string
        Serial.print((char)c);          // Echo character to serial
    }
}
```

**Parser Features:**
- **OEM to ASCII Conversion**: Translates keyboard scan codes to readable characters
- **Real-time Echo**: Displays scanned characters on Serial Monitor
- **String Building**: Accumulates characters until newline/carriage return
- **Modifier Key Support**: Handles shift, ctrl, alt combinations

**Input Termination:**
The parser recognizes barcode completion when it receives:
- Newline character (`\n`)
- Carriage return character (`\r`)

## 📚 API Reference

### Core Functions

#### `String waitForBarcodeScan(String prompt, unsigned long timeoutMs)`
Waits for barcode input with timeout protection.
- **Parameters:**
  - `prompt`: Message displayed to user
  - `timeoutMs`: Maximum wait time in milliseconds
- **Returns:** Scanned barcode string (empty if timeout)

#### `void triggerError()`
Activates error state with visual/audio feedback.
- **Actions:**
  - Activates relay
  - Blinks error LED
  - Prints error messages
  - Calls system halt

#### `void clearAllData()`
Resets all system variables to initial state.
- **Resets:**
  - `initial_scan`
  - `check_input`
  - `currentInput`
  - `isErrorState`

## 🔧 Troubleshooting

### Common Issues

#### USB Scanner Not Detected
```
Problem: "USB initialization failed"
Solution: 
- Check USB Host Shield connections
- Verify power supply (5V, adequate current)
- Try different USB cable/scanner
```

#### No Barcode Input
```
Problem: Scanner works but no input received
Solution:
- Ensure scanner is in HID keyboard mode
- Check scanner configuration (some require setup)
- Verify USB Host Shield library installation
```

#### False Error Triggers
```
Problem: Identical barcodes showing as different
Solution:
- Check for extra characters (spaces, newlines)
- Verify scanner output format
- Add debug prints to compare raw strings
```

### Debug Mode
Add debug output to troubleshoot scanning issues:

```cpp
// Add to waitForBarcodeScan() function
Serial.print("Raw input length: ");
Serial.println(currentInput.length());
Serial.print("Raw input: '");
Serial.print(currentInput);
Serial.println("'");
```

## 🤝 Contributing

### Development Setup
1. Fork the repository
2. Create a feature branch: `git checkout -b feature-name`
3. Make changes and test thoroughly
4. Commit: `git commit -am 'Add feature'`
5. Push: `git push origin feature-name`
6. Create Pull Request

### Code Style
- Use descriptive variable names
- Comment complex logic
- Follow Arduino coding conventions
- Test with different barcode types

### Reporting Issues
When reporting bugs, include:
- Arduino board type and version
- USB Host Shield model
- Barcode scanner specifications
- Serial Monitor output
- Steps to reproduce

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 📞 Support

- **Repository**: [https://github.com/Sp-177/Adruino](https://github.com/Sp-177/Adruino)

## 🙏 Acknowledgments

- USB Host Shield Library 2.0 by Oleg Mazurov
- Arduino Community for extensive documentation
