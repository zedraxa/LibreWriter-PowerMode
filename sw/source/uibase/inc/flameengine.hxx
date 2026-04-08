/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef INCLUDED_SW_SOURCE_UIBASE_INC_FLAMEENGINE_HXX
#define INCLUDED_SW_SOURCE_UIBASE_INC_FLAMEENGINE_HXX

#include <vcl/timer.hxx>
#include <vcl/window.hxx>
#include <tools/gen.hxx>
#include <vector>

enum class EffectType { Normal, Word, Paragraph };

struct FlameInstance
{
    Point      aPos;
    int        nFrame = 0;
    int        nLife  = 0;
    int        nMaxLife = 0;
    float      fScale = 1.0f;
    EffectType eType  = EffectType::Normal;
};

struct EmberInstance
{
    Point aPos;
    Point aVel;
    int   nLife = 0;
};

struct DebrisInstance
{
    Point aPos;
    Point aVel;
    int   nLife = 0;
    int   nSize = 0;
};

class FlameEngine final
{
public:
    static FlameEngine& Get();

    /** Called from end of KeyInput — fires on EVERY keystroke.
     *  Force-flushes the text buffer before reading caret position. */
    void OnKeyStroke(vcl::Window* pWin, const Point& rCaretPixel, sal_Unicode cChar);

    void Paint(vcl::RenderContext& rRC);
    bool IsActive() const { return !m_aInstances.empty() || !m_aEmbers.empty() || !m_aDebris.empty() || m_nShakeLife > 0; }

    bool IsEnabled() const { return m_bEnabled; }
    void SetEnabled(bool bEnabled) { m_bEnabled = bEnabled; }

    bool  IsShaking() const { return m_nShakeLife > 0; }
    Point GetShakeOffset() const { return m_aShakeOffset; }

private:
    FlameEngine();
    DECL_LINK(TimerHdl, Timer*, void);

    void SpawnFlame(const Point& rPos, EffectType eType = EffectType::Normal);
    void SpawnEmbers(const Point& rPos, int nCount);
    void SpawnDebris(const Point& rPos, int nCount);
    void UpdatePhysics();

    void InvalidateInstances();
    void InvalidateComboRect();
    static bool ShouldSuppress();

    AutoTimer                 m_aTimer;
    vcl::Window*              m_pWindow = nullptr;
    bool                      m_bEnabled = true;
    std::vector<FlameInstance> m_aInstances;
    std::vector<EmberInstance> m_aEmbers;
    std::vector<DebrisInstance> m_aDebris;

    sal_uInt64 m_nLastTick = 0;
    int        m_nCombo = 0;
    int        m_nWordCombo = 0;
    Point      m_aLastCaretPos;

    // Shake & Juice
    int   m_nShakeLife = 0;
    Point m_aShakeOffset;
    float m_fHeatLevel = 1.0f;
    sal_uInt64 m_nLastStrokeTick = 0;

    static constexpr int TIMER_MS       = 20;   // 50 fps
    static constexpr int MAX_INSTANCES  = 12;
    static constexpr int BASE_LIFE      = 35;   // 20ms × 35 = 700ms
    static constexpr int COMBO_WINDOW   = 1000;
    static constexpr int DRAW_W         = 60;
    static constexpr int DRAW_H         = 90;
};

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
