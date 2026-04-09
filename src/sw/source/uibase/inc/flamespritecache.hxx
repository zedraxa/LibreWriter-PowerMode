/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef INCLUDED_SW_SOURCE_UIBASE_INC_FLAMESPRITECACHE_HXX
#define INCLUDED_SW_SOURCE_UIBASE_INC_FLAMESPRITECACHE_HXX

#include <vcl/graph.hxx>
#include <vector>

/// Singleton: loads GIF fire animation and caches decoded frames as Graphic objects.
class FlameSpriteCache final
{
public:
    static FlameSpriteCache& Get();

    bool   IsLoaded()   const { return !m_aFrames.empty(); }
    size_t FrameCount() const { return m_aFrames.size(); }
    const Graphic& GetFrame(size_t nIdx) const { return m_aFrames[nIdx % m_aFrames.size()]; }

private:
    FlameSpriteCache();
    void Load();

    std::vector<Graphic> m_aFrames;
};

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
