/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef INCLUDED_SW_SOURCE_UIBASE_INC_NAVMGR_HXX
#define INCLUDED_SW_SOURCE_UIBASE_INC_NAVMGR_HXX

#include <vector>
#include <memory>

#include "swtypes.hxx"
#include "calbck.hxx"
#include "unocrsr.hxx"

class   SwWrtShell;
struct  SwPosition;
class SwUnoCrsr;

class SwNavigationMgr : SwClient
{
private:
    /*
     * List of entries in the navigation history
     * Entries are SwUnoCrsr because thos gets corrected automatically
     * when nodes are deleted.
     *
     * The navigation history behaves as a stack, to which items are added when we jump to a new position
     * (e.g. click a link, or double click an entry from the navigator).
     * Every use of the back/forward buttons results in moving the stack pointer within the navigation history
     */
    typedef ::std::vector< std::shared_ptr<SwUnoCrsr> > Stack_t;
    Stack_t m_entries;
    Stack_t::size_type m_nCurrent; /* Current position within the navigation history */
    SwWrtShell & m_rMyShell; /* The active shell within which the navigation occurs */

    void GotoSwPosition(const SwPosition &rPos);

public:
    /* Constructor that initializes the shell to the current shell */
    SwNavigationMgr( SwWrtShell & rShell );
    virtual ~SwNavigationMgr()
    {
        for(auto pEntry : m_entries)
        {
            if(pEntry && GetRegisteredIn() == pEntry.get())
                pEntry->Remove(this);
        }
    }
    /* Can we go back in the history ? */
    bool backEnabled() ;
    /* Can we go forward in the history ? */
    bool forwardEnabled();
    /* The method that is called when we click the back button */
    void goBack() ;
    /* The method that is called when we click the forward button */
    void goForward() ;
    /* The method that adds the position pPos to the navigation history */
    bool addEntry(const SwPosition& rPos);
    void SwClientNotify(const SwModify& rModify, const SfxHint& rHint) SAL_OVERRIDE;
};
#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
