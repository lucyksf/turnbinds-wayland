# Turnbinds for Wayland
This is an application written in C to bring turnbinds (just like AHK) to Wayland on Linux :)

# Requirements
## Debian/Ubuntu
``sudo apt update``
###
``sudo apt install build-essential linux-headers evtest``

## Arch (-based) systems
``sudo pacman -S base-devel linux-headers evtest``

# How to find your sensitivity
## Run ``sudo evtest`` and find your mouse input, then run
##
## ``sudo libinput debug-events --device /dev/input/eventX`` X=your mouse input number
##
## Move your mouse steadily for a few seconds, you'll see a lot of ``event3   POINTER_MOTION    6446    +15.366s    0.00/  0.21 ( +0.00/ +1.00)``, the number 6446 will be different for you depending on your sensitivity, this number is what you want to save. Now you'll need to note your monitor's refresh rate. The equation goes as follows: POINTER_MOTION / REFRESH_RATE ≈ XX. This will be your number to put as your left_speed and right_speed
## Inside the code, you can adjust the usleep() function at the bottom to match with your monitor's refresh rate

## Compiling
``gcc -o turnbinds turnbinds.c``

## Usage
``sudo ./turnbinds``
### sudo is required due to uinputs needing kernel access, it's possible to run without root, although I find it to be a bit convoluted

### How to do it:

## Create a udev rule
``sudo touch /etc/udev/rules.d/99-uinput.rules``
###
``sudo vim /etc/udev/rules.d/99-uinput.rules``

### Add the following:
``KERNEL=="uinput", MODE="0660", GROUP="input"``
###
``KERNEL=="event*", MODE="0660", GROUP="input"``

## Add your user to the ``input`` group
``sudo usermod -aG input $USER``

## Reload udev
``sudo udevadm control --reload-rules``
###
``sudo udevadm trigger``
#### And relogin/restart

### You can also make an alias to run the file
###
### ``vim ~/.bashrc`` (OR ~/.zshrc)
###
### ``alias nameHere="sudo ~/location/turnbinds"``



# :)
