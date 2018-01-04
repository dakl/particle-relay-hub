# particle-relay-hub

[![Build Status](https://travis-ci.org/dakl/particle-relay-hub.svg?branch=master)](https://travis-ci.org/dakl/particle-relay-hub)

Particle-controlled relay board (https://docs.particle.io/datasheets/kits-and-accessories/particle-shields/#relay-shield) hooked up to four power plugs, with http-endpoints.

The four power logs can be hooked up to arbitary stuff, such as various lamps.

Compilation and deployment (flashing the mcu) is handled by travis-ci. 

# local development

Installation (and updates) of the particle CLI tools is done using `make setup`. Compilation is done by running `make build`. You need to login to particle cloud first (`make login`).