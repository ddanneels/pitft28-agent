# pitft28-agent
An experiment with Adafruit PiTFT 2,8" capacitive screen

1.  ## Useful packages

    ````bash
    sudo apt install libevdev-tools fbi fbset evtest libts-bin
    ````

1.  ## Register the device in your Linux system

    In the `/boot/config.txt` file, you must add the following lines
    ````
    dtoverlay=pitft28-capacitive,rotation=90,speed=32000000,fps=25
    dtoverlay=pitft28-capacitive,touch-swapxy=true,touch-invx=true
    ````
    
    And ensure that the following lines are uncommented (not starting with #) :
    ````
    dtparam=i2c_arm=on
    dtparam=i2s=on
    dtparam=spi=on
    ````

    In the `/boot/cmdline.txt` file, you must add `fbconf=map:1` to prevent Linux console to show up on PiTFT screen.

1.  ## Register the buttons as a input device

    Here is the file [pitft28-keys.dts](pitft28-keys.dts). It is a device-tree overlay file that should be compiled and put into the `/boot/overlays/` directory of your RaspberryPi (Raspbian).

    1)  ### Compile
        To compile and put the result file in the right directory, use the following command :
        ````
        sudo dtc -@ -I dts -O dtb -o /boot/overlays/pitft28-keys.dtbo  pitft28-keys.dts
        ````

        > **Note:** You may need to change the .dts file before compilation to adjust the number of buttons, the GPIO pins associated with each of them or the GPIO chip model.

    1)  ### Register
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

    1)  ### Check
        After rebooting your board, you can use `evtest` command to check that a new input device is available and that functions are correctly mapped.

1.  ## Permissions

    From the command line, run the `groups` command to ensure that you are member of both `video` and `input` groups. If not, run the following command : 
    ````bash
    sudo usermod $(whoami) -a -G video,input
    ````

1.  ## Compilation prerequesites

    ````bash
    sudo apt install build-essential gcc git libts-dev libevdev-dev 
    ````

1.  ## Launch the agent

    The agent will expect two environment variables to be set : 
    * `PITFT_DISPLAY` to something like "/dev/fb0"
    * `PITFT_TOUCHPANEL` to something like "/dev/input/event2"