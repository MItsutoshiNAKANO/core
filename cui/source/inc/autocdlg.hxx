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
#ifndef INCLUDED_CUI_SOURCE_INC_AUTOCDLG_HXX
#define INCLUDED_CUI_SOURCE_INC_AUTOCDLG_HXX

#include <sfx2/tabdlg.hxx>
#include <svtools/langtab.hxx>
#include <svtools/simptabl.hxx>
#include <svtools/svtabbx.hxx>
#include <svtools/treelistentry.hxx>
#include <svx/checklbx.hxx>
#include <svx/langbox.hxx>
#include <vcl/button.hxx>
#include <vcl/fixed.hxx>
#include <vcl/field.hxx>
#include <vcl/group.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/metric.hxx>

class CharClass;
class CollatorWrapper;
class SmartTagMgr;

namespace editeng { class SortedAutoCompleteStrings; }

// class OfaAutoCorrDlg --------------------------------------------------

class OfaAutoCorrDlg : public SfxTabDialog
{
    VclPtr<VclContainer> m_pLanguageBox;
    VclPtr<SvxLanguageBox>  m_pLanguageLB;

    sal_uInt16 m_nReplacePageId;
    sal_uInt16 m_nExceptionsPageId;

    DECL_LINK(SelectLanguageHdl, ListBox*);
public:

    OfaAutoCorrDlg(vcl::Window* pParent, const SfxItemSet *pSet);
    virtual ~OfaAutoCorrDlg();
    virtual void dispose() SAL_OVERRIDE;

    void EnableLanguage(bool bEnable);
};

// class OfaACorrCheckListBox ------------------------------------------

class OfaACorrCheckListBox : public SvSimpleTable
{
    using SvSimpleTable::SetTabs;
    using SvTreeListBox::GetCheckButtonState;
    using SvTreeListBox::SetCheckButtonState;

    protected:
        virtual void    SetTabs() SAL_OVERRIDE;
        virtual void    HBarClick() SAL_OVERRIDE;
        virtual void    KeyInput( const KeyEvent& rKEvt ) SAL_OVERRIDE;

    public:
        OfaACorrCheckListBox(SvSimpleTableContainer& rParent, WinBits nBits = WB_BORDER)
            : SvSimpleTable(rParent, nBits)
        {
        }

        inline void *GetUserData(sal_uLong nPos) { return GetEntry(nPos)->GetUserData(); }
        inline void SetUserData(sal_uLong nPos, void *pData ) { GetEntry(nPos)->SetUserData(pData); }
        inline sal_uLong GetSelectEntryPos() { return GetModel()->GetAbsPos(FirstSelected()); }

        bool            IsChecked(sal_uLong nPos, sal_uInt16 nCol = 0);
        void            CheckEntryPos(sal_uLong nPos, sal_uInt16 nCol, bool bChecked);
        static SvButtonState GetCheckButtonState( SvTreeListEntry*, sal_uInt16 nCol );
        void            SetCheckButtonState( SvTreeListEntry*, sal_uInt16 nCol, SvButtonState );
};

// class OfaAutocorrOptionsPage ------------------------------------------


class OfaAutocorrOptionsPage : public SfxTabPage
{
    using TabPage::ActivatePage;

private:
    VclPtr<SvxCheckListBox> m_pCheckLB;

    OUString m_sInput;
    OUString m_sDoubleCaps;
    OUString m_sStartCap;
    OUString m_sBoldUnderline;
    OUString m_sURL;
    OUString m_sNoDblSpaces;
    OUString m_sDash;
    OUString m_sAccidentalCaps;

public:
    OfaAutocorrOptionsPage(vcl::Window* pParent, const SfxItemSet& rSet);
    virtual ~OfaAutocorrOptionsPage();
    virtual void dispose() SAL_OVERRIDE;

    static VclPtr<SfxTabPage>  Create( vcl::Window* pParent,
                                const SfxItemSet* rAttrSet);

