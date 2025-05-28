# Turnbinds for Wayland
This is an application written in C to bring turnbinds (just like AHK) to Wayland on Linux :)

# Requirements
## Debian/Ubuntu
sudo apt update
sudo apt install build-essential linux-headers

## Arch (-based) systems
sudo pacman -S base-devel linux-headers

# Compiling
## ``gcc -o turnbinds turnbinds.c``

# Usage
``sudo ./turnbinds``
## sudo is required due to uinputs needing kernel access, it's possible to run without root, although I find it to be a bit convoluted

## Create a udev rule
### sudo vim /etc/udev/rules.d/99-uinput.rules

### Add the following:
#### KERNEL=="uinput", MODE="0660", GROUP="input"
#### KERNEL=="event*", MODE="0660", GROUP="input"

### Add your user to the ``input`` group
#### sudo usermod -aG input $USER

#### Reload udev
### sudo udevadm control --reload-rules
### sudo udevadm trigger
#### And relogin
