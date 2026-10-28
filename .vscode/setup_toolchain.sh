#!/usr/bin/env bash

set -euo pipefail

# This setup scripts installs and configures:
# - AVR Toolchain packages
# - VS Code
# - VS Code extension for C/C++ file
# - VS Code extension for serial communication with CrazyCar
# - VS Code extension for Makefile support
# - Copies all extensions installed for the admin user (that executes this script)
#   to user 'LAPTOP_USERNAME' home directory
# - Adds the user 'schueler01' to the 'dialout' group to access the serial port
# - Copies udev rules for most types of Arduino

echo "====== Setting up AVR toolchain for CrazyCar ... ======"


# Install all necessary packages for AVR development + flashing of AVR
sudo apt update
sudo apt install -y make curl gcc-avr binutils-avr avr-libc gdb-avr avrdude


# =============== Download most current version of VS Code ===============
# curl's -L option follows redirects, -o option specifies the output file name
VSCODE_INSTALLER_FILENAME="vscode.deb"

curl -Lo "${VSCODE_INSTALLER_FILENAME}" https://go.microsoft.com/fwlink/?LinkID=760868

# Install VS Code without it asking for the key to be added, i.e. silent mode
echo "code code/add-microsoft-repo boolean true" | sudo debconf-set-selections
sudo apt install ./"${VSCODE_INSTALLER_FILENAME}"

# Remove VS code installer
rm "${VSCODE_INSTALLER_FILENAME}"

# =============== Install VS Code extensions ===============
# C/C++ extension for full syntax support etc.
code --install-extension ms-vscode.cpptools
# Install VS Code extension to communicate with the CrazyCar over the serial port
code --install-extension ms-vscode.vscode-serial-monitor
# Install VS Code extension for Makefile support
code --install-extension ms-vscode.makefile-tools


# =============== Add user to dialout group and add .vscode directory ===============
# On Ubuntu, the user needs to be added to the 'dialout' group to access the serial port.
# We use the username 'schueler01' for the user account with normal user permissions on
# all CrazyCar laptops.
LAPTOP_USERNAME="schueler01"
sudo adduser $LAPTOP_USERNAME dialout
# Also add the current user, i.e. admin, to dialout group, so that the admin can also access the serial port.
sudo adduser $USER dialout
# As we have installed the extensions for the admin account, we need to copy /home/$USER/.vscode to the
# /home/$LAPTOP_USERNAME/ location for all extensions to be available to the student user account
sudo cp -fR "/home/${USER}/.vscode" "/home/${LAPTOP_USERNAME}"


# ============== Copy udev rules ===============
sudo cp "99-avr-serial.rules" "/etc/udev/rules.d/"
sudo udevadm control --reload-rules
sudo udevadm trigger


echo "====== Setup finished ======"