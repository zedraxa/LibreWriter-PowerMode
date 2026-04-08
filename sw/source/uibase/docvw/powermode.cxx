#include "powermode.hxx"
#include <svtools/graphicfilter.hxx>
#include <vcl/svapp.hxx>

PowerModeFireEffect::PowerModeFireEffect(Window* pParent, const Point& rPos)
    : FloatingWindow(pParent, WB_NOBORDER | WB_SYSTEMWINDOW) // Çerçevesiz serbest pencere
{
    // Pencere arka planını şeffaf yap
    SetBackground(Wallpaper(COL_TRANSPARENT));

    // GIF Yükleme (MVP için sabit yol, sonradan kaynak kod veya profile alınabilir)
    GraphicFilter& rFilter = GraphicFilter::GetGraphicFilter();
    Graphic aGraphic;
    OUString aPath = "/tmp/fire.gif";
    rFilter.ImportGraphic(aGraphic, INetURLObject(aPath));
    maFireAnim = GraphicObject(aGraphic);

    // Animasyonu başlat
    maFireAnim.StartAnimation(this, Point(0,0), Size(40,40));

    // Pencerenin boyutunu ve konumunu ayarla
    SetPosSizePixel(rPos, Size(40, 40));
    Show();

    // 300 ms sonra bu pencereyi yok eden timer
    maDestructTimer.SetInvokeHandler(LINK(this, PowerModeFireEffect, OnTimer));
    maDestructTimer.SetTimeout(300);
    maDestructTimer.Start();
}

PowerModeFireEffect::~PowerModeFireEffect()
{
    maFireAnim.StopAnimation(this);
}

IMPL_LINK_NOARG(PowerModeFireEffect, OnTimer, Timer*, void)
{
    // Süre dolunca pencereyi güvenle yok et
    disposeOnce();
}

void PowerModeFireEffect::Paint(vcl::RenderContext& rRenderContext, const tools::Rectangle& /*rRect*/)
{
    // GIF çizim işlemi
    maFireAnim.Draw(rRenderContext, Point(0,0), Size(40,40));
}

// ---- Manager Sınıfı ----
PowerModeManager& PowerModeManager::Get()
{
    static PowerModeManager instance;
    return instance;
}

void PowerModeManager::SpawnFire(Window* pParent, const Point& rPos)
{
    CleanupOldEffects();

    // İmleç pozisyonuna göre biraz hizalama yap (sağ üst gibi)
    Point aAdjustedPos(rPos.X() + 5, rPos.Y() - 20);

    auto pEffect = VclPtr<PowerModeFireEffect>::Create(pParent, aAdjustedPos);
    m_aActiveEffects.push_back(pEffect);
}

void PowerModeManager::CleanupOldEffects()
{
    // Performans için maksimum 5 eşzamanlı alev animasyonu
    while (m_aActiveEffects.size() > 5) {
        if (m_aActiveEffects.front()) {
            m_aActiveEffects.front()->disposeOnce();
        }
        m_aActiveEffects.erase(m_aActiveEffects.begin());
    }
}
