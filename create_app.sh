#!/bin/bash
set -e

# --- AYARLAR ---
APP_NAME="4BitCPU"

EXECUTABLE_NAME="cpu_sim" 
ICON_SOURCE="icon.png"

BUNDLE_ROOT="${APP_NAME}.app"
CONTENTS_DIR="${BUNDLE_ROOT}/Contents"
MACOS_DIR="${CONTENTS_DIR}/MacOS"
RESOURCES_DIR="${CONTENTS_DIR}/Resources"
FRAMEWORKS_DIR="${CONTENTS_DIR}/Frameworks"

echo "======================================="
echo "📦 macOS Ultra Paketleyici (V2: Icon Fix)"
echo "======================================="

# 1. Temizlik
rm -rf "${BUNDLE_ROOT}"

# 2. İskelet
mkdir -p "${MACOS_DIR}"
mkdir -p "${RESOURCES_DIR}"
mkdir -p "${FRAMEWORKS_DIR}"

# 3. Kopyalama
echo "🚀 Executable kopyalanıyor..."
if [ ! -f "${EXECUTABLE_NAME}" ]; then
    echo "❌ HATA: '${EXECUTABLE_NAME}' dosyası bulunamadı!"
    echo "   Lütfen 'make' komutunu çalıştırın."
    exit 1
fi
cp "${EXECUTABLE_NAME}" "${MACOS_DIR}/"
chmod +x "${MACOS_DIR}/${EXECUTABLE_NAME}"

if [ -d "Fonts" ]; then
    cp -r "Fonts" "${MACOS_DIR}/"
fi

# 4. Raylib Kütüphanesini Gömme
echo "🧰 Raylib işlemleri..."
RAYLIB_PREFIX=$(brew --prefix raylib 2>/dev/null || true)
if [ -z "$RAYLIB_PREFIX" ]; then
    echo "❌ HATA: Raylib Homebrew ile yüklü değil."
    exit 1
fi

RAYLIB_DYLIB="${RAYLIB_PREFIX}/lib/libraylib.dylib"
cp "${RAYLIB_DYLIB}" "${FRAMEWORKS_DIR}/"
chmod 755 "${FRAMEWORKS_DIR}/libraylib.dylib"

# 5. YOL DEĞİŞTİRME (SWAP)
echo "🔧 Kütüphane yolları güncelleniyor..."
TARGET_BIN="${MACOS_DIR}/${EXECUTABLE_NAME}"

# A) Rpath ekle 
install_name_tool -add_rpath "@executable_path/../Frameworks" "${TARGET_BIN}"

# B) Eski yolu bul ve değiştir
OLD_PATH=$(otool -L "${TARGET_BIN}" | grep "libraylib" | awk '{print $1}')
if [ -n "$OLD_PATH" ]; then
    echo "   Eski Yol: ${OLD_PATH}"
    echo "   Yeni Yol: @rpath/libraylib.dylib"
    install_name_tool -change "${OLD_PATH}" "@rpath/libraylib.dylib" "${TARGET_BIN}"
fi

# C) ID Güncelle ve İmzala
install_name_tool -id "@rpath/libraylib.dylib" "${FRAMEWORKS_DIR}/libraylib.dylib"
codesign --force --sign - "${TARGET_BIN}" &> /dev/null
codesign --force --sign - "${FRAMEWORKS_DIR}/libraylib.dylib" &> /dev/null

# 6. Info.plist
cat > "${CONTENTS_DIR}/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>${EXECUTABLE_NAME}</string>
    <key>CFBundleIconFile</key>
    <string>AppIcon</string>
    <key>CFBundleIdentifier</key>
    <string>com.bedirhancelik.${APP_NAME}</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
    <key>NSHighResolutionCapable</key>
    <true/>
</dict>
</plist>
EOF

# 7. İkon (DÜZELTİLEN KISIM)
if [ -f "${ICON_SOURCE}" ]; then
    echo "🎨 İkon seti oluşturuluyor..."
    ICONSET_TEMP="TempIcon.iconset"
    mkdir -p "${ICONSET_TEMP}"
    
    # iconutil tüm bu boyutları İSTER, yoksa hata verir:
    sips -z 16 16     "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_16x16.png" &> /dev/null
    sips -z 32 32     "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_16x16@2x.png" &> /dev/null
    sips -z 32 32     "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_32x32.png" &> /dev/null
    sips -z 64 64     "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_32x32@2x.png" &> /dev/null
    sips -z 128 128   "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_128x128.png" &> /dev/null
    sips -z 256 256   "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_128x128@2x.png" &> /dev/null
    sips -z 256 256   "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_256x256.png" &> /dev/null
    sips -z 512 512   "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_256x256@2x.png" &> /dev/null
    sips -z 512 512   "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_512x512.png" &> /dev/null
    sips -z 1024 1024 "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_512x512@2x.png" &> /dev/null
    
    iconutil -c icns "${ICONSET_TEMP}" -o "${RESOURCES_DIR}/AppIcon.icns"
    rm -rf "${ICONSET_TEMP}"
else
    echo "⚠️ UYARI: icon.png bulunamadı, varsayılan ikon kullanılacak."
fi

touch "${BUNDLE_ROOT}"
echo "✅ İŞLEM TAMAMLANDI."