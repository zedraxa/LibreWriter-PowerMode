#ifndef INCLUDED_SW_POWERMODE_HXX
#define INCLUDED_SW_POWERMODE_HXX

#include <vcl/window.hxx>
#include <vcl/timer.hxx>
#include <vcl/graphicobj.hxx>
#include <vcl/floatwin.hxx>
#include <memory>
#include <vector>

class SwEditWin;

// Her bir alev efektini temsil eden şeffaf pencere sınıfı
class PowerModeFireEffect : public FloatingWindow
{
private:
    Timer maDestructTimer;     // Efektin ekranda kalma süresi
    GraphicObject maFireAnim;  // Animasyonlu GIF

    // Timer tetiklendiğinde çalışacak fonksiyon
    DECL_LINK(OnTimer, Timer*, void);

public:
    PowerModeFireEffect(Window* pParent, const Point& rPos);
    virtual ~PowerModeFireEffect() override;

    // Pencere çizimi (GraphicObject buraya çizilir)
    virtual void Paint(vcl::RenderContext& rRenderContext, const tools::Rectangle& rRect) override;
};

// Singleton yönetici: Ekranda aynı anda çok fazla animasyon olmasını engeller
class PowerModeManager
{
private:
    std::vector<VclPtr<PowerModeFireEffect>> m_aActiveEffects;
public:
    static PowerModeManager& Get();
    void SpawnFire(Window* pParent, const Point& rPos);
    void CleanupOldEffects();
};

#endif // INCLUDED_SW_POWERMODE_HXX
