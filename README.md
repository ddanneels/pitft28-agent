# pitft28-agent
An experiment with Adafruit PiTFT 2,8" capacitive screen

1.  ## Register the PiTFT in your Linux system

    You just need to download and run the [adafruit-pitft.sh][1] script. Answer "no" to the last questions (no X display, no console) to get a raw framebuffer.

1. ## Register the buttons as a input device

    Here is the file [pitft28-keys.dts](pitft28-keys.dts). It is a device-tree overlay file that should be compiled and put into the `/boot/overlays/` directory of your RaspberryPi (Raspbian).

    To compile and put the result file in the right directory, use the following command :
    ````
    sudo dtc -@ -I dts -O dtb -o /boot/overlays/pitft28-keys.dtbo  pitft28-keys.dts
    ````

    > **Note:** You may need to change the .dts file before compilation to adjust the number of buttons, the GPIO pins associated with each of them or the GPIO chip model.

    In the `/boot/config.txt` file, you must add the following line 
    ````
    dtoverlay=pitft28-keys
    ````

    > **Note:** You can add parameters to associate a different function with one or several buttons.
    > The buttons are identified by their GPIO pin : 17, 22, 23, 27 from top to bottom.
    > The default functions are respectively : *Back*, *Up*, *Down*, *Enter*
    >
    > For example, to associate the first button to *Home* instead of *Back*, you should use
    > ````
    > dtoverlay=pitft28-keys,label17=Home,keycode17=102
    > ````
    >
    > To see all available functions, search for `input-event-codes.h` file.

    After rebooting your board, you can use `evtest` command to check that a new input device is available and that functions are correctly mapped.




[1]: https://github.com/adafruit/Raspberry-Pi-Installer-Scripts/blob/master/adafruit-pitft.sh
