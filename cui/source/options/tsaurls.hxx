/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_CUI_SOURCE_OPTIONS_TSAURLS_HXX
#define INCLUDED_CUI_SOURCE_OPTIONS_TSAURLS_HXX

#include <vcl/lstbox.hxx>
#include <vcl/button.hxx>

class TSAURLsDialog : public ModalDialog
{
private:
    VclPtr<ListBox>    m_pURLListBox;
    VclPtr<PushButton> m_pAddBtn;
    VclPtr<PushButton> m_pDeleteBtn;
    VclPtr<OKButton>   m_pOKBtn;

    DECL_LINK(AddHdl_Impl, void *);
    DECL_LINK(DeleteHdl_Impl, void *);
    DECL_LINK(OKHdl_Impl, void *);

    std::set<OUString> m_aURLs;

    void AddTSAURL(const OUString &rURL);

public:
    TSAURLsDialog(vcl::Window* pParent);
    virtual ~TSAURLsDialog();
    virtual void dispose() SAL_OVERRIDE;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
