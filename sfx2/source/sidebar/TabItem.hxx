/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */
#ifndef INCLUDED_SFX2_SOURCE_SIDEBAR_TABITEM_HXX
#define INCLUDED_SFX2_SOURCE_SIDEBAR_TABITEM_HXX

#include <vcl/button.hxx>

#include "DeckDescriptor.hxx"

namespace vcl { class Window; }

namespace sfx2 { namespace sidebar {

/** A single button in the tab bar.
*/
class TabItem
    : public ImageRadioButton
{
public:
    TabItem (vcl::Window* pParentWindow);

    virtual void Paint (vcl::RenderContext& rRenderContext, const Rectangle& rUpdateArea) SAL_OVERRIDE;
    virtual void MouseMove (const MouseEvent& rEvent) SAL_OVERRIDE;
    virtual void MouseButtonDown (const MouseEvent& rMouseEvent) SAL_OVERRIDE;
    virtual void MouseButtonUp (const MouseEvent& rMouseEvent) SAL_OVERRIDE;

private:
    bool mbIsLeftButtonDown;
    enum PaintType {
        PT_Native,
        PT_Theme
    } mePaintType;
};

} } // end of namespace sfx2::sidebar

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
