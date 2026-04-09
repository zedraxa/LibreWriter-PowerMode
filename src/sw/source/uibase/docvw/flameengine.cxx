#include <flameengine.hxx>
#include <flamespritecache.hxx>
#include <vcl/outdev.hxx>
#include <vcl/svapp.hxx>
#include <vcl/image.hxx>
#include <vcl/bitmap.hxx>
#include <tools/time.hxx>
#include <tools/mapunit.hxx>
#include <comphelper/lok.hxx>
#include <algorithm>
#include <cmath>
#include <cstdio>

FlameEngine& FlameEngine::Get()
{
    static FlameEngine s;
    return s;
}

FlameEngine::FlameEngine()
    : m_aTimer("sw FlameEngine")
{
    m_aTimer.SetInvokeHandler(LINK(this, FlameEngine, TimerHdl));
    m_aTimer.SetTimeout(TIMER_MS);
}

bool FlameEngine::ShouldSuppress()
{
    if (Application::IsHeadlessModeEnabled())
        return true;
    if (comphelper::LibreOfficeKit::isActive())
        return true;
    return false;
}

void FlameEngine::OnKeyStroke(vcl::Window* pWin, const Point& rCaretPixel, sal_Unicode cChar)
{
    if (ShouldSuppress() || !m_bEnabled)
        return;

    m_pWindow = pWin;

    // Reset combo on backspace or delete
    if (cChar == 8 || cChar == 127)
    {
        m_nCombo = 0;
        m_nWordCombo = 0;
        m_fHeatLevel = 1.0f;
        return;
    }

    // Skip control chars, but allow Space (32) and Enter (13/10)
    if (cChar < 32 && cChar != 13 && cChar != 10)
        return;

    FlameSpriteCache& rCache = FlameSpriteCache::Get();
    if (!rCache.IsLoaded())
        return;

    // Combo tracking
    sal_uInt64 nNow = tools::Time::GetSystemTicks();
    bool bInTimeWindow = (m_nLastTick > 0 && (nNow - m_nLastTick) <= sal_uInt64(COMBO_WINDOW));

    if (bInTimeWindow)
        m_nCombo = std::min(m_nCombo + 1, 999);
    else
    {
        m_nCombo = 1;
        m_nWordCombo = 0;
    }

    // Heat Level Tracking (Speed)
    if (m_nLastStrokeTick > 0)
    {
        sal_uInt64 nDiff = nNow - m_nLastStrokeTick;
        if (nDiff < 200) // Fast typing (<200ms between keys)
            m_fHeatLevel = std::min(m_fHeatLevel + 0.15f, 3.5f);
    }
    m_nLastStrokeTick = nNow;

    m_nLastTick = nNow;
    m_aLastCaretPos = rCaretPixel;

    EffectType eType = EffectType::Normal;
    int nDebrisCount = 1 + (rand() % 3);
    int nEmberCount = 2 + (rand() % 4);
    int nNewShake = 3;

    if (cChar == 32) // Space
    {
        m_nWordCombo++;
        eType = EffectType::Word;
        nDebrisCount = 6 + (rand() % 5);
        nEmberCount = 10 + (rand() % 10);
        nNewShake = 10 + std::min(m_nWordCombo, 15);
    }
    else if (cChar == 13 || cChar == 10) // Enter
    {
        m_nWordCombo++;
        eType = EffectType::Paragraph;
        nDebrisCount = 15 + (rand() % 10);
        nEmberCount = 20 + (rand() % 15);
        nNewShake = 20 + std::min(m_nWordCombo * 2, 30);
    }

    m_nShakeLife = std::max(m_nShakeLife, nNewShake);

    SpawnFlame(rCaretPixel, eType);
    SpawnEmbers(rCaretPixel, nEmberCount);
    SpawnDebris(rCaretPixel, nDebrisCount);
}

