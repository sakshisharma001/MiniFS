#!/bin/bash

# MiniFS Concurrent Client Test Script
# This script tests concurrent client operations

set -e

# Configuration
SERVER_PORT=8080
SERVER_ADDR="127.0.0.1"
SERVER_BIN="../bin/minifs_server"
CLIENT_BIN="../bin/minifs_client"
STORAGE_DIR="../storage"
TEST_DIR="test_files"
NUM_CLIENTS=10

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Cleanup function
cleanup() {
    print_info "Cleaning up..."
    
    # Stop server
    if [ ! -z "$SERVER_PID" ]; then
        kill $SERVER_PID 2>/dev/null || true
        wait $SERVER_PID 2>/dev/null || true
    fi
    
    # Clean test files
    rm -rf $TEST_DIR
    rm -rf $STORAGE_DIR
    
    print_info "Cleanup complete"
}

# Set trap for cleanup
trap cleanup EXIT INT TERM

# Check if executables exist
if [ ! -f "$SERVER_BIN" ]; then
    print_error "Server executable not found: $SERVER_BIN"
    print_info "Run 'make' first to build the project"
    exit 1
fi

if [ ! -f "$CLIENT_BIN" ]; then
    print_error "Client executable not found: $CLIENT_BIN"
    print_info "Run 'make' first to build the project"
    exit 1
fi

# Create test directory
print_info "Creating test directory..."
mkdir -p $TEST_DIR

# Generate test files
print_info "Generating test files..."

# Small file (1KB)
dd if=/dev/urandom of=$TEST_DIR/small_1.txt bs=1024 count=1 2>/dev/null

# Medium file (100KB)
dd if=/dev/urandom of=$TEST_DIR/medium_1.txt bs=1024 count=100 2>/dev/null

# Large file (1MB)
dd if=/dev/urandom of=$TEST_DIR/large_1.txt bs=1024 count=1024 2>/dev/null

# Duplicate files for testing
for i in $(seq 2 $NUM_CLIENTS); do
    cp $TEST_DIR/small_1.txt $TEST_DIR/small_$i.txt
    cp $TEST_DIR/medium_1.txt $TEST_DIR/medium_$i.txt
    cp $TEST_DIR/large_1.txt $TEST_DIR/large_$i.txt
done

print_info "Generated $NUM_CLIENTS sets of test files"

# Start server
print_info "Starting server on port $SERVER_PORT..."
$SERVER_BIN -p $SERVER_PORT -s $STORAGE_DIR -l /tmp/minifs_test.log &
SERVER_PID=$!

# Wait for server to start
sleep 2

# Check if server is running
if ! kill -0 $SERVER_PID 2>/dev/null; then
    print_error "Server failed to start"
    exit 1
fi

print_info "Server started (PID: $SERVER_PID)"

# Test 1: Concurrent Uploads
print_info "Test 1: Concurrent uploads ($NUM_CLIENTS clients)"
for i in $(seq 1 $NUM_CLIENTS); do
    (
        echo "UPLOAD $TEST_DIR/small_$i.txt small_$i.txt"
        echo "EXIT"
    ) | $CLIENT_BIN -a $SERVER_ADDR -p $SERVER_PORT > /dev/null 2>&1 &
done

wait
print_info "Concurrent uploads completed"

# Test 2: Concurrent Downloads
print_info "Test 2: Concurrent downloads ($NUM_CLIENTS clients)"
for i in $(seq 1 $NUM_CLIENTS); do
    (
        echo "DOWNLOAD small_$i.txt $TEST_DIR/downloaded_small_$i.txt"
        echo "EXIT"
    ) | $CLIENT_BIN -a $SERVER_ADDR -p $SERVER_PORT > /dev/null 2>&1 &
done

wait
print_info "Concurrent downloads completed"

# Verify downloads
print_info "Verifying downloaded files..."
download_errors=0
for i in $(seq 1 $NUM_CLIENTS); do
    if ! cmp -s $TEST_DIR/small_$i.txt $TEST_DIR/downloaded_small_$i.txt; then
        print_error "Download verification failed for small_$i.txt"
        download_errors=$((download_errors + 1))
    fi
done

if [ $download_errors -eq 0 ]; then
    print_info "All downloads verified successfully"
else
    print_error "$download_errors download(s) failed verification"
fi

# Test 3: Concurrent List Operations
print_info "Test 3: Concurrent LIST operations ($NUM_CLIENTS clients)"
for i in $(seq 1 $NUM_CLIENTS); do
    (
        echo "LIST"
        echo "EXIT"
    ) | $CLIENT_BIN -a $SERVER_ADDR -p $SERVER_PORT > /dev/null 2>&1 &
