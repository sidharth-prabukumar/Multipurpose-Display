# Building the Project

This project is built as a VSCode project. However, STM32 IDE is required in order to obtain the Drivers. After that, everything can be run from outside the IDE.

1. Open the ioc file using STM32 IDE

2. Generate the code in order to get the ```Drivers```

3. From within the ```stm32``` directory, run ```make```

4. The ```.elf``` and ```.bin``` files are created inside the ```build``` directory

5. Connect the ```STM32 F446RE``` Nucleo Board to the PC.

6. Run ```make flash``` to flash to the STM32 hardware

