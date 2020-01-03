# SmartWaterTank - Save water with IoT 🚰

SmartWaterTank converts your ordinary tank to a smart one, and helps save some water. Flash the sketch right now on your NodeMCU chip and you are almost ready to go. This sketch not only saves water by turning off the pump for you, but this also helps you in getting a deep analysis of water level & consumption during a selected period of time. To use the sketch at its best, you should SignUp on [Blynk](https://blynk.io/) & [IFTTT](https://ifttt.com/).

[![](https://img.shields.io/badge/Hosted%20on-Blynk-brightgreen)](https://blynk.io/)

## Getting Started 🚀

Before proceeding I expect that you already have decent knowledge of programming and circuits. 

#### 1. Circuit

For making this project you're gonna need a NodeMCU and an Ultrasonic sensor. Assemble them together as shown in the image down below. Later, you can assemble these parts as per your needs in the wanter tank. Just make sure that ultrasonic sensor is attached to tank lid (facing downwards) and none of the electrical components or circuits have any contact with water.

* This image is just for demonstration. Actual pin numbers may vary in the Sketch uploaded in this repository.
![Sample Circuit Design](https://github.com/vishalroygeek/SmartWaterTank/blob/master/assets/sample-circuit-design.png)

#### 2. Blynk Project

Now download the Blynk app and setup a project. For viewing the water level add a graph widget and for controlling your water pump or water device you wanna interact with, create a webhook widget. Also copy the Blynk token of your project from the app.

<img src="https://github.com/vishalroygeek/SmartWaterTank/blob/master/assets/blynk-app-screenshot.png" width="300">

#### 3. Finish

You should now create a webhook URL on IFTTT for performing action on your connected appliance. Once ready put your URL in the webhook widget of the Blynk project we created in step 2. Now you can plug in your NodeMCU and configure it from your mobile phone. Once configured, your NodeMCU will restart and should be shown online in the Blynk app dashboard.

✋ If you wish, you can deep dive into the arduino sketch and can modify it according to your needs. There you should also be able to find some more components attaching options already available. Like - Reset button & Status LED.


# LICENSE
___
```
Copyright (c) 2019 Vishal Roy

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

