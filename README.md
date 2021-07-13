# Reading Temperature through NodeMCU

## Requirements:

- NodeMCU module
- Breadboard
- DHT11 Temperature sensor
- Arduino app

## Project preview:

In this project we attempt through read the live temperature through a temperature sensor connected to a
NodeMCU module. The requests will be made through a telegram bot which will then make a request to the server hosted
by the NodeMCU to read the temperature. The temperature sensor return the live temperature and the server will pass a message through
the API of the telegram bot which well then display the temperature on the user end.

## Instructions: 
Open ```ReadTemperature.ino``` present in the ReadTemperature folder and open in on the Arduino application.
Using a breadboard connect the DHT11 sensor to the NodeMCU. Connect the NodeMCU to your PC using USB cable.
Make a telegram bot using BotFather. Get the API token and url and update it in the code. Also update your WiFi router
information. Upload the code to the NodeMCU board and call \temp to the telegram bot to read the temperature.