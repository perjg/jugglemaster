#!/bin/bash

# Build the project
echo "Building project..."
npm run build

# Check if build was successful
if [ $? -ne 0 ]; then
    echo "Build failed. Exiting."
    exit 1
fi

# Change to dist directory and start server
echo "Starting server on http://localhost:5173"
cd dist
python3 -m http.server 5173