    virtual bool        FillItemSet( SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual void        Reset( const SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual void        ActivatePage( const SfxItemSet& ) SAL_OVERRIDE;

};

// class OfaSwAutoFmtOptionsPage ----------------------------------------------------

class OfaSwAutoFmtOptionsPage : public SfxTabPage
{
    using TabPage::ActivatePage;

    VclPtr<OfaACorrCheckListBox>   m_pCheckLB;
    VclPtr<PushButton>     m_pEditPB;

    OUString        sDeleteEmptyPara;
    OUString        sUseReplaceTbl;
    OUString        sCapitalStartWord;
    OUString        sCapitalStartSentence;
    OUString        sUserStyle;
    OUString        sBullet;
    OUString        sBoldUnder;
    OUString        sNoDblSpaces;
    OUString        sCorrectCapsLock;
    OUString        sDetectURL;
    OUString        sDash;
    OUString        sRightMargin;
    OUString        sNum;
    OUString        sBorder;
    OUString        sTable;
    OUString        sReplaceTemplates;
    OUString        sDelSpaceAtSttEnd;
    OUString        sDelSpaceBetweenLines;

    OUString        sMargin;
    OUString        sBulletChar;
    OUString        sByInputBulletChar;

    vcl::Font        aBulletFont;
    vcl::Font        aByInputBulletFont;
    sal_uInt16       nPercent;

    SvLBoxButtonData*   pCheckButtonData;

        DECL_LINK(SelectHdl, OfaACorrCheckListBox*);
        DECL_LINK(EditHdl, void *);
        SvTreeListEntry* CreateEntry(OUString& rTxt, sal_uInt16 nCol);


        OfaSwAutoFmtOptionsPage( vcl::Window* pParent,
                            const SfxItemSet& rSet );
        virtual ~OfaSwAutoFmtOptionsPage();
        virtual void dispose() SAL_OVERRIDE;

public:
        static VclPtr<SfxTabPage>  Create( vcl::Window* pParent,
                                const SfxItemSet* rAttrSet);
        virtual bool        FillItemSet( SfxItemSet* rSet ) SAL_OVERRIDE;
        virtual void        Reset( const SfxItemSet* rSet ) SAL_OVERRIDE;
        virtual void        ActivatePage( const SfxItemSet& ) SAL_OVERRIDE;
};

// class AutoCorrEdit ----------------------------------------------------

class AutoCorrEdit : public Edit
{
    Link<>  aActionLink;
    bool    bSpaces;

    public:
                    AutoCorrEdit(vcl::Window* pParent, const ResId& rResId) :
                        Edit(pParent, rResId), bSpaces(false){}
                    AutoCorrEdit(vcl::Window* pParent) :
                        Edit(pParent), bSpaces(false){}

    void            SetActionHdl( const Link<>& rLink )
                                { aActionLink = rLink;}

    void            SetSpaces(bool bSet)
                                {bSpaces = bSet;}

    virtual void    KeyInput( const KeyEvent& rKEvent ) SAL_OVERRIDE;
};

// class OfaAutocorrReplacePage ------------------------------------------


struct DoubleString
{
    OUString  sShort;
    OUString  sLong;
    void*   pUserData; ///< CheckBox -> form. Text Bool -> selection text
};

typedef std::vector<DoubleString> DoubleStringArray;
typedef std::map<LanguageType, DoubleStringArray> DoubleStringTable;

struct StringChangeList
{
    DoubleStringArray aNewEntries;
    DoubleStringArray aDeletedEntries;
};

typedef std::map<LanguageType, StringChangeList> StringChangeTable;

class OfaAutocorrReplacePage : public SfxTabPage
{
        using TabPage::ActivatePage;
        using TabPage::DeactivatePage;

private:

        StringChangeTable aChangesTable;

        VclPtr<CheckBox>       m_pTextOnlyCB;
        VclPtr<AutoCorrEdit>   m_pShortED;
        VclPtr<AutoCorrEdit>   m_pReplaceED;
        VclPtr<SvTabListBox>   m_pReplaceTLB;
        VclPtr<PushButton>     m_pNewReplacePB;
        VclPtr<PushButton>     m_pDeleteReplacePB;

        OUString        sModify;
        OUString        sNew;

        std::set<OUString> aFormatText;
        DoubleStringTable       aDoubleStringTable;
        CollatorWrapper*        pCompareClass;
        CharClass*              pCharClass;
        LanguageType            eLang;

        bool bHasSelectionText;
        bool bFirstSelect:1;
        bool bReplaceEditChanged:1;
        bool bSWriter:1;

        DECL_LINK(SelectHdl, SvTabListBox*);
        DECL_LINK(NewDelHdl, PushButton*);
        DECL_LINK(ModifyHdl, Edit*);

        void RefillReplaceBox(  bool bFromReset,
                                LanguageType eOldLanguage,
                                LanguageType eNewLanguage);

        void setTabs();
public:
                        OfaAutocorrReplacePage( vcl::Window* pParent, const SfxItemSet& rSet );
                        virtual ~OfaAutocorrReplacePage();
    virtual void        dispose() SAL_OVERRIDE;

    static VclPtr<SfxTabPage>  Create( vcl::Window* pParent, const SfxItemSet* rAttrSet);

    virtual bool        FillItemSet( SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual void        Reset( const SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual void        ActivatePage( const SfxItemSet& ) SAL_OVERRIDE;
    virtual sfxpg       DeactivatePage( SfxItemSet* pSet = 0 ) SAL_OVERRIDE;
    virtual void        Resize() SAL_OVERRIDE;
    virtual void        StateChanged(StateChangedType nStateChange) SAL_OVERRIDE;

    void    SetLanguage(LanguageType eSet);
    void    DeleteEntry(const OUString& sShort, const OUString& sLong);
    void    NewEntry(const OUString& sShort, const OUString& sLong, bool bKeepSourceFormatting);
};

// class OfaAutocorrExceptPage ---------------------------------------------

struct StringsArrays
{
    std::vector<OUString> aAbbrevStrings;
    std::vector<OUString> aDoubleCapsStrings;

    StringsArrays() { }
};
typedef std::map<LanguageType, StringsArrays> StringsTable;

class OfaAutocorrExceptPage : public SfxTabPage
{
    using TabPage::ActivatePage;
    using TabPage::DeactivatePage;

private:
    VclPtr<AutoCorrEdit>   m_pAbbrevED;
    VclPtr<ListBox>        m_pAbbrevLB;
    VclPtr<PushButton>     m_pNewAbbrevPB;
    VclPtr<PushButton>     m_pDelAbbrevPB;
    VclPtr<CheckBox>       m_pAutoAbbrevCB;

    VclPtr<AutoCorrEdit>   m_pDoubleCapsED;
    VclPtr<ListBox>        m_pDoubleCapsLB;
    VclPtr<PushButton>     m_pNewDoublePB;
    VclPtr<PushButton>     m_pDelDoublePB;
    VclPtr<CheckBox>       m_pAutoCapsCB;

    StringsTable    aStringsTable;
    CollatorWrapper* pCompareClass;
    LanguageType    eLang;

    DECL_LINK(NewDelHdl, void*);
    DECL_LINK(SelectHdl, ListBox*);
    DECL_LINK(ModifyHdl, Edit*);
                    /// Box filled with new language
    void            RefillReplaceBoxes(bool bFromReset,
                                        LanguageType eOldLanguage,
                                        LanguageType eNewLanguage);
public:
                        OfaAutocorrExceptPage( vcl::Window* pParent, const SfxItemSet& rSet );
                        virtual ~OfaAutocorrExceptPage();
    virtual void        dispose() SAL_OVERRIDE;

    static VclPtr<SfxTabPage>  Create( vcl::Window* pParent,
                                const SfxItemSet* rAttrSet);

    virtual bool        FillItemSet( SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual void        Reset( const SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual void        ActivatePage( const SfxItemSet& ) SAL_OVERRIDE;
    virtual sfxpg       DeactivatePage( SfxItemSet* pSet = 0 ) SAL_OVERRIDE;
    void                SetLanguage(LanguageType eSet);

};

// class OfaQuoteTabPage -------------------------------------------------

class OfaQuoteTabPage : public SfxTabPage
{
    using TabPage::ActivatePage;

private:
    /// For anything but writer
    VclPtr<SvxCheckListBox> m_pCheckLB;

    /// Just for writer
    VclPtr<OfaACorrCheckListBox>   m_pSwCheckLB;

    OUString        sNonBrkSpace;
    OUString        sOrdinal;

    SvLBoxButtonData*   pCheckButtonData;

    VclPtr<CheckBox>   m_pSingleTypoCB;
    VclPtr<PushButton> m_pSglStartQuotePB;
    VclPtr<FixedText>  m_pSglStartExFT;
    VclPtr<PushButton> m_pSglEndQuotePB;
    VclPtr<FixedText>  m_pSglEndExFT;
    VclPtr<PushButton> m_pSglStandardPB;

    VclPtr<CheckBox>   m_pDoubleTypoCB;
    VclPtr<PushButton> m_pDblStartQuotePB;
    VclPtr<FixedText>  m_pDblStartExFT;
    VclPtr<PushButton> m_pDblEndQuotePB;
    VclPtr<FixedText>  m_pDblEndExFT;
    VclPtr<PushButton> m_pDblStandardPB;

    OUString    m_sStartQuoteDlg;
    OUString    m_sEndQuoteDlg;

    OUString    m_sStandard;


    sal_UCS4    cSglStartQuote;
    sal_UCS4    cSglEndQuote;

    sal_UCS4    cStartQuote;
    sal_UCS4    cEndQuote;

    DECL_LINK( QuoteHdl, PushButton* );
    DECL_LINK( StdQuoteHdl, PushButton* );

    OUString            ChangeStringExt_Impl( sal_UCS4 );

    SvTreeListEntry* CreateEntry(OUString& rTxt, sal_uInt16 nCol);

                        OfaQuoteTabPage( vcl::Window* pParent, const SfxItemSet& rSet );
public:
                        virtual ~OfaQuoteTabPage();
    virtual void        dispose() SAL_OVERRIDE;

    static VclPtr<SfxTabPage>  Create( vcl::Window* pParent,
                                const SfxItemSet* rAttrSet);

    virtual bool        FillItemSet( SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual void        Reset( const SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual void        ActivatePage( const SfxItemSet& ) SAL_OVERRIDE;
};

// class OfaAutoCompleteTabPage ---------------------------------------------

class OfaAutoCompleteTabPage : public SfxTabPage
{
public:
    class AutoCompleteMultiListBox : public MultiListBox
    {
        VclPtr<OfaAutoCompleteTabPage> m_pPage;
    public:
        AutoCompleteMultiListBox(vcl::Window *pParent, WinBits nBits)
            : MultiListBox(pParent, nBits)
            , m_pPage(NULL)
        {
        }
        virtual ~AutoCompleteMultiListBox();
        virtual void dispose() SAL_OVERRIDE;
        void SetPage(OfaAutoCompleteTabPage *pPage) { m_pPage = pPage; }
        virtual bool PreNotify( NotifyEvent& rNEvt ) SAL_OVERRIDE;
    };

private:
    using TabPage::ActivatePage;
    VclPtr<CheckBox>       m_pCBActiv; ///<Enable word completion
    VclPtr<CheckBox>       m_pCBAppendSpace;///<Append space
    VclPtr<CheckBox>       m_pCBAsTip; ///<Show as tip

    VclPtr<CheckBox>       m_pCBCollect;///<Collect words
    VclPtr<CheckBox>       m_pCBRemoveList;///<...save the list for later use...

    VclPtr<ListBox>        m_pDCBExpandKey;
    VclPtr<NumericField>   m_pNFMinWordlen;
    VclPtr<NumericField>   m_pNFMaxEntries;
    VclPtr<AutoCompleteMultiListBox> m_pLBEntries;
    VclPtr<PushButton>     m_pPBEntries;
    editeng::SortedAutoCompleteStrings* m_pAutoCompleteList;
    sal_uInt16      m_nAutoCmpltListCnt;

    DECL_LINK( CheckHdl, CheckBox* );

                        OfaAutoCompleteTabPage( vcl::Window* pParent,
                                                const SfxItemSet& rSet );
public:
    virtual ~OfaAutoCompleteTabPage();
    virtual void dispose() SAL_OVERRIDE;
    static VclPtr<SfxTabPage>  Create( vcl::Window* pParent,
                                const SfxItemSet* rAttrSet);

    virtual bool        FillItemSet( SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual void        Reset( const SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual void        ActivatePage( const SfxItemSet& ) SAL_OVERRIDE;

    void CopyToClipboard() const;
    DECL_LINK(DeleteHdl, void *);
};

// class OfaSmartTagOptionsTabPage ---------------------------------------------

/** Smart tag options tab page

    This tab page is used to enable/disable smart tag types
*/
class OfaSmartTagOptionsTabPage : public SfxTabPage
{
    using TabPage::ActivatePage;

private:

    // controls
    VclPtr<CheckBox>               m_pMainCB;
    VclPtr<SvxCheckListBox>        m_pSmartTagTypesLB;
    VclPtr<PushButton>             m_pPropertiesPB;

    /** Inserts items into m_aSmartTagTypesLB

        Reads out the smart tag types supported by the SmartTagMgr and
        inserts the associated strings into the list box.
    */
    void FillListBox( const SmartTagMgr& rSmartTagMgr );

    /** Clears the m_aSmartTagTypesLB
    */
    void ClearListBox();

    /** Handler for the check box

        Enables/disables all controls in the tab page (except from the
        check box.
    */
    DECL_LINK(CheckHdl, void *);

    /** Handler for the push button

        Calls the displayPropertyPage function of the smart tag recognizer
        associated with the currently selected smart tag type.
    */
    DECL_LINK(ClickHdl, void *);

    /** Handler for the list box

        Enables/disables the properties push button if selection in the
        smart tag types list box changes.
    */
    DECL_LINK(SelectHdl, void *);

public:
    /// construction via Create()
    OfaSmartTagOptionsTabPage( vcl::Window* pParent, const SfxItemSet& rSet );
    virtual ~OfaSmartTagOptionsTabPage();
    virtual void dispose() SAL_OVERRIDE;

    static VclPtr<SfxTabPage>  Create( vcl::Window* pParent, const SfxItemSet* rAttrSet);

    virtual bool        FillItemSet( SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual void        Reset( const SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual void        ActivatePage( const SfxItemSet& ) SAL_OVERRIDE;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
