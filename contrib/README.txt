# How to Flash

Place U-Boot NAND image as `msm460-uboot.bin` into a TFTP server directory.
Place OpenWrt factory image as `msm460-factory.bin` into a TFTP server directory.

You can start a TFTP server in the current directory using dnsmasq:

```bash
sudo dnsmasq --no-daemon --listen-address=0.0.0.0 --port=0 --enable-tftp=enxd0c0bf2ee448 --tftp-root="$(pwd)" --user=root --group=root
```

Replace `enxd0c0bf2ee448` with the name of your network interface. 

1. Assign yourself the IP-Address 192.168.1.66
2. Connect the Router to the PC while keeping the reset button pressed.
3. The LEDs will eventually begin to flash. THey will start to flash faster after around 15 seconds.
4. Release the reset button.
5. Make sure you are currently in the directory where this README is located.
6. Run the following commands:

```bash
tftp 192.168.1.1 -m binary -c put commands-synced.bin nflashd.cccc9999
```
