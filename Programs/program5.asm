BASLA:
    LDI 5
    STA 0       ; Sayac = 5

DONGU:
    ; ... İşlemlerini yap ...
    
    ; Sayacı 1 azalt
    LDI 1
    STA 10      ; Temp = 1
    LDA 0       ; Sayac'ı al
    SUB [10]    ; ACC = Sayac - 1
    STA 0       ; Yeni değeri kaydet
    
    ; --- KRİTİK NOKTA: JNZ SİMÜLASYONU ---
    ; Şu an ACC'de yeni değer var.
    
    JZ BITIS    ; Eğer sonuç 0 olduysa (Z=1), BITIS'e git (Döngüden çık).
    
    JMP DONGU   ; Eğer 0 olmadıysa (JZ çalışmadı), yukarı zıpla (Devam et).

BITIS:
    HLT