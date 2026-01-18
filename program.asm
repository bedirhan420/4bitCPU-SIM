.data
    dizi: 3 4 2     ; RAM[0]=3, RAM[1]=4, RAM[2]=2
    ptr:  0         ; RAM[3] (Pointer)
    cnt:  3         ; RAM[4] (Sayaç)

.code
    ; --- BASLANGIC ---
    LDI 0
    STA 15          ; Sonucu temizle
    
DONGU:
    LDA cnt
    JZ BITIS
    
    LDAI ptr        ; Pointer verisi (Doğru çalışıyor!)
    
    ; --- DUZELTME BURADA ---
    ADD [15]        ; Buraya [sum] veya [F] degil, [15] yaz!
    STA 15          ; Kaydet
    
    LDI 1
    ADD [ptr]       ; Pointer artir (Bu da artık doğru çalışıyor!)
    STA ptr
    
    LDI 1
    STA 10          ; Temp
    LDA cnt
    SUB [10]        ; Sayac azalt
    STA cnt
    
    JMP DONGU

BITIS:
    HLT