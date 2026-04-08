/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <flamespritecache.hxx>
#include <vcl/graphicfilter.hxx>
#include <vcl/animate/Animation.hxx>
#include <vcl/animate/AnimationFrame.hxx>
#include <tools/stream.hxx>
#include <cstdio>

FlameSpriteCache& FlameSpriteCache::Get()
{
    static FlameSpriteCache s;
    return s;
}

FlameSpriteCache::FlameSpriteCache()
{
    Load();
}

void FlameSpriteCache::Load()
{
    OUString aPath("/media/yusuf/Data/librePower/lo-core/instdir/program/fire-65.gif");
    SvFileStream aStream(aPath, StreamMode::READ);
    if (!aStream.good())
    {
        std::fprintf(stderr, "[FlameSpriteCache] Cannot open %s\n",
                     OUStringToOString(aPath, RTL_TEXTENCODING_UTF8).getStr());
        return;
    }

    GraphicFilter& rFilter = GraphicFilter::GetGraphicFilter();
    Graphic aGfx;
    if (rFilter.ImportGraphic(aGfx, aPath, aStream) != ERRCODE_NONE)
    {
        std::fprintf(stderr, "[FlameSpriteCache] Import failed\n");
        return;
    }

    if (!aGfx.IsAnimated())
    {
        std::fprintf(stderr, "[FlameSpriteCache] Not animated\n");
        return;
    }

    Animation aAnim = aGfx.GetAnimation();
    for (size_t i = 0; i < aAnim.Count(); ++i)
    {
        const AnimationFrame& rFrame = aAnim.Get(i);
        m_aFrames.push_back(Graphic(rFrame.maBitmap));
    }

    std::fprintf(stderr, "[FlameSpriteCache] Loaded %zu GIF frames\n", m_aFrames.size());
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
