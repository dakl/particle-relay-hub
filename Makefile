setup: 
	npm install -g particle-cli

login:
	particle login

build:
	particle compile photon ./firmware/ --saveTo photon_firmware.bin

flash:
	particle flash ${RELAY_HUB_DEVICE_ID} firmware.bin
