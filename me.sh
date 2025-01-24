#!/bin/bash

# Define the server address and port
SERVER="127.0.0.1"
PORT=80

# Define the number of requests to send
REQUEST_COUNT=100000

# Define the request to send
REQUEST="GET / HTTP/1.1\r\nHost: $SERVER\r\nConnection: close\r\n\r\n"

# Function to send a single request
send_request() {
    echo -e $REQUEST | nc $SERVER $PORT
}

# Loop to send multiple requests in parallel
for ((i=1; i<=REQUEST_COUNT; i++))
do
    send_request &
    echo "Sent request $i"
done

# Wait for all background processes to finish
wait

echo "Finished sending $REQUEST_COUNT requests."