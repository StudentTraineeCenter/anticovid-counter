# Anticovid People Counter
## Introduction
We are creating an IoT projects that takes advantage of IoT arduino platform in 
combination with Azure Cloud Services. Our pipeline consists of taking pictures of room 
by camera built into arduino board and sending them to the cloud where we utilize machine
learning to count the number people in the frame. Owner can monitor the number of humans
in a room and if it exceeds certain threshold given by the current COVID-19 laws the owner 
can see it and act accordingly.

## Problem
When government had announced the limitation of people inside building, 
owners of these properties had to deal with this - especially in shops. 
The usual solution was to limit the count of the shopping carts and force every customer 
to take one while entering the shop.

We wanted to make this easier and more comfortable by using the modern technology - 
Azure Services and Arduino board.

## Hardware and software
As our board we use Arduino AI-Thinker ESP32-CAM with its 2 Mpx camera module OV2640. 
The reasons why we use this hardware are its small size, relatively large resolution of the camera and Wi-Fi connectivity - all on one board.  

We wrote the C++ code in JetBrains' CLion with use of the PlatformIO framework for convenience.

Azure Services served us a back-end for our project - 
Azure IoT Hub (for communication) and Azure Functions (for processing).

## Solution
The idea was to take the photos in defined interval on the Arduino board, 
crop it and send using MQTT protocol to Azure IoT Hub endpoint, 
from where the Azure Function would take the place. 
There it will be processed with <AI MODEL NAME> model to obtain 
the number of people on the photo.

## Members of our team and their roles
- **Kučera, Petr** - supervisor
- **Kuboň, Václav** - developer in Azure
- **Jakub Ferenčík** - developer on Arduino

## Progress
### Arduino (Jakub)
Arduino is an open-source hardware which is suitable for small but also complex project thanks 
to its simplicity and properties according to relatively small price. 
We successfully managed to connect the pins on the board using the pinout scheme in
[this article](https://randomnerdtutorials.com/esp32-cam-video-streaming-face-recognition-arduino-ide) 
and also the board itself to the computer - which were the prerequisites to start developing.

First, I had to get used to coding for Arduino, because this was my first time developing for Arduino, 
and even very first experience of writing code using C++ programming language. 
Few weeks after I had been already organizing the files and searching for the libraries
and ended up with these:
* [ESP32 Camera Driver](https://github.com/espressif/esp32-camera)
* [Arduino JSON](https://arduinojson.org/)
* [Azure IoT Hub](https://platformio.org/lib/show/5462/ESP32%20Azure%20IoT%20Arduino)
* [Azure IoT Protocol - MQTT](https://platformio.org/lib/show/1279/AzureIoTProtocol_MQTT)
* [Azure IoT Utility](https://platformio.org/lib/show/1277/AzureIoTUtility)
* [Azure IoT Socket - WiFi](https://platformio.org/lib/show/11479/AzureIoTSocket_WiFi)

I also wrote 'helpers' whose task is to manipulate the libraries listed above:
* **AzureHelper** - contains methods used for preparing MQTT messages and communication with Azure
* **CameraHelper** - contains methods used for taking picture (using esp32cam library) and manipulating them

And lastly one class, which extends the options of logging in Arduino.
* **Logger** - contains methods used for better logging


#### Code base
As code-base I have used examples listed on [PlatformIO library listing page](https://platformio.org/lib/show/5462/ESP32%20Azure%20IoT%20Arduino) 
which gave me some initial inspiration how to communicate with Azure and receive responses.

#### Attempt to use BLOB as a storage for our pictures
We wanted to use BLOB as a destination for uploading photos using the Azure IoT Hub library, 
but there was a problem already while compiling the code which was caused by 'upload_to_blob' class, 
and it was needed to be disabled using a build parameter.

#### Encoding messages
We have discussed what would be the most convenient way to encode photo to JSON (as a replacement for BLOB)
which could be then send via MQTT protocol.
Azure IoT Hub limitation is 256 kB per message, which gives us theoretically option to send 500x500 px 
grayscale photo, but as soon as we sent our first message was sent, we encountered a problem, 
that the Azure IoT Hub will not decode the message or received at all. The photo is firstly converted to byte array, 
which is then encoded with Base64 and included into JSON. 

#### Troubleshooting
When we get MQTT communication to work with lower photo resolution (97x97 px), another problem appeared because of the 
limited size of RAM on Arduino. However, we quickly found a solution - use PSRAM for storing some objects.

There was another complication with Azure IoT Hub, when we tried increasing the resolution. 
After a few weeks we found that there was probably a non-mentioned limit to the message 
length - namely the message cannot be longer than ~5000 characters.
We tried sending the photo split in multiple messages, this approach did not work though.


### Azure (Václav)
The Azure Cloud Service offers tools for collecting data from IoT devices and for smooth integration
with other Azure services. For the data collection part we chose the IoT Hub through which we
connect to our IoT device and re-route the data to another destination. This part describes 
the most promising version of the web app that should gather the data from the IoT Hub, 
reconstruct the original image and apply computer vision model for classifying the person 
on the image as either wearing a mask or not.

#### ESP32 to web app transmission
At first, we intended to use event-hub triggered azure function for the app part of the project. 
Data sent to IoT Hub either through HTTP or MQTT protocols can trigger the built-in event hub
conveniently built into the service. Azure function written in Python executes when 
the new data arrive. This method was however never finished because 
1. functions were not proven to be ideal because we wanted continuous real time display of 
incoming data which diminishes the advantages of Azure functions.
2. We were not able to send data to IoT Hub correctly either through MQTT or HTTP anyway.

We tried one more approach for which we included the code. There is an option to send data 
from IoT device to Azure blob storage - service intended for unstructured data like images. 
As described in previous sections this method failed on the part of the IoT device in the end. 
Next paragraph offers description of the technologies used in the app which can serve at least 
as an inspiration for similar endeavours.

#### Web app
The backend of the app uses the Python Flask framework. The script application.py handles
download from Blob Storage and calls the custom vision model.

After creating the blob storage in Azure portal we just need the connection string and blob name 
to connect. The update message route is called in regular intervals and checks if any new blobs 
arrived or not. If so the blob is downloaded and stored. Because the MQTT limits 
disallowed sending the whole image the app is designed to wait for all the parts to arrive before
completing the image. The image is converted into correct format and the model is called through
custom vision API to predict class probabilities.

The nature of the problem required asynchronous communication to be used. That's why we used
Ajax in the frontend template for the period calling of the aforementioned function. 
The top class is the displayed.

#### Computer vision model
The model was trained in the Azure Custom Vision service using this face-mask dataset from 
[Kaggle](https://www.kaggle.com/andrewmvd/face-mask-detection). Thanks to using the service the 
training of the model was easy and did not take much time. The model achieved average precision 
of 98.4% according to the performance tab.

## Status
**Unfinished.**

We were not able to complete the project due to problems with sending bigger messages to Azure,
so we decided to mark this project as unfinished and focus on some other work which gives us more sense. 