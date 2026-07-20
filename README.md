# NXP Application Code Hub
[<img src="https://mcuxpresso.nxp.com/static/icon/nxp-logo-color.svg" width="100"/>](https://www.nxp.com)

## Periodic Wakeup via Real-Time Clock with Console Logging 
This demo shows how to use the RTC API timer as a periodic wakeup source from standby mode on the FRDM-A-S32K312 development board. The MCU enters standby, wakes up every ~5 seconds via the WKPU triggered by the RTC, prints a wakeup message over LPUART, and goes back to sleep.

[<p><img src="images/FRDM-A-S32K312-RTC-Wakeup.png" width="500"/></p>](./images/FRDM-A-S32K312-RTC-Wakeup.png)

#### Boards: FRDM-A-S32K312
#### Categories: Low Power
#### Peripherals: UART, TIMER, CLOCKS, GPIO
#### Toolchains: S32 Design Studio IDE

## Table of Contents
1. [Software and Tools](#step1)
2. [Hardware](#step2)
3. [Setup](#step3)
4. [Results](#step4)
5. [Support](#step5)
6. [Release Notes](#step6)

## 1. Software and Tools<a name="step1"></a>
This example was developed using the FRDM Automotive Bundle for S32K3. To download and install the complete software and tools ecosystem, use the following link:<br>
- [S32K3 FRDM Automotive Board Installation Package](https://www.nxp.com/app-autopackagemgr/automotive-software-package-manager:AUTO-SW-PACKAGE-MANAGER?currentTab=0&selectedDevices=S32K3&applicationVersionID=156)

## 2. Hardware<a name="step2"></a>
### 2.1 Required Hardware
- Personal Computer
- Type-C USB cable
- [FRDM-A-S32K312](https://www.nxp.com/design/design-center/development-boards-and-designs/FRDM-A-S32K312)
 [<p><img src="https://www.nxp.com/assets/images/en/dev-board-image/FRDM-A-S32K312-TOP.jpg" width="400"/></p>](https://www.nxp.com/assets/images/en/dev-board-image/FRDM-A-S32K312-TOP.jpg)

### 2.2 Hardware Connections
- Connect 12V DC power supply to the board via the 12V power connector
- Connect the Type-C USB cable to PC and FRDM-A-S32K312 board for debugging
- Connect a USB-to-UART adapter to the LPUART6 pins to observe terminal output

### 2.3 Debugger Connection
- Connect 12V DC power supply to the board via the 12V power connector
- Connect the Type-C USB cable to PC and FRDM-A-S32K312 board for debugging

## 3. Setup<a name="step3"></a>

### 3.1 Import the Project into S32 Design Studio IDE
1. Open S32 Design Studio IDE, in the Dashboard Panel, choose **Import project from Application Code Hub**.
[<p align="center"><img src="images/import_project_1.png" width="400"/></p>](./images/import_project_1.png)

2. You can find the demo you need by searching for the name directly. Open the project, click the **GitHub link** from this window, S32 Design Studio IDE will automatically retrieve project attributes then click **Next>**.
[<p align="center"><img src="images/import_project_3.png" width="600"/></p>](./images/import_project_2.png)

3. Select **main** branch and then click **Next>**.
4. Select your local path for the repo in **Destination->Directory** window. The S32 Design Studio IDE will clone the repo into this path, click **Next>**.

5. Select **Import existing Eclipse projects** then click **Next>**.

6. Select the project in this repo (only one project in this repo) then click **Finish**.

### 3.2 Generating, Building and Running the Example
1. In Project Explorer, right-click the project and select **Update Code and Build Project**. This will generate the configuration (Pins, Clocks, Peripherals), update the source code and build the project using the active configuration (e.g. Debug_FLASH).
Make sure the build completes successfully and the *.elf file is generated without errors.
[<p align="center"><img src="images/update_and_build.png" width="200"/></p>](./images/update_and_build.png)
Press **Yes** in the **SDK Component Management** pop-up window to continue.

2. Go to **Debug** and select **Debug Configurations**. There will be a debug configuration for this project:
[<p align="center"><img src="images/Debug_config.png" width="200"/></p>](./images/Debug_config.png)

        Configuration Name                  Description
        -------------------------------     -----------------------
        $(example)_debug_flash_pemicro      Debug the FLASH configuration using PEmicro probe

    Select the desired debug configuration and click on **Debug**. Now the perspective will change to the **Debug Perspective**.
    Use the controls to control the program flow.

## 4. Results<a name="step4"></a>
Once the firmware is running, the following behavior is observed:

1. On cold boot (or after pressing reset), the terminal prints the startup banner and the MCU enters standby mode. The Green LED turns off when standby is entered.
2. After ~5 seconds, the RTC triggers the WKPU, causing a functional reset (wakeup) on S32K312.
3. On each wakeup, the terminal prints the wakeup count, stays awake briefly, then re-enters standby.

The expected terminal output is:
[<p><img src="images/FRDM-A-S32K312-RTC-Wakeup-Terminal.png" width="500"/></p>](./images/FRDM-A-S32K312-RTC-Wakeup-Terminal.png)

```
[WKPU] RTC Periodic Wakeup demo started.
[WKPU] Entering standby mode. MCU will wake up after ~5 s.
[WKPU] Hello! I woke up the 1 time. Going back to sleep now!
[WKPU] Hello! I woke up the 2 time. Going back to sleep now!
[WKPU] Hello! I woke up the 3 time. Going back to sleep now!
[WKPU] Hello! I woke up the 4 time. Going back to sleep now!
```

## 5. Support<a name="step5"></a>
For general technical questions related to NXP microcontrollers, please use the *NXP Community Forum*.

#### Project Metadata

<!----- Boards ----->
[![Board badge](https://img.shields.io/badge/Board-FRDM&ndash;A&ndash;S32K312-blue)]()

<!----- Categories ----->
[![Category badge](https://img.shields.io/badge/Category-LOW%20POWER-yellowgreen)](https://mcuxpresso.nxp.com/appcodehub?category=low_power)

<!----- Peripherals ----->
[![Peripheral badge](https://img.shields.io/badge/Peripheral-UART-yellow)](https://mcuxpresso.nxp.com/appcodehub?peripheral=uart)
[![Peripheral badge](https://img.shields.io/badge/Peripheral-TIMER-yellow)](https://mcuxpresso.nxp.com/appcodehub?peripheral=timer)
[![Peripheral badge](https://img.shields.io/badge/Peripheral-CLOCKS-yellow)](https://mcuxpresso.nxp.com/appcodehub?peripheral=clocks)
[![Peripheral badge](https://img.shields.io/badge/Peripheral-GPIO-yellow)](https://mcuxpresso.nxp.com/appcodehub?peripheral=gpio)

<!----- Toolchains ----->
[![Toolchain badge](https://img.shields.io/badge/Toolchain-S32%20DESIGN%20STUDIO%20IDE-orange)](https://mcuxpresso.nxp.com/appcodehub?toolchain=s32_design_studio_ide)

Questions regarding the content/correctness of this example can be entered as Issues within this GitHub repository.

>**Note**: For more general technical questions regarding NXP Microcontrollers and the difference in expected functionality, enter your questions on the [NXP Community Forum](https://community.nxp.com/)

[![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/NXP_Semiconductors)
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/nxp-semiconductors)
[![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/nxpsemi/)
[![Follow us on Twitter](https://img.shields.io/badge/X-Follow%20us%20on%20X-black.svg)](https://x.com/NXP)

## 6. Release Notes<a name="step6"></a>
| Version | Description / Update                           | Date                        |
|:-------:|------------------------------------------------|----------------------------:|
| 1.0     | Initial release on Application Code Hub        | July 20<sup>th</sup> 2026   |