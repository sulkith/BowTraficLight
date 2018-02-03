# BowTraficLight
This Project is a Tracffic Light for FITA Tournaments.
It consistsof two Parts the Sender and the Receiver.
The Sender is a simple device which sends a Signal via the VirtualWire Library.
The Receiver starts the Flow of the Tournament.
* 20/10s s of time to get ready.
* 120/240 s of time to shoot / 30s before the end the Light switches to Yellow.

If group Mode is active this Flow is repeated twice otherwise the Device will enter the Idle Mode and wait for the next start
A german Documentation can be found here: [Docu\Docu.pdf](https://github.com/sulkith/BowTraficLight/blob/master/Docu/Docu.pdf)

# Components
The used Components for the Sender([Pictures](https://github.com/sulkith/BowTraficLight/tree/master/Pictures/Sender)) were:
* Arduino Pro Micro Clone
* Radio Frequency Sender Module
* 7805 to reduce the Voltage for the Arduino and the RF Module
* Button to switch on the Power
* 9V Battery to power the Sender
* some Resistors and Capacitors

The used Components for the Receiver([Pictures](https://github.com/sulkith/BowTraficLight/tree/master/Pictures/Receiver)) were:
* Arduino Pro Micro Clone
* Radio Frequency Receiver Module
* WS2812 LED strip
* little buzzer to make some noise
* Power Bank to supply 5V
* some Resistors and Capacitors

