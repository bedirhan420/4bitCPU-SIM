; --- RASTGELE IKI SAYIYI KARSILASTIRMA (Full Assembly) ---
; RAM[14] = Input Port (RNG - Rastgele Sayi Verir)
; RAM[15] = Output Port (Ekran)
; RAM[5]  = 1. Sayi (A)
; RAM[6]  = 2. Sayi (B)
; RAM[10] = Gecici Depo (Temp)

BASLA:
    ; --- 1. ADIM: Sayilari Uret ---
    
    ; Birinci sayiyi donanimdan iste
    LDA 14      ; ACC = Random()
    STA 5       ; RAM[5]'e kaydet (A)
    
    ; Ikinci sayiyi donanimdan iste
    LDA 14      ; ACC = Random()
    STA 6       ; RAM[6]'ya kaydet (B)
    
    ; --- 2. ADIM: Karsilastir (B - A) ---
    
    LDA 6       ; B'yi yukle
    STA 10      ; B'yi RAM[10]'da yedekle (Cunku SUB bozacak)
    
    SUB [5]     ; ACC = B - A
    
    ; Mantik:
    ; Eger B < A ise sonuc negatiftir -> Carry (Borc) olusur -> JC calisir.
    ; Eger B >= A ise sonuc pozitiftir -> Carry olusmaz.
    
    JC A_BUYUK  ; Eger Carry varsa A daha buyuktur, oraya git.

B_BUYUK:
    ; Buradaysak B >= A demektir.
    LDA 10      ; Yedekledigimiz B'yi geri al (Cunku ACC su an fark sonucu)
    STA 15      ; Ekrana bas
    HLT         ; Bitir

A_BUYUK:
    ; Buradaysak A > B demektir.
    LDA 5       ; A'yi yukle
    STA 15      ; Ekrana bas
    HLT