void FlameEngine::SpawnFlame(const Point& rPos, EffectType eType)
{
    float fComboScale = (1.0f + (m_nCombo - 1) * 0.01f) * (m_fHeatLevel * 0.4f + 0.6f);

    if (eType == EffectType::Word)
    {
        FlameInstance fi;
        fi.aPos = rPos;
        fi.nLife    = BASE_LIFE + 10;
        fi.nMaxLife = fi.nLife;
        fi.fScale   = fComboScale * 1.8f;
        fi.nFrame   = 0;
        fi.eType    = eType;
        m_aInstances.push_back(fi);
    }
    else if (eType == EffectType::Paragraph)
    {
        for (int i = 0; i < 6; ++i)
        {
            FlameInstance fi;
            fi.aPos = Point(rPos.X() + (rand() % 61 - 30), rPos.Y() + (rand() % 61 - 30));
            fi.nLife    = BASE_LIFE + 20;
            fi.nMaxLife = fi.nLife;
            fi.fScale   = fComboScale * 1.4f;
            fi.nFrame   = 0;
            fi.eType    = eType;
            m_aInstances.push_back(fi);
        }
    }
    else
    {
        FlameInstance fi;
        fi.aPos = rPos;
        fi.nLife    = BASE_LIFE + (m_nCombo / 4);
        fi.nMaxLife = fi.nLife;
        fi.fScale   = fComboScale;
        fi.nFrame   = 0;
        fi.eType    = eType;
        m_aInstances.push_back(fi);
    }

    while (m_aInstances.size() > size_t(MAX_INSTANCES))
        m_aInstances.erase(m_aInstances.begin());

    if (!m_aTimer.IsActive())
        m_aTimer.Start();

    InvalidateInstances();
    if (m_pWindow)
        m_pWindow->PaintImmediately();
}

void FlameEngine::SpawnEmbers(const Point& rPos, int nCount)
{
    for (int i = 0; i < nCount; ++i)
    {
        EmberInstance ei;
        ei.aPos = rPos;
        ei.aVel = Point(rand() % 11 - 5, -(rand() % 15 + 5));
        ei.nLife = 10 + (rand() % 20);
        m_aEmbers.push_back(ei);
    }
    // Limit embers
    if (m_aEmbers.size() > 100)
        m_aEmbers.erase(m_aEmbers.begin(), m_aEmbers.begin() + 20);
}

void FlameEngine::SpawnDebris(const Point& rPos, int nCount)
{
    for (int i = 0; i < nCount; ++i)
    {
        DebrisInstance di;
        di.aPos = rPos;
        di.aVel = Point(rand() % 15 - 7, rand() % 10 - 15); // Pop up then fall
        di.nLife = 20 + (rand() % 30);
        di.nSize = 2 + (rand() % 5);
        m_aDebris.push_back(di);
    }
    if (m_aDebris.size() > 150)
        m_aDebris.erase(m_aDebris.begin(), m_aDebris.begin() + 30);
}

void FlameEngine::UpdatePhysics()
{
    // Embers rise and flicker
    for (auto& e : m_aEmbers)
    {
        e.aPos.AdjustX(e.aVel.X() / 2);
        e.aPos.AdjustY(e.aVel.Y());
        e.nLife--;
    }
    m_aEmbers.erase(std::remove_if(m_aEmbers.begin(), m_aEmbers.end(), [](const EmberInstance& e){ return e.nLife <= 0; }), m_aEmbers.end());

    // Debris falls with gravity
    for (auto& d : m_aDebris)
    {
        d.aVel.AdjustY(2); // Gravity
        d.aPos.AdjustX(d.aVel.X());
        d.aPos.AdjustY(d.aVel.Y());
        d.nLife--;
    }
    m_aDebris.erase(std::remove_if(m_aDebris.begin(), m_aDebris.end(), [](const DebrisInstance& d){ return d.nLife <= 0; }), m_aDebris.end());

    // Shake decay
    if (m_nShakeLife > 0)
    {
        int nForce = m_nShakeLife / 2 + 1;
        m_aShakeOffset = Point(rand() % (nForce * 2 + 1) - nForce, rand() % (nForce * 2 + 1) - nForce);
        m_nShakeLife--;
    }
    else
    {
        m_aShakeOffset = Point(0, 0);
    }

    // Heat decay
    m_fHeatLevel = std::max(1.0f, m_fHeatLevel - 0.02f);
}

void FlameEngine::InvalidateInstances()
{
    if (!m_pWindow) return;
    m_pWindow->Invalidate(InvalidateFlags::NoChildren); // Rough but safe for chaos
}

