#!/bin/bash

ktm
./clean.sh
./launch.sh --angle=270

sleep 5

: <<'END'
The below parts don't work because ./launch.sh will not stop running until the
simulation is closed (poking doesnt happen until after exit)

uPokeDB DEPLOY_ALL=true
echo "localhost"
echo "9000"

uPokeDB MOOS_MANUAL_OVERRIDE_ALL=false
echo
echo

END
