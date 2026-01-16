; --- 4-BIT FIBONACCI SERISI ---
; Seri: 0, 1, 1, 2, 3, 5, 8, 13(D) ...
; RAM[0] = A (Onceki Sayi)
; RAM[1] = B (Simdiki Sayi)
; RAM[2] = Yedek (Swap islemi icin)
; RAM[15] = EKRAN

BASLANGIC:
    ; --- Ilk Degerleri Ata (A=0, B=1) ---
    LDI 0
    STA 0       ; A = 0
    
    LDI 1
    STA 1       ; B = 1
    
    ; Ilk sayiyi (0) ekrana bas
    LDI 0
    STA 15

DONGU:
    ; --- 1. Adim: B'yi Ekrana Bas ---
    LDA 1       ; B'yi yukle
    STA 15      ; Ekrana bas (Visualizer'da izle)
    
    ; --- 2. Adim: Topla (A + B) ---
    LDA 0       ; A'yi yukle
    ADD [1]     ; B'yi ekle (ACC = A + B)
    
    ; --- 3. Adim: Tasma Kontrolu ---
    ; Eger sonuc 15'ten buyukse (8 + 13 = 21), Carry bayragi kalkar.
    JC BITIS    ; Carry varsa (Seri bitti) -> CIK
    
    ; --- 4. Adim: Degiskenleri Guncelle (Swap) ---
    ; Yeni A = Eski B
    ; Yeni B = Toplam (Su an ACC'de)
    
    STA 2       ; Toplami kaybetmemek icin RAM[2]'ye (Temp) koy
    
    LDA 1       ; Eski B'yi al
    STA 0       ; A'ya yaz (A guncellendi)
    
    LDA 2       ; Toplami (Temp) geri al
    STA 1       ; B'ye yaz (B guncellendi)
    
    JMP DONGU   ; Basa don

BITIS:
    LDA 1      ; Bitis isareti olarak F bas
    STA 15
    HLT