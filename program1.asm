; 4-Bit Carpma Islemi (3 x 4 = 12)
; Yontem: Tekrarli Toplama
; RAM[0]  = Carpilan (3)
; RAM[1]  = Carpan (4 - Sayac olarak kullanacagiz)
; RAM[2]  = Sonuc
; RAM[10] = Sabit Sayi 1 (Azaltma islemi icin)
; RAM[15] = EKRAN

BASLANGIC:
    ; --- Kurulum (Variable Initialization) ---
    
    ; Azaltma islemi icin RAM[10]'a 1 koyalim
    ; Cunku SUB komutumuz sadece RAM adresinden calisiyor (SUB Immediate yok)
    LDI 1
    STA 10
    
    ; RAM[0] = 3 (Carpilacak Sayi)
    LDI 3
    STA 0
    
    ; RAM[1] = 4 (Kac kere toplanacak?)
    LDI 4
    STA 1
    
    ; RAM[2] = 0 (Sonuc baslangici)
    LDI 0
    STA 2

DONGU:
    ; --- Sayac Kontrolu ---
    LDA 1       ; Sayaci (Kalan carpani) yukle
    JZ BITIS    ; Eger sayac 0 ise islem bitmistir -> CIK
    
    ; --- Toplama Islemi (Sonuc = Sonuc + RAM[0]) ---
    LDA 2       ; Mevcut sonucu yukle
    ADD [0]     ; Ustune 3 ekle
    STA 2       ; Geri kaydet
    
    STA 15      ; *Gorsellestirme*: Sonucun arttigini ekranda izle
    
    ; --- Sayac Azaltma (Sayac = Sayac - 1) ---
    LDA 1       ; Sayaci yukle
    SUB [10]    ; 1 cikar (RAM[10]'daki 1'i kullaniyoruz)
    STA 1       ; Sayaci guncelle
    
    JMP DONGU   ; Basa don

BITIS:
    ; Sonucu goster ve dur
    LDA 2
    STA 15
    HLT