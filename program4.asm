; --- FONKSIYON TESTI ---

BASLA:
    LDI 3
    CALL IKI_KATINI_AL ; Alt programa git
    STA 15             ; Sonucu goster (6 olmali)
    HLT

; --- ALT PROGRAM (Fonksiyon) ---
IKI_KATINI_AL:
    ; ACC'deki sayiyi kendisiyle toplar (x2)
    STA 10    ; Yedekle
    ADD [10]  ; Kendisini ekle
    RET       ; Geri don