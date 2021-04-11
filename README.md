
# Plantlapse

Watching plants grow


![Mint](docs/mint.gif)


# What is this?

You can pick up an ESP32 with a camera and an SD card slot for like, six bucks, it's bananas.

With an AliExpress ESP32-Cam, this is a (potentially) great general purpose timelapse camera. Not just for plants!

Here are some features:

* A web interface to control capture / timing parameters
* Able to serve up images over the web interface to check that those parameters are good, and physically place your camera
* Able to save images to SD card
* SD card images available to download over the web interface
* Nonvolatile storage of the parameters, along with the current snapshot number, in order to gracefully recover from power failure (Planned)
* Over-the-air updating (Currently broken)
* Reasonably well-laid out codebase that's easy to hack on (Debatable)

# How does it work?

Using the ESP-IDF V4.2:

Clone and then `make menuconfig` to set up your particular parameters.

Serial flasher Config: 16MB flash size

Partition Table: Custom

Webserver Configuration: Website mount point: `/sdcard`

OTA Configuation: `http://192.168.1.112:8070/plantlapse-app/build/plantlapse.bin` or similar, and check "skip server cert"

Example Connection Configuration: Your wifi AP credentials

ESP HTTPS OTA: Allow HTTP for OTA

(this might be something we can work on, in the meantime, keep it within your LAN)

ESP32-specific: Support for external, SPI-connected RAM

Long filename support: Long filename buffer in heap



Pointing your browser to http://plantcam.local will open up a settings page.

![Set](docs/set.png)

http://plantcam.local/getsettings will return a JSON payload of all the current settings.

![Get](docs/get.png)

http://plantcam.local/now.jpg will snap a picture with the current settings and serve it up.


See [here](https://jrainimo.com/build/?p=2323) for more gratuitous words about this project.
