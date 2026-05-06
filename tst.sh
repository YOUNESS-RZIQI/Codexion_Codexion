#!/bin/bash

# Configuration
EXECUTABLE="./codexion"
ITERATIONS=100
SLEEP_TIME=0.5
# Arguments: number_of_coders time_to_burnout time_to_compile 
# time_to_debug time_to_refactor number_of_compiles_required 
# dongle_cooldown scheduler
ARGS="3 330 100 100 100 3 10 edf"

echo "Starting $ITERATIONS tests for $EXECUTABLE..."

for i in $(seq 1 $ITERATIONS)
do
    echo "---------------------------------------"
    echo "Test Run #$i"
    echo "---------------------------------------"
    
    # Run the executable with the provided arguments
    $EXECUTABLE $ARGS
    
    # Check if the run was successful or if it crashed
    if [ $? -ne 0 ]; then
        echo "[!] Warning: Test #$i exited with a non-zero status."
    fi

    # Sleep unless it's the last iteration
    if [ $i -lt $ITERATIONS ]; then
        echo "Waiting $SLEEP_TIME seconds before next run..."
        sleep $SLEEP_TIME
    fi
done

echo "All $ITERATIONS tests completed."