; --- DIZI TOPLAMA (POINTER: RAM[12]) ---
; RAM[0]=3, RAM[1]=4, RAM[2]=2 (Toplanacak Sayilar)
; RAM[12] = Pointer
; RAM[13] = Sayac
; RAM[15] = Toplam

BASLA:
    ; --- 1. ADIM: Diziyi Doldur (BU KISIM COK ONEMLI) ---
    LDI 3
    STA 0   ; RAM[0] = 3 olsun

    LDI 4
    STA 1   ; RAM[1] = 4 olsun

    LDI 2
    STA 2   ; RAM[2] = 2 olsun

    ; --- 2. ADIM: Degiskenleri Hazirla ---
    LDI 0
    STA 12  ; Pointer (RAM[12]) baslangicta 0 olsun (RAM[0]'i gostersin)

    LDI 3
    STA 13  ; Sayac (RAM[13]) 3 olsun (3 sayi toplayacagiz)

    LDI 0
    STA 15  ; Toplam (RAM[15]) sifirla

DONGU:
    ; Sayac bitti mi?
    LDA 13
    JZ BITIS

    ; Pointer'in gosterdigi sayiyi al
    LDAI 12     ; ACC = RAM[ RAM[12] ] (Yani RAM[0]'daki 3 gelecek)

    ; Toplama ekle
    ADD [15]
    STA 15      ; Sonucu yaz

    ; Pointer'i 1 arttir
    LDI 1
    ADD [12]
    STA 12

    ; Sayaci 1 azalt
    LDI 1
    STA 10      ; Temp
    LDA 13
    SUB [10]
    STA 13

    JMP DONGU

BITIS:
    HLT