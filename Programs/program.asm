.data
    sabit: 5        ; RAM[0] = 5

.code
    LDA 14          ; Kullanici 5 girdi diyelim -> ACC = 5
    
    ADD [sabit]     ; ACC = 5 + 5 -> ACC = 10 (A)
    
    STA 15          ; RAM[15] = 10 oldu. (ACC hala 10)
    
    OUT             ; ACC icindeki degeri (10) ekrana basar.
    
    HLT    
