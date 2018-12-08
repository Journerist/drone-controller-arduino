# Drone controller arduino

## Compatiblity
- Arduino MKR WIFI 1010

Other Arduinos should work as well when an wifi shield is installed.

## Description
This project contain software that fulfills 2 jobs:

1. Make an endpoint available `http://<drone-ip>/?motor0=<motor-0-speed>&motor1=<motor-1-speed>&motor2=<motor-2-speed>&motor3=<motor-3-speed>`
2. Print important sensors in a to be defined json format.

## Outlook

### Control
Based upon that, a drone can be controlled using any environment. Experiments like building a neuronal network for stabilization and navigation should be possible because the heavy load can be done on a more powerful machine.

### Swarm
On top of that, swarm control can be realized because each of this drones have the same api.

## Related repositories
https://github.com/Journerist/drone-frontend
