# How to run Sniffex 1-3.c
1. `gcc -Wall -o sniffex sniffex.c -lpcap`
2. `sudo ./sniffex`
3. Ping/telnet whatever you are trying to sniff!

# How to run spoofer.c
1. `gcc -o spoofer spoofer.c`
2. run `sudo ./spoofer 0` for ICMP
3. run `sudo ./spoofer 1` for Ethernet

# Notes
I used MC10 not MC02.
------
Name: Steven Dellamore
PUID: 027856266
username: sdellamo
