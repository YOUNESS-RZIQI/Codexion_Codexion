#!/bin/bash

# Configuration
EXECUTABLE="./codexion"
ITERATIONS=1000
SLEEP_TIME=0.5
# Arguments: number_of_coders time_to_burnout time_to_compile 
# time_to_debug time_to_refactor number_of_compiles_required 
# dongle_cooldown scheduler

# should burnout.
# ARGS="199 328 100 100 100 5 10  edf"
# ARGS="200 298 100 100 100 5 10  edf"
# ARGS="199 2098 100 100 100 5 600  edf"
# ARGS="199 2098 600 100 100 5 100  edf"
# ARGS="200 1398 100 100 100 5 600  edf"

# should not bournout
# ARGS="199 340 100 100 100 5 10  edf"
# ARGS="9 340 100 100 100 5 10  edf"
# ARGS="199 2110 100 100 100 5 600  edf"
# ARGS="9 2110 100 100 100 5 600  edf"
# ARGS="200 1410 100 100 100 5 600  edf"
# ARGS="199 2110 600 100 100 5 100  edf"

# should burnout.
# ARGS="199 328 100 100 100 5 10  fifo"
# ARGS="200 298 100 100 100 5 10  fifo"
# ARGS="199 2098 100 100 100 5 600  fifo"
# ARGS="199 2098 600 100 100 5 100  fifo"
# ARGS="200 1398 100 100 100 5 600  fifo"

# should not bournout
# ARGS="199 340 100 100 100 5 10  fifo"
# ARGS="9 340 100 100 100 5 10  fifo"
# ARGS="199 2110 100 100 100 5 600  fifo"
# ARGS="9 2110 100 100 100 5 600  fifo"
# ARGS="200 1410 100 100 100 5 600  fifo"
# ARGS="199 2110 600 100 100 5 100  fifo"

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