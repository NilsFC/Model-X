# Model-X
**BLE Model M Universal Controller**

![Model X](./site/Model%20X%20CTRL%20(Int.Batt.SW)_16b.png)

Model X is direct replacement controller for the old and good IBM Model M keyboards.<br>
Old mechanic keyboards are coming back among enthusiasts and programmers. And they're gaining value.<br>
Unfortunatly modern computers do not have DIN or PS2 ports anymore, so there's no simple way to connect a 40 years old hardware to our brand new Laptop.<br>
By simply replacing the entire legacy electronics inside the keyboard, it is possible to revive a "piece of history" and even add modern function like USB connection, Wireless and remappable caracters. Old PCB can be saved for reverting to orginal condition.<br>
The Model X CTRL is designed to fit and be used with standard 102/103 keys, 84/85 keys IBM SSK (Space Saving Keyboard) and, with an adapter, 122 key IBM M122 (AKA Battlecruiser).<br>
<br>
Main features:
  - Easy Plug and Play upgrade- no need to solder anything to original PCB or Cable;
  - BLE (Bluetooth Low Energy) and USB type C Connection;
  - Battery recharge via USB-C (same cable as most of the smart phones, notebooks and laptops);
  - Low consumption: a 2000 mAh battery last for over one month without recharging or turning off;
  - Works with IBM Model M (139140X) and IBM Model M122 Type2/3 (AKA Battleship);
  - Works with all model M FFC variants and LED connectors;
  - ZMK firmware (open source);
  - Based on ProMicro NRF52840 dev board (aka SuperMini) - technically the Model X is a "shield";
  - PCB designed in KiCad (open source);
  - Expandable: user can connect his own LEDs and solder on board related resistors;
  - Expandable: it is possible to easily connect an External Battery Switch (battery cut-off) and External Soft Off Button (J12/J13);
  - Expandable: 2 User IOs are exposed togheter with supply to add functions to the keyboard (for example beam spring coil);
  - Expandable: columns and rows matrix are exposed to 2x IDC connectors and can be routed via Model X TERM or Model X EXT122 accessories;
<br>
<br>
As a completion of the Model X, there are some accessories:
  - Model X EXT122<br>  
  <img src="./site/Model%20X%20EXT122%20(J7%202x07)_16b.png" alt="Model X EXT122" width="600">
  This accessory is designed to connect the "Model X CTRL" to Model M 122 Flat FFCs, since the original IBM controller is placed in a different position than the standard Model M.
  <br>

  - Model X LED<br>  
  <img src="./site/Model%20X%20LED_16b.png" alt="Model X LED" width="200">
  This accessory replaces the original LED unit from IBM. On this board are soldered 3x 5mm High-Intensity LEDs that draw less current, with same brigthness than the original ones, making the battery last even longer.
  LEDs colour can be chosen and to fit all different Model M variants, it can be equipped with FFC or JST connectors, with or without Resistors.

  - Model X TERM<br>  
  <img src="./site/Model%20X%20TERM_16b.png" alt="Model X TERM" width="200">
  This was the initial design of a remote terminal board in order to fit all possible position of the controller in respect of the keyboard 2x flat flexible cables (FFC).
  Actually it's more for DIY and experimentation, since it requires the user to build some sort of "bracheting" but gives more flexibility.
<br>
<br>
<br>
The project is planned to be release as Open source hardware
<br>
<br>
<br>
Inspiration and ideas:
This project have been inspired by John Hawthorn to whom all my gratitude goes, whose Model H project taught me a lot (https://modelh.club/)

Other usefull links.
Bitten's Model M Restoration Megaguide
https://photos.google.com/share/AF1QipOkRh4Wn0OBdTQppsHGt_hizKJ_D73hiwTTOsZsvn9ZJFifN0-klXLtLuJNfa2Axg?pli=1&key=OENXSXY5NVk3alZDZ0dCcEMxR05kZHZtaFBqanN3
