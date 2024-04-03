ALL THE FOLLOWING HAS ALSO CAUSED ME ISSUES:

Installing USB Serial (CDC) refixed everything for me. Although libusb-win32 worked for a little bit.

----

Using zadig with the libusb-win32 setting fixed the upload issue on pi pico.
Might have to go to options > list all devices and find "pico"



Just follow this:


>I wasn't able to use @savejeff 's method (it would just timeout with ....... shown), I had to use Zadig to install libusb-win32 drivers for the "RP2 Boot2 (Interface 1)" device so that the upload would work. Otherwise it would also show me an unknown device in the device manager.

>Maybe for people who already used the Arduino IDE with the Pico have this driver pre-installed?

>On Windows 10 btw.

>Edit: I also experienced the same behavior for my Sparkfun RP2040 ThingPlus, using Zadig allowed the first upload to go through, and from there on I could delete upload_port since it would auto-detect the COM port correctly.
