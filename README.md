# **Model-X CTRL**

### **BLE Model M Universal Controller**

Project wiki: https://github.com/NilsFC/Model-X/wiki

![Model X](<./Pics/Resources/Model%20X%20CTRL%20(Int.Batt.SW)_16b.png>)

Model X is a direct replacement controller for the classic IBM Model M keyboards.<br>
Old mechanical keyboards are making a comeback among enthusiasts and programmers and gaining value.<br>
Unfortunately, modern computers no longer have DIN or PS2 ports, so there’s no simple way to connect a 40-year-old hardware to a brand-new laptop.<br>
By replacing the legacy electronics inside the keyboard, it’s possible to revive a “piece of history” and even add modern functions like USB connectivity, wireless support, and remappable characters. The original PCB can be preserved to revert the keyboard to its original state.<br>
The Model X CTRL is designed to fit and work with standard 101/102 keys, 84/85 keys IBM SSK (Space Saving Keyboard), and, with an adapter, the 122-key IBM M122 (AKA Battlecruiser).<br>
The "X" in Model X stands for "Cross keyboard," as it aims to be compatible with most Model M variants.

---

### **Main Features**

- **Plug-and-Play Upgrade**: No soldering required to connect to the original PCB or cable.
- **BLE (Bluetooth Low Energy)** and **USB Type-C Connection**
- **USB-C Charging** (same cable as most smartphones, notebooks, and laptops)
- **Low Power Consumption**: A 2000 mAh battery lasts over a month without recharging or turning off.
- **Compatibility**: Works with IBM Model M (139140X), IBM Model M122 Type 2/3 (AKA Battleship) and 84 keys SSK.
- **Broad Support**: Compatible with all Model M FFC variants and LED connectors.
- **ZMK Firmware** (open source)
- **Software Remappable Keys** via ZMK Studio User Interface
- **Hardware Platform**: Based on the ProMicro NRF52840 dev board (aka SuperMini); Model X is a "shield".
- **Designed in KiCad** (open source)
- **Expandable**: User-connectable LEDs and solderable on-board resistors.
- **Battery Control**: Easily connect an external battery switch (battery cut-off) and an external soft-off button (J12/J13).
- **Additional I/O**: 2 user I/O lines exposed with supply for added functionality (e.g., beam spring coil).
- **Matrix Connections**: Rows and columns matrix are exposed via two 2x IDC connectors, usable with Model X TERM or Model X EXT122 accessories.

---

### **Installation**

#### **IBM Model M retrofit**

#### **IBM Model M retrofit (101/102 keys)**  
[![IBM Model M122 retrofit](https://img.youtube.com/vi/JZ2OXhT8EQA/0.jpg)](https://www.youtube.com/watch?v=JZ2OXhT8EQA)
<br>

#### **IBM Model M122 retrofit (122 keys)**  
[![IBM Model M122 retrofit](https://img.youtube.com/vi/N0QGWPXJK3Q/0.jpg)](https://www.youtube.com/watch?v=N0QGWPXJK3Q)

---

### **Accessories**

#### **Model X EXT122**

<img src="./Pics/Resources/Model%20X%20EXT122%20(J7%202x07)_16b.png" alt="Model X EXT122" width="600">

This accessory connects the Model X CTRL to Model M 122 Flat FFCs, accommodating the different controller placement in the standard Model M.

---

#### **Model X LED**

<img src="./Pics/Resources/Model%20X%20LED_16b.png" alt="Model X LED" width="200">

This accessory replaces the original IBM LED unit with 3 high-intensity 5mm LEDs that consume less power, extending battery life without compromising brightness. LEDs are customizable in color, with optional FFC or JST connectors, and can be equipped with or without resistors.

---

#### **Model X TERM**

<img src="./Pics/Resources/Model%20X%20TERM_16b.png" alt="Model X TERM" width="200">

Originally designed as a remote terminal board to fit various controller positions relative to the keyboard's FFC cables, this accessory is great for DIY enthusiasts, offering flexibility for custom projects.

---

### **Open Source**

The project is planned to be released as open-source hardware.

---

### **Inspiration and Resources**

- **John Hawthorn**  
  Check out the fantastic work of this guy to whom I am very grateful. Thanks to his Open Source project I really learned a lot. [Model H Project](https://modelh.club/)
- **ZMK Firmware**  
  Incredible work of this group. If you're a Keyboard and DIY enthusiast, surely this needs no presentation: [https://zmk.dev/](https://zmk.dev/)

#### Other useful links:

- **Bitten's Model M Restoration Megaguide**  
  [View Guide](https://photos.google.com/share/AF1QipOkRh4Wn0OBdTQppsHGt_hizKJ_D73hiwTTOsZsvn9ZJFifN0-klXLtLuJNfa2Axg?pli=1&key=OENXSXY5NVk3alZDZ0dCcEMxR05kZHZtaFBqanN3)
