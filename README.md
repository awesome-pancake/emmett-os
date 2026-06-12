# Computer Science 30 Capstone Project

## Instructions

This project was developed using WSL Ubuntu, but can be built from most linux distributions or in a virtual machine. Keep in mind that these instructions were designed with WSL in mind.

### 1. Installing gnu-efi

One core dependency of the bootloader is gnu-efi. To install gnu-efi, run the following three commands in the root folder:

```bash
git clone https://github.com/ncroxon/gnu-efi.git
cd gnu-efi
make
```

This library allows for the development of UEFI applications that are bootable. For UEFI to recognize the bootloader, it must be placed on a FAT32 formatted storage device in `/boot/efi/bootx64.efi`.

### 2. Building the Project

Once gnu-efi is installed, you can test it by entering `emmett-os/bootloader/v1` and running `make`. The bootloader should successfully build and you should see a file called `bootx64.efi`. From here, change directory to `emmett-os/kernel` and run `make` again.

From here, format a storage device as FAT32 and place `bootx64.efi` in `/boot/efi`. Place `kernel.elf` in the root folder of the storage device. From here, you should be all good to go!

### 3. Running the Project

Running the project is one of the trickier parts of testing it. Over the course of this project, I found two different ways to run it:
- Booting it on an actual x86-64 device,
- Running it in a virtual machine like virtualbox.

#### 3.1 Booting on an x86-64 Device

In order to run the project on an actual machine, you need to make sure that the device is a laptop, and that secure boot is disabled in the UEFI settings. The device needs to be a laptop since my project will only connect to legacy PS/2 keyboards, which are what modern laptops use for their keyboard connection. Disabling secure boot means that the machine will be capable of running UEFI applications without a registered signature, such as my project. You can enter UEFI settings usually by pressing F12 or DEL during the computer's boot sequence.

Once the machine has secure boot disabled, you can usually enter the boot menu of the computer either by pressing F12 or DEL depending on the machine. From here, select the volume label of the storage device you have the project saved on, and you should be good to go!

#### 3.2 Booting on Virtualbox

Running the project on a virtual machine is a little bit trickier than booting it on a real machine for various reasons. Luckily, you do not need to have virtualbox installed on WSL in order to run the project. To test my project, I installed a third party tool called rEFInd as well as virtualbox's USB extension pack. Create a new 64 bit virtual machine without an operating system image, and select the rEFInd iso as the disk image to use. Make sure in the settings you have EFI enabled and secure boot disabled! When first booting up the virtual machine, you should see the rEFInd menu. Go to `devices>usb` and select the volume label of the USB drive that the project is saved on and reset the machine. After resetting, you should see a coloured icon called "bootx64.efi" or "fallback bootloader". Hit enter on this icon and you should load into the project as if you were on a real machine.