# ESP32-Joystick-Mouse
A simple Script for a homemade joystick mouse, made using [ESP32-BLE-Mouse lib][https://github.com/T-vK/ESP32-BLE-Mouse] by T-vk

# Functionality:

Joystick mimics the cursor (Pretty straightforward)
Pressing the Joystick button: LEFT MOUSE BUTTON
Holding the Joystick button: RIGHT MOUSE BUTTON
Double pressing the Joystick button: Switches into scroll mode, pressing it twice again turns of the scroll mode

# Currently used pins:
- Joystick X axis = 36
- Joystick Y axis = 39

# How to setup:

- Get the [ESP32-BLE-Mouse lib][https://github.com/T-vK/ESP32-BLE-Mouse] by T-vk
- Check the neutral X/Y values for your joystick, overwrite the values for X_NEUTRAL and Y_NEUTRAL
- May have to change the pin value for joyX and joyY depending on the pins you use
- Upload the code to your esp32 and connect via bluetooth (Name should be something like "ESP32 Bluetooth Mouse")
- Enjoy
