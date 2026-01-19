.data
    sabit: 5        ; RAM[0] = 5
    ; RAM[E] (14) Input
    ; RAM[F] (15) Output

.code
    ; 1. INPUT (Kullanicidan Sayi Al)
    LDA 14          ; Kullanici 5 girdi diyelim -> ACC = 5
    
    ; 2. ISLEM (Sabit Ekle)
    ADD [sabit]     ; ACC = 5 + 5 -> ACC = 10 (A)
    
    ; 3. KAYIT (RAM'e Yedekle)
    STA 15          ; RAM[15] = 10 oldu. (ACC hala 10)
    
    ; 4. CIKTI (Ekrana Bas)
    OUT             ; ACC icindeki degeri (10) ekrana basar.
    
    HLT    
