#!/bin/bash
# T469 Car Infotainment System - Automated Build Script
# Usage: ./build.sh [component]
# Components: all, qt, alsa, mplayer, car, deploy

set -e

PROJECT_DIR="$HOME/T469"
SRC_DIR="$PROJECT_DIR/src"
COMPILED_DIR="$PROJECT_DIR/compiled"
QT_INSTALL="/opt/qt5-arm"
CAR_APP="$PROJECT_DIR/car-app"
DEPLOY_DIR="$PROJECT_DIR/deploy"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log_info() { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

check_toolchain() {
    if ! command -v arm-linux-gnueabi-gcc &> /dev/null; then
        log_error "ARM cross-compiler not found. Install with:"
        log_error "sudo apt install gcc-arm-linux-gnueabi g++-arm-linux-gnueabi"
        exit 1
    fi
    log_info "Toolchain: $(arm-linux-gnueabi-gcc --version | head -1)"
}

build_zlib() {
    log_info "Building zlib-1.2.11..."
    cd "$SRC_DIR/zlib-1.2.11"
    CC=arm-linux-gnueabi-gcc ./configure --prefix="$COMPILED_DIR/zlib-1.2.11"
    make -j$(nproc)
    make install
    log_info "zlib built successfully"
}

build_alsa_lib() {
    log_info "Building alsa-lib-1.2.2..."
    cd "$SRC_DIR/alsa-lib-1.2.2"
    ./configure --host=arm-linux-gnueabi --prefix="$PWD/build"
    make -j$(nproc)
    make install
    log_info "alsa-lib built successfully"
}

build_alsa_utils() {
    log_info "Building alsa-utils-1.2.2..."
    cd "$SRC_DIR/alsa-utils-1.2.2"

    ALSA_PREFIX="$COMPILED_DIR/alsa-lib-1.2.2/build"
    export CC=arm-linux-gnueabi-gcc
    export CXX=arm-linux-gnueabi-g++
    export AR=arm-linux-gnueabi-ar
    export STRIP=arm-linux-gnueabi-strip
    export RANLIB=arm-linux-gnueabi-ranlib
    export PKG_CONFIG_PATH="${ALSA_PREFIX}/lib/pkgconfig"
    export CFLAGS="-I${ALSA_PREFIX}/include"
    export LDFLAGS="-L${ALSA_PREFIX}/lib"

    ./configure --host=arm-linux-gnueabi \
        --prefix="$PWD/build" \
        --disable-alsamixer \
        --disable-xmlto

    make -j$(nproc)

    # Copy binaries to compiled directory
    mkdir -p "$COMPILED_DIR/alsa-utils-1.2.2/bin" "$COMPILED_DIR/alsa-utils-1.2.2/sbin"
    cp aplay/aplay amixer/amixer amidi/amidi alsaucm/alsaucm topology/alsatplg \
        "$COMPILED_DIR/alsa-utils-1.2.2/bin/"
    cp alsactl/alsactl "$COMPILED_DIR/alsa-utils-1.2.2/sbin/"
    cp seq/aconnect/aconnect seq/aplaymidi/aplaymidi seq/aseqdump/aseqdump seq/aseqnet/aseqnet \
        "$COMPILED_DIR/alsa-utils-1.2.2/bin/"

    log_info "alsa-utils built successfully"
}

build_qt() {
    log_info "Building Qt 5.12.12 for ARM..."

    # Fix line endings if transferred from Windows
    if file "$SRC_DIR/qt-everywhere-src-5.12.12/configure" | grep -q CRLF; then
        log_warn "Fixing Windows line endings..."
        find "$SRC_DIR/qt-everywhere-src-5.12.12" -type f \
            \( -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.conf' \
               -o -name '*.prf' -o -name '*.pro' -o -name '*.pri' -o -name '*.json' \) \
            -exec dos2unix {} +
    fi

    mkdir -p "$PROJECT_DIR/build-qt-5.12.12"
    cd "$PROJECT_DIR/build-qt-5.12.12"

    "$SRC_DIR/qt-everywhere-src-5.12.12/configure" \
        -release -opensource -confirm-license \
        -xplatform linux-arm-gnueabi-g++ \
        -prefix "$QT_INSTALL" \
        -no-opengl -no-vulkan -no-openssl -no-xcb \
        -nomake examples -nomake tests \
        -skip qt3d -skip qtactiveqt -skip qtandroidextras -skip qtcanvas3d \
        -skip qtcharts -skip qtconnectivity -skip qtdatavis3d -skip qtdeclarative \
        -skip qtdoc -skip qtgamepad -skip qtgraphicaleffects -skip qtlocation \
        -skip qtmacextras -skip qtmultimedia -skip qtnetworkauth \
        -skip qtpurchasing -skip qtquickcontrols -skip qtquickcontrols2 \
        -skip qtremoteobjects -skip qtscript -skip qtscxml -skip qtsensors \
        -skip qtserialbus -skip qtserialport -skip qtspeech -skip qtsvg \
        -skip qttools -skip qttranslations -skip qtvirtualkeyboard -skip qtwayland \
        -skip qtwebchannel -skip qtwebengine -skip qtwebsockets -skip qtwebview \
        -skip qtwinextras -skip qtx11extras -skip qtxmlpatterns

    make -j$(nproc)
    sudo make install
    log_info "Qt installed to $QT_INSTALL"
}

build_car() {
    log_info "Building car application..."
    cd "$CAR_APP"

    # Clean previous build
    rm -f *.o moc_* qrc_* car

    # Generate Makefile
    "$QT_INSTALL/bin/qmake" car.pro

    # Build
    make -j$(nproc)
    log_info "Car application built: $CAR_APP/car"
}

create_deploy() {
    log_info "Creating deployment package..."
    rm -rf "$DEPLOY_DIR"
    mkdir -p "$DEPLOY_DIR/bin" "$DEPLOY_DIR/lib"

    # Copy car application
    cp "$CAR_APP/car" "$DEPLOY_DIR/bin/"

    # Copy Qt libraries
    cp "$QT_INSTALL/lib/"libQt5*.so* "$DEPLOY_DIR/lib/" 2>/dev/null || true

    # Copy ALSA tools
    cp "$COMPILED_DIR/alsa-utils-1.2.2/bin/"* "$DEPLOY_DIR/bin/" 2>/dev/null || true
    cp "$COMPILED_DIR/alsa-utils-1.2.2/sbin/"* "$DEPLOY_DIR/bin/" 2>/dev/null || true

    # Copy MPlayer
    cp "$COMPILED_DIR/MPlayer-1.4/build/bin/mplayer" "$DEPLOY_DIR/bin/" 2>/dev/null || true

    # Copy resources
    cp -r "$CAR_APP/img" "$DEPLOY_DIR/" 2>/dev/null || true
    cp "$CAR_APP/style.qss" "$DEPLOY_DIR/" 2>/dev/null || true

    # Create run script
    cat > "$DEPLOY_DIR/run.sh" << 'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
export LD_LIBRARY_PATH="$SCRIPT_DIR/lib:$LD_LIBRARY_PATH"
export QT_QPA_PLATFORM=linuxfb
"$SCRIPT_DIR/bin/car" "$@"
EOF
    chmod +x "$DEPLOY_DIR/run.sh"

    log_info "Deployment package created: $DEPLOY_DIR ($(du -sh "$DEPLOY_DIR" | cut -f1))"
}

# Main
check_toolchain

case "${1:-all}" in
    all)
        build_zlib
        build_alsa_lib
        build_alsa_utils
        build_qt
        build_car
        create_deploy
        ;;
    qt) build_qt ;;
    alsa) build_alsa_lib && build_alsa_utils ;;
    mplayer) log_info "MPlayer build not implemented in this script" ;;
    car) build_car ;;
    deploy) create_deploy ;;
    *)
        echo "Usage: $0 [all|qt|alsa|car|deploy]"
        exit 1
        ;;
esac

log_info "Build completed successfully!"
