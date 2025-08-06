#!/bin/bash


set -e  

echo "🚁 Building DroneView Flight Operations Dashboard..."

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' 

print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

if ! command -v cmake &> /dev/null; then
    print_error "CMake is not installed. Please install CMake first."
    exit 1
fi

CMAKE_VERSION=$(cmake --version | head -n1 | cut -d" " -f3)
print_status "Found CMake version: $CMAKE_VERSION"

if [ ! -d "build" ]; then
    print_status "Creating build directory..."
    mkdir build
else
    print_status "Using existing build directory..."
fi

cd build

if [[ "$OSTYPE" == "darwin"* ]]; then
    print_status "Detected macOS - configuring for Qt6..."
    
    QT_PATHS=(
        "/opt/homebrew/lib/cmake/Qt6"
        "/usr/local/lib/cmake/Qt6"
        "/usr/local/Qt-6*/lib/cmake/Qt6"
    )
    
    QT_PATH=""
    for path in "${QT_PATHS[@]}"; do
        if [ -d "$path" ] || ls $path &> /dev/null; then
            QT_PATH="$path"
            break
        fi
    done
    
    if [ -z "$QT_PATH" ]; then
        print_warning "Qt6 not found in standard locations. Trying without CMAKE_PREFIX_PATH..."
        cmake ..
    else
        print_status "Found Qt6 at: $QT_PATH"
        cmake -DCMAKE_PREFIX_PATH="$QT_PATH" ..
    fi
    
    CORES=$(sysctl -n hw.ncpu)
    print_status "Building with $CORES cores..."
    make -j$CORES
    
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    print_status "Detected Linux - configuring for Qt6..."
    cmake ..
    
    CORES=$(nproc)
    print_status "Building with $CORES cores..."
    make -j$CORES
    
elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    print_status "Detected Windows - configuring for Qt6..."
    cmake -G "MinGW Makefiles" ..
    cmake --build . --config Release
    
else
    print_warning "Unknown platform: $OSTYPE. Trying default configuration..."
    cmake ..
    cmake --build .
fi

if [ $? -eq 0 ]; then
    print_success "Build completed successfully!"
    
    if [[ "$OSTYPE" == "darwin"* ]] || [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if [ -f "./DroneView" ]; then
            print_success "Executable created: ./build/DroneView"
            print_status "To run the application: ./build/DroneView"
        fi
    elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
        if [ -f "./DroneView.exe" ]; then
            print_success "Executable created: ./build/DroneView.exe"
            print_status "To run the application: ./build/DroneView.exe"
        fi
    fi
    
else
    print_error "Build failed! Please check the error messages above."
    exit 1
fi