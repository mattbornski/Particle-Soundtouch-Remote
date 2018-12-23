#!/bin/bash

install_particle() {
    npm install -g particle-cli
}

particle --version > /dev/null || install_particle
