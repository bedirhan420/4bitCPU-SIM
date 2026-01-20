#!/bin/bash
set -e # Hata olursa scripti anında durdur

APP_NAME="4BitCPU"
EXECUTABLE_NAME="cpu_sim"
ICON_SOURCE="icon.png" # Senin kaynak ikon dosyan (örn: 1024x1024 png)
BUNDLE_ROOT="${APP_NAME}.app"
CONTENTS_DIR="${BUNDLE_ROOT}/Contents"
MACOS_DIR="${CONTENTS_DIR}/MacOS"
RESOURCES_DIR="${CONTENTS_DIR}/Resources"

echo "======================================="
echo "📦 macOS Uygulama Paketleyici v3 (Final)"
echo "======================================="

# 1. Temizlik (Önceki kalıntıları yok et)
echo "🧹 Temizlik yapılıyor..."
rm -rf "${BUNDLE_ROOT}"

# 2. İskeleti Oluştur
echo "🏗️ Klasör yapısı oluşturuluyor..."
mkdir -p "${MACOS_DIR}"
mkdir -p "${RESOURCES_DIR}"

# 3. Ana Dosyaları Kopyala
echo "🚀 Executable ve Kaynaklar kopyalanıyor..."

# Executable kontrolü
if [ ! -f "${EXECUTABLE_NAME}" ]; then
    echo "❌ HATA: '${EXECUTABLE_NAME}' bulunamadı! Lütfen önce 'make' çalıştırın."
    exit 1
fi
cp "${EXECUTABLE_NAME}" "${MACOS_DIR}/"
chmod +x "${MACOS_DIR}/${EXECUTABLE_NAME}"

# Fonts klasörü kontrolü (Fontlar executable'ın yanına konmalı)
if [ -d "Fonts" ]; then
    echo "📂 Fonts klasörü pakete ekleniyor..."
    cp -r "Fonts" "${MACOS_DIR}/"
else
    echo "⚠️ UYARI: 'Fonts' klasörü bulunamadı! Yazılar bozuk çıkacak."
fi

# 4. Info.plist Oluştur (Standartlara uygun)
echo "📝 Info.plist yazılıyor..."
cat > "${CONTENTS_DIR}/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>English</string>
    <key>CFBundleExecutable</key>
    <string>${EXECUTABLE_NAME}</string>
    <key>CFBundleIconFile</key>
    <string>AppIcon</string>
    <key>CFBundleIdentifier</key>
    <string>com.bedirhancelik.${APP_NAME}</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>${APP_NAME}</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0.0</string>
    <key>CFBundleVersion</key>
    <string>1</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.15</string>
    <key>NSHighResolutionCapable</key>
    <true/>
</dict>
</plist>
EOF

# 5. İkon Dönüştürme (Varsa)
if [ -f "${ICON_SOURCE}" ]; then
    echo "🎨 İkon dönüştürülüyor (Bu biraz sürebilir)..."
    ICONSET_TEMP="TempIcon.iconset"
    mkdir -p "${ICONSET_TEMP}"
    
    # Tüm gerekli boyutları oluştur
    sips -z 16 16     "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_16x16.png" &> /dev/null
    sips -z 32 32     "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_16x16@2x.png" &> /dev/null
    sips -z 32 32     "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_32x32.png" &> /dev/null
    sips -z 64 64     "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_32x32@2x.png" &> /dev/null
    sips -z 128 128   "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_128x128.png" &> /dev/null
    sips -z 256 256   "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_128x128@2x.png" &> /dev/null
    sips -z 512 512   "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_256x256.png" &> /dev/null
    sips -z 512 512   "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_256x256@2x.png" &> /dev/null
    sips -z 1024 1024 "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_512x512.png" &> /dev/null
    sips -z 1024 1024 "${ICON_SOURCE}" --out "${ICONSET_TEMP}/icon_512x512@2x.png" &> /dev/null
    
    # .icns dosyasına paketle ve Resources içine koy
    iconutil -c icns "${ICONSET_TEMP}" -o "${RESOURCES_DIR}/AppIcon.icns"
    rm -rf "${ICONSET_TEMP}"
else
    echo "ℹ️ '${ICON_SOURCE}' bulunamadı, varsayılan sistem ikonu kullanılacak."
fi

# 6. Son Dokunuş (Finder'ı Tetikleme)
echo "✨ Paket mühürleniyor..."
touch "${BUNDLE_ROOT}"

echo "======================================="
echo "✅ İŞLEM TAMAMLANDI: ${BUNDLE_ROOT}"
echo "⚠️ ÖNEMLİ: Eğer ikon hala görünmüyorsa veya adı garipse:"
echo "   Oluşan uygulamayı Masaüstüne taşıyın ve geri getirin."
echo "======================================="