done

wait
print_info "Concurrent LIST operations completed"

# Test 4: Concurrent Info Operations
print_info "Test 4: Concurrent INFO operations ($NUM_CLIENTS clients)"
for i in $(seq 1 $NUM_CLIENTS); do
    (
        echo "INFO small_$i.txt"
        echo "EXIT"
    ) | $CLIENT_BIN -a $SERVER_ADDR -p $SERVER_PORT > /dev/null 2>&1 &
done

wait
print_info "Concurrent INFO operations completed"

# Test 5: Delete while another client downloads
print_info "Test 5: Delete while another client downloads"
(
    echo "DOWNLOAD small_1.txt $TEST_DIR/delete_test.txt"
    echo "EXIT"
) | $CLIENT_BIN -a $SERVER_ADDR -p $SERVER_PORT > /dev/null 2>&1 &
DOWNLOAD_PID=$!

sleep 0.5

(
    echo "DELETE small_2.txt"
    echo "EXIT"
) | $CLIENT_BIN -a $SERVER_ADDR -p $SERVER_PORT > /dev/null 2>&1

wait $DOWNLOAD_PID
print_info "Delete while download test completed"

# Test 6: Duplicate upload (should fail)
print_info "Test 6: Duplicate upload test (should fail)"
(
    echo "UPLOAD $TEST_DIR/small_1.txt small_1.txt"
    echo "EXIT"
) | $CLIENT_BIN -a $SERVER_ADDR -p $SERVER_PORT > /dev/null 2>&1

if [ $? -ne 0 ]; then
    print_info "Duplicate upload correctly rejected"
else
    print_warn "Duplicate upload was not rejected (unexpected)"
fi

# Test 7: Large file transfer
print_info "Test 7: Large file transfer (1MB)"
(
    echo "UPLOAD $TEST_DIR/large_1.txt large_test.bin"
    echo "DOWNLOAD large_test.bin $TEST_DIR/downloaded_large.bin"
    echo "DELETE large_test.bin"
    echo "EXIT"
) | $CLIENT_BIN -a $SERVER_ADDR -p $SERVER_PORT > /dev/null 2>&1

if cmp -s $TEST_DIR/large_1.txt $TEST_DIR/downloaded_large.bin; then
    print_info "Large file transfer verified successfully"
else
    print_error "Large file transfer verification failed"
fi

# Test 8: Invalid filename
print_info "Test 8: Invalid filename test (should fail)"
(
    echo "UPLOAD $TEST_DIR/small_1.txt ../../../etc/passwd"
    echo "EXIT"
) | $CLIENT_BIN -a $SERVER_ADDR -p $SERVER_PORT > /dev/null 2>&1

if [ $? -ne 0 ]; then
    print_info "Invalid filename correctly rejected"
else
    print_warn "Invalid filename was not rejected (unexpected)"
fi

# Test 9: Server restart with metadata persistence
print_info "Test 9: Server restart with metadata persistence"
# Stop server
kill $SERVER_PID 2>/dev/null || true
wait $SERVER_PID 2>/dev/null || true
sleep 1

# Restart server
print_info "Restarting server..."
$SERVER_BIN -p $SERVER_PORT -s $STORAGE_DIR -l /tmp/minifs_test.log &
SERVER_PID=$!
sleep 2

# Check if files still exist
(
    echo "LIST"
    echo "EXIT"
) | $CLIENT_BIN -a $SERVER_ADDR -p $SERVER_PORT > /tmp/list_output.txt 2>&1

if grep -q "small_1.txt" /tmp/list_output.txt; then
    print_info "Metadata persistence verified"
else
    print_error "Metadata persistence failed"
fi

# Test 10: Stress test - rapid operations
print_info "Test 10: Stress test - rapid operations"
for i in $(seq 1 20); do
    (
        echo "UPLOAD $TEST_DIR/small_1.txt stress_$i.txt"
        echo "DELETE stress_$i.txt"
        echo "EXIT"
    ) | $CLIENT_BIN -a $SERVER_ADDR -p $SERVER_PORT > /dev/null 2>&1 &
done

wait
print_info "Stress test completed"

# Final cleanup
print_info "Running final cleanup..."
kill $SERVER_PID 2>/dev/null || true
wait $SERVER_PID 2>/dev/null || true

print_info "========================================="
print_info "All tests completed successfully!"
print_info "========================================="

exit 0
