# LibreOffice Writer Fire Mode v0.1 - Kurulum ve Patch Rehberi

Şu anki dizine (`sw/source/uibase/inc/` ve `sw/source/uibase/docvw/`) `powermode.hxx` ve `powermode.cxx` dosyalarını ekledim. 

LibreOffice derleme ortamınızda bu özelliği aktifleştirmek için aşağıdaki varolan LibreOffice dosyalarında değişiklik (patch) yapmanız gerekiyor:

### 1. `sw/source/uibase/docvw/edtwin.cxx` Dosyasına Hook Ekleme

Aşağıdaki include komutlarını dosyanın başlarına ekleyin:
```cpp
#include "powermode.hxx"
#include <swcrsr.hxx> // Caret pozisyonu için gereklidir
```

`SwEditWin::KeyInput(const KeyEvent& rKEvt)` fonksiyonunu bulun ve içine şu bloğu ekleyin:
```cpp
void SwEditWin::KeyInput(const KeyEvent& rKEvt)
{
    // ... Varolan ilk kodlar ...

    // [EKLENEN POWER MODE KODU - BAŞLANGIÇ]
    // Karakter tuşlaması olup olmadığını ve özel bir CTRL/ALT tuşu basilip basılmadığını kontrol ediyoruz.
    if (rKEvt.GetCharCode() != 0 && rKEvt.GetKeyCode().GetModifier() == 0)
    {
        SwCursorShell* pCursorSh = GetCursorShell();
        if (pCursorSh)
        {
            tools::Rectangle aCharRect = pCursorSh->GetCharRect();
            Point aPos = aCharRect.TopRight(); 
            
            // Writer'ın metin alanı pixel koordinatlarını al
            Point aPixelPos = LogicToPixel(aPos);
            
            PowerModeManager::Get().SpawnFire(this, aPixelPos);
        }
    }
    // [EKLENEN POWER MODE KODU - BİTİŞ]

    // ... Fonksiyonun geri kalanı ...
}
```

### 2. Derleme Sistemi Dosyası: `sw/Library_sw.mk`

Make sisteminin yeni `.cxx` dosyamızı derlediğinden emin olmak için `sw/Library_sw.mk` dosyasında bulunan `gb_Library_add_exception_objects` listesine `powermode` dosyamızı yol olarak eklememiz gerekir:

```makefile
$(eval $(call gb_Library_add_exception_objects,sw,\
    ...
    sw/source/uibase/docvw/powermode \
    ...
))
```

### 3. Derleme ve Test

1. Terminalinizden LibreOffice kök dizinine gidin.
2. `/tmp/fire.gif` adresine şeffaf arka planlı bir ateş animasyonu (.gif) koyun.
3. Sadece writer (`sw`) modülünü hızlıca derlemek için:
   ```bash
   make sw
   ```
4. Derleme bittikten sonra LibreOffice uygulamanızı (`instdir/program/soffice.bin --writer`) çalıştırın ve Writer ekranında herhangi bir tuşa bastığınızda imlecin yanında alev efektinin çıktığını göreceksiniz!