void FlameEngine::InvalidateComboRect()
{
    // No-op, InvalidateInstances handles it
}

IMPL_LINK_NOARG(FlameEngine, TimerHdl, Timer*, void)
{
    UpdatePhysics();

    sal_uInt64 nNow = tools::Time::GetSystemTicks();
    if (m_nCombo > 1 && (nNow - m_nLastTick) > sal_uInt64(COMBO_WINDOW))
    {
        m_nCombo = 1;
        m_nWordCombo = 0;
    }

    FlameSpriteCache& rCache = FlameSpriteCache::Get();
    auto it = m_aInstances.begin();
    while (it != m_aInstances.end())
    {
        it->nLife--;
        it->nFrame++;
        if (it->nLife <= 0 || it->nFrame >= int(rCache.FrameCount()))
            it = m_aInstances.erase(it);
        else
            ++it;
    }

    if (IsActive())
    {
        InvalidateInstances();
        if (m_pWindow)
            m_pWindow->PaintImmediately();
    }
    else
    {
        m_aTimer.Stop();
        if (m_pWindow)
            m_pWindow->PaintImmediately();
    }
}

void FlameEngine::Paint(vcl::RenderContext& rRC)
{
    FlameSpriteCache& rCache = FlameSpriteCache::Get();
    if (!rCache.IsLoaded()) return;

    rRC.Push(vcl::PushFlags::MAPMODE | vcl::PushFlags::LINECOLOR | vcl::PushFlags::FILLCOLOR);
    rRC.SetMapMode(MapMode(MapUnit::MapPixel));

    // Draw Debris (Breaking parts)
    rRC.SetLineColor(COL_TRANSPARENT);
    for (const auto& d : m_aDebris)
    {
        rRC.SetFillColor(Color(120 + (rand()%30), 100 + (rand()%20), 80)); // Grey-brown stone
        rRC.DrawRect(tools::Rectangle(d.aPos, Size(d.nSize, d.nSize)));
    }

    // Draw Flames
    for (const auto& fi : m_aInstances)
    {
        size_t nIdx = size_t(fi.nFrame) % rCache.FrameCount();
        int nW = int(DRAW_W * fi.fScale);
        int nH = int(DRAW_H * fi.fScale);
        rRC.DrawImage(Point(fi.aPos.X() - nW / 2, fi.aPos.Y() - nH + 10), Size(nW, nH), Image(rCache.GetFrame(nIdx).GetBitmap()));
    }

    // Draw Embers
    rRC.SetFillColor(Color(255, 150 + (rand()%100), 0));
    for (const auto& e : m_aEmbers)
    {
        rRC.DrawRect(tools::Rectangle(e.aPos, Size(2, 2)));
    }

    // Draw Combo Text with Pulse & Shake
    if (m_nCombo >= 5)
    {
        double fPulse = 1.0 + 0.1 * std::sin(tools::Time::GetSystemTicks() * 0.015);
        int nFontSize = int((26 + std::min(m_nWordCombo, 25)) * fPulse);

        vcl::Font aFont("Arial", Size(0, nFontSize));
        aFont.SetWeight(WEIGHT_BOLD);
        if (m_nWordCombo > 10) aFont.SetItalic(ITALIC_NORMAL);
        rRC.SetFont(aFont);

        Color aColor = COL_LIGHTRED;
        if (m_nWordCombo >= 5) aColor = Color(255, 180, 0);
        if (m_nWordCombo >= 15) aColor = COL_YELLOW;
        if (m_nWordCombo >= 30) aColor = COL_CYAN;

        OUString aText = (m_nWordCombo > 0)
            ? (OUString::number(m_nWordCombo) + u" WORD COMBO!"_ustr)
            : (OUString::number(m_nCombo) + u" COMBO!"_ustr);

        // Smart placement: Slightly higher and to the right
        Point aTextPos(m_aLastCaretPos.X() + 45 + m_aShakeOffset.X(), m_aLastCaretPos.Y() - 85 + m_aShakeOffset.Y());

        rRC.SetTextColor(COL_BLACK);
        rRC.DrawText(Point(aTextPos.X() + 3, aTextPos.Y() + 3), aText);
        rRC.SetTextColor(aColor);
        rRC.DrawText(aTextPos, aText);
    }

    rRC.Pop();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
