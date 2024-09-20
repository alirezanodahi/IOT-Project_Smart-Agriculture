# Smart Agriculture IoT System

## Overview
This project is an IoT-based smart agriculture system that monitors environmental parameters such as gas levels, soil moisture, temperature, and humidity. The system automates irrigation, heating, and cooling to optimize crop growth.

## Project Architecture

### Software:
- **Backend**: Python (Django) for UI and backend.
- **Communication**: Mosquitto for deploying the MQTT protocol.

### Hardware:
- **Arduino Mega**: Main microcontroller.
- **NodeMCU**: Wireless communication module.
- **MQ-4**: Methane gas sensor.
- **YL-69**: Soil moisture sensor.
- **DHT-11**: Temperature and humidity sensor.
- **LCD Keypad Shield**: Local control and display.
- **Relays**: Controls external devices (heaters, fans, etc.).
- **LEDs**: Visual indicators for system status.
- **Other Components**: Breadboards, power supply (12V, 7.5A adapter), fan, heater.

## System Design
Below is the Fritzing diagram showing the circuit design for the system:

![Fritzing Circuit Diagram](<add-image-link-here>)

## Setup Instructions

### Software Setup:

- Ensure you have Python and Django installed.
- Set up a virtual environment for the project.
- Install the required dependencies for the Django application.
- Start the Django server to run the application.

### MQTT Setup:

- Install Mosquitto on your system.
- Configure Mosquitto for your specific environment.
- Set up MQTT clients to publish and subscribe to the necessary topics.

### Hardware Setup:

- Flash the Arduino Mega and NodeMCU with the provided C++ code.
- Connect the sensors, relays, and actuators as per the circuit diagram.
- Power the system using the 12V adapter and test each component.

## Code Explanation

- **Arduino Code**: Handles sensor readings and controls actuators based on the received MQTT messages.
- **Django Backend**: Monitors sensor data and sends control signals via MQTT to the hardware.
- **MQTT Communication**: The microcontrollers communicate with the Django backend via MQTT.

## Usage

### Web UI:
- The Django admin panel allows you to monitor sensor data and control devices (e.g., irrigation system, fans).

### LCD Keypad Shield:
- Use the shield for manual control and real-time monitoring on the local display.

## Future Enhancements
- Integration with additional sensors (e.g., pH, EC sensors).
- Improved user interface for better visualization of sensor data.
- More advanced control algorithms for irrigation and climate control.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contributing
Contributions are welcome! Please open an issue or submit a pull request for any improvements.
