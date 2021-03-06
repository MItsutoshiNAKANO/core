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
#include "vbafield.hxx"
#include "vbarange.hxx"
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/text/XTextViewCursorSupplier.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include <com/sun/star/text/XTextFieldsSupplier.hpp>
#include <com/sun/star/text/FilenameDisplayFormat.hpp>
#include <com/sun/star/util/XRefreshable.hpp>
#include <com/sun/star/util/XUpdatable.hpp>
#include <comphelper/string.hxx>
#include <ooo/vba/word/WdFieldType.hpp>
#include <swtypes.hxx>
#include <cppuhelper/implbase.hxx>

using namespace ::ooo::vba;
using namespace ::com::sun::star;

SwVbaField::SwVbaField(  const uno::Reference< ooo::vba::XHelperInterface >& rParent, const uno::Reference< uno::XComponentContext >& rContext, const css::uno::Reference< css::text::XTextDocument >& rDocument, const  uno::Reference< css::text::XTextField >& xTextField) throw ( uno::RuntimeException ) : SwVbaField_BASE( rParent, rContext ), mxTextDocument( rDocument )
{
    mxTextField.set( xTextField, uno::UNO_QUERY_THROW );
}

sal_Bool SAL_CALL SwVbaField::Update() throw (uno::RuntimeException, std::exception)
{
    uno::Reference< util::XUpdatable > xUpdatable( mxTextField, uno::UNO_QUERY );
    if( xUpdatable.is() )
    {
        xUpdatable->update();
        return sal_True;
    }
    return sal_False;
}

// XHelperInterface
OUString
SwVbaField::getServiceImplName()
{
    return OUString("SwVbaField");
}

uno::Sequence<OUString>
SwVbaField::getServiceNames()
{
    static uno::Sequence< OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = "ooo.vba.word.Field";
    }
    return aServiceNames;
}

// FIXME? copy and paste code
// the codes are copied from ww8par5.cxx
class SwVbaReadFieldParams
{
private:
    OUString aData;
    sal_Int32 nLen, nFnd, nNext, nSavPtr;
    OUString aFieldName;
public:
    explicit SwVbaReadFieldParams( const OUString& rData );
    ~SwVbaReadFieldParams();

    long SkipToNextToken();

    sal_Int32 FindNextStringPiece( sal_Int32 _nStart = -1 );

    OUString GetResult() const;
    OUString GetFieldName()const { return aFieldName; }
};

SwVbaReadFieldParams::SwVbaReadFieldParams( const OUString& _rData )
    : aData( _rData ), nLen( _rData.getLength() ), nNext( 0 )
{
    // First search for an opening parenthesis or a space or a quotation mark
    // or a backslash, so that the field command
    // (thus INCLUDEPICTURE or ...) is ignored.
    while( (nLen > nNext) && (aData[ nNext ] == ' ') )
        ++nNext;

    sal_Unicode c;
    while(     nLen > nNext
            && (c = aData[ nNext ]) != ' '
            && c != '"'
            && c != '\\'
            && c != 132
            && c != 0x201c )
        ++nNext;

    nFnd      = nNext;
    nSavPtr   = nNext;
    aFieldName = aData.copy( 0, nFnd );
}

SwVbaReadFieldParams::~SwVbaReadFieldParams()
{
}

OUString SwVbaReadFieldParams::GetResult() const
{
    return    (-1 == nFnd)
            ? OUString()
            : aData.copy( nFnd, (nSavPtr - nFnd) );
}

// ret: -2: NOT a '\' parameter but normal Text
long SwVbaReadFieldParams::SkipToNextToken()
{
    long nRet = -1;     // end
    if (
         (-1 != nNext) && (nLen > nNext) &&
         -1 != (nFnd = FindNextStringPiece(nNext))
       )
    {
        nSavPtr = nNext;

        if ('\\' == aData[nFnd] && '\\' != aData[nFnd + 1])
        {
            nRet = aData[++nFnd];
            nNext = ++nFnd;             // and set behind
        }
        else
        {
            nRet = -2;
            if (
                 (-1 != nSavPtr ) &&
                 (
                   ('"' == aData[nSavPtr - 1]) ||
                   (0x201d == aData[nSavPtr - 1])
                 )
               )
            {
                --nSavPtr;
            }
        }
    }
    return nRet;
}

// FindNextPara is searching for the next Backslash-Parameter or the next string
// until blank or the next "\" or until the closing quotation mark
// or until the string end of pStr.

// Output ppNext (if ppNext != 0) beginning of the search for the next parameter or 0

// Return value: 0 if String-End reached, otherwise begin of the parameter or the string

sal_Int32 SwVbaReadFieldParams::FindNextStringPiece(const sal_Int32 nStart)
{
    sal_Int32  n = ( -1 == nStart ) ? nFnd : nStart;  // Start
    sal_Int32 n2;          // End

    nNext = -1;        // Default for not found

    while( (nLen > n) && (aData[ n ] == ' ') )
        ++n;

    if( nLen == n )
        return -1;     // String End reached!

    if(     (aData[ n ] == '"')     // quotation marks are in front of parenthesis?
        ||  (aData[ n ] == 0x201c)
        ||  (aData[ n ] == 132) )
    {
        n++;                        // ignore quotation marks
        n2 = n;                     // From here search for the end
        while(     (nLen > n2)
                && (aData[ n2 ] != '"')
                && (aData[ n2 ] != 0x201d)
                && (aData[ n2 ] != 147) )
            n2++;                   // Search for the end of the parenthesis
    }
    else                        // no quotation marks
    {
        n2 = n;                     // from here search for the end
        while( (nLen > n2) && (aData[ n2 ] != ' ') ) // Search for the end of the parenthesis
        {
            if( aData[ n2 ] == '\\' )
            {
                if( aData[ n2+1 ] == '\\' )
                    n2 += 2;        // double-backslash -> OK
                else
                {
                    if( n2 > n )
                        n2--;
                    break;          // single-backslash -> End
                }
            }
            else
                n2++;               // no backslash -> OK
        }
    }
    if( nLen > n2 )
    {
        if(aData[ n2 ] != ' ') n2++;
        nNext = n2;
    }
    return n;
}

// SwVbaFields

static uno::Any lcl_createField( const uno::Reference< XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< frame::XModel >& xModel, const uno::Any& aSource )
{
    uno::Reference< text::XTextField > xTextField( aSource, uno::UNO_QUERY_THROW );
    uno::Reference< text::XTextDocument > xTextDocument( xModel, uno::UNO_QUERY_THROW );
    uno::Reference< word::XField > xField( new SwVbaField( xParent, xContext, xTextDocument, xTextField ) );
    return uno::makeAny( xField );
}

typedef ::cppu::WeakImplHelper< css::container::XEnumeration > FieldEnumeration_BASE;
typedef ::cppu::WeakImplHelper< container::XIndexAccess, container::XEnumerationAccess > FieldCollectionHelper_BASE;

class FieldEnumeration : public FieldEnumeration_BASE
{
    uno::Reference< XHelperInterface > mxParent;
    uno::Reference< uno::XComponentContext > mxContext;
    uno::Reference< frame::XModel > mxModel;
    uno::Reference< container::XEnumeration > mxEnumeration;
public:
    FieldEnumeration(  const uno::Reference< XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext > & xContext, const uno::Reference< frame::XModel >& xModel, const uno::Reference< container::XEnumeration >& xEnumeration ) : mxParent( xParent ), mxContext( xContext ), mxModel( xModel ), mxEnumeration( xEnumeration )
    {
    }
    virtual sal_Bool SAL_CALL hasMoreElements(  ) throw (uno::RuntimeException, std::exception) SAL_OVERRIDE
    {
        return mxEnumeration->hasMoreElements();
    }
    virtual uno::Any SAL_CALL nextElement(  ) throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException, std::exception) SAL_OVERRIDE
    {
        if ( !hasMoreElements() )
            throw container::NoSuchElementException();
        return lcl_createField( mxParent, mxContext, mxModel, mxEnumeration->nextElement() );
    }
};

class FieldCollectionHelper : public FieldCollectionHelper_BASE
{
    uno::Reference< XHelperInterface > mxParent;
    uno::Reference< uno::XComponentContext > mxContext;
    uno::Reference< frame::XModel > mxModel;
    uno::Reference< container::XEnumerationAccess > mxEnumerationAccess;
public:
    FieldCollectionHelper( const uno::Reference< XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< frame::XModel >& xModel ) throw (css::uno::RuntimeException) : mxParent( xParent ), mxContext( xContext ), mxModel( xModel )
    {
        uno::Reference< text::XTextFieldsSupplier > xSupp( xModel, uno::UNO_QUERY_THROW );
        mxEnumerationAccess.set( xSupp->getTextFields(), uno::UNO_QUERY_THROW );
    }
    // XElementAccess
    virtual uno::Type SAL_CALL getElementType(  ) throw (uno::RuntimeException, std::exception) SAL_OVERRIDE { return  mxEnumerationAccess->getElementType(); }
    virtual sal_Bool SAL_CALL hasElements(  ) throw (uno::RuntimeException, std::exception) SAL_OVERRIDE { return mxEnumerationAccess->hasElements(); }
    // XIndexAccess
    virtual ::sal_Int32 SAL_CALL getCount(  ) throw (uno::RuntimeException, std::exception) SAL_OVERRIDE
    {
        uno::Reference< container::XEnumeration > xEnumeration =  mxEnumerationAccess->createEnumeration();
        sal_Int32 nCount = 0;
        while( xEnumeration->hasMoreElements() )
        {
            ++nCount;
            xEnumeration->nextElement();
        }
        return nCount;
    }
    virtual uno::Any SAL_CALL getByIndex( ::sal_Int32 Index ) throw (lang::IndexOutOfBoundsException, lang::WrappedTargetException, uno::RuntimeException, std::exception ) SAL_OVERRIDE
    {
        if( Index < 0 || Index >= getCount() )
            throw lang::IndexOutOfBoundsException();

        uno::Reference< container::XEnumeration > xEnumeration =  mxEnumerationAccess->createEnumeration();
        sal_Int32 nCount = 0;
        while( xEnumeration->hasMoreElements() )
        {
            if( nCount == Index )
            {
                return xEnumeration->nextElement();
            }
            ++nCount;
        }
        throw lang::IndexOutOfBoundsException();
    }
    // XEnumerationAccess
    virtual uno::Reference< container::XEnumeration > SAL_CALL createEnumeration(  ) throw (uno::RuntimeException, std::exception) SAL_OVERRIDE
    {
        uno::Reference< container::XEnumeration > xEnumeration =  mxEnumerationAccess->createEnumeration();
        return uno::Reference< container::XEnumeration >( new FieldEnumeration( mxParent, mxContext, mxModel, xEnumeration ) );
    }
};

SwVbaFields::SwVbaFields( const uno::Reference< XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext > & xContext, const uno::Reference< frame::XModel >& xModel ) : SwVbaFields_BASE( xParent, xContext , uno::Reference< container::XIndexAccess >( new FieldCollectionHelper( xParent, xContext, xModel ) ) ), mxModel( xModel )
{
    mxMSF.set( mxModel, uno::UNO_QUERY_THROW );
}

uno::Reference< word::XField > SAL_CALL
SwVbaFields::Add( const css::uno::Reference< ::ooo::vba::word::XRange >& Range, const css::uno::Any& Type, const css::uno::Any& Text, const css::uno::Any& /*PreserveFormatting*/ ) throw (css::script::BasicErrorException, css::uno::RuntimeException, std::exception)
{
    sal_Int32 nType = word::WdFieldType::wdFieldEmpty;
    Type >>= nType;
    OUString sText;
    Text >>= sText;

    OUString sFieldName;
    if( ( nType == word::WdFieldType::wdFieldEmpty ) && !sText.isEmpty() )
    {
        SwVbaReadFieldParams aReadParam(sText);
        sFieldName = aReadParam.GetFieldName();
        OSL_TRACE("SwVbaFields::Add, the field name is %s ",OUStringToOString( sFieldName, RTL_TEXTENCODING_UTF8 ).getStr() );
    }

    uno::Reference< text::XTextContent > xTextField;
    if( nType == word::WdFieldType::wdFieldFileName || sFieldName.equalsIgnoreAsciiCase("FILENAME") )
    {
        xTextField.set( Create_Field_FileName( sText ), uno::UNO_QUERY_THROW );
    }
    else if( nType == word::WdFieldType::wdFieldDocProperty || sFieldName.equalsIgnoreAsciiCase("DOCPROPERTY") )
    {
        xTextField.set( Create_Field_DocProperty( sText ), uno::UNO_QUERY_THROW );
    }
    else
    {
        throw uno::RuntimeException("Not implemented" );
    }

    SwVbaRange& rVbaRange = dynamic_cast<SwVbaRange&>(*Range.get());
    uno::Reference< text::XTextRange > xTextRange = rVbaRange.getXTextRange();
    uno::Reference< text::XText > xText = xTextRange->getText();
    xText->insertTextContent( xTextRange, xTextField, true );
    return uno::Reference< word::XField >( new SwVbaField( mxParent, mxContext, uno::Reference< text::XTextDocument >( mxModel, uno::UNO_QUERY_THROW ), uno::Reference< text::XTextField >( xTextField, uno::UNO_QUERY_THROW ) ) );
}

uno::Reference< text::XTextField > SwVbaFields::Create_Field_FileName( const OUString& _text )
    throw (uno::RuntimeException, script::BasicErrorException)
{
    uno::Reference< text::XTextField > xTextField( mxMSF->createInstance("com.sun.star.text.TextField.FileName"), uno::UNO_QUERY_THROW );
    sal_Int16 nFileFormat = text::FilenameDisplayFormat::NAME_AND_EXT;
    if( !_text.isEmpty() )
    {
        long nRet;
        SwVbaReadFieldParams aReadParam( _text );
        while (-1 != (nRet = aReadParam.SkipToNextToken()))
        {
            switch (nRet)
            {
                case 'p':
                    nFileFormat = text::FilenameDisplayFormat::FULL;
                    break;
                case '*':
                    //Skip over MERGEFORMAT
                    aReadParam.SkipToNextToken();
                    break;
                default:
                    DebugHelper::basicexception(SbERR_BAD_ARGUMENT, OUString());
                    break;
            }
        }
    }

    uno::Reference< beans::XPropertySet > xProps( xTextField, uno::UNO_QUERY_THROW );
    xProps->setPropertyValue("FileFormat", uno::makeAny( nFileFormat ) );

    return xTextField;
}

struct DocPropertyTable
{
    const char* sDocPropertyName;
    const char* sFieldService;
};

static const DocPropertyTable aDocPropertyTables[] =
{
    { "Author", "com.sun.star.text.textfield.docinfo.CreateAuthor" },
    { "Bytes", NULL },
    { "Category", NULL },
    { "Characters",NULL },
    { "CharactersWithSpaces", NULL },
    { "Comments", "com.sun.star.text.textfield.docinfo.Description" },
    { "Company", NULL },
    { "CreateTime", "com.sun.star.text.textfield.docinfo.CreateDateTime" },
    { "HyperlinkBase", NULL },
    { "Keywords", "com.sun.star.text.textfield.docinfo.Keywords" },
    { "LastPrinted", "com.sun.star.text.textfield.docinfo.PrintDateTime" },
    { "LastSavedBy", "com.sun.star.text.textfield.docinfo.ChangeAuthor" },
    { "LastSavedTime", "com.sun.star.text.textfield.docinfo.ChangeDateTime" },
    { "Lines", NULL },
    { "Manager", NULL },
    { "NameofApplication", NULL },
    { "ODMADocID", NULL },
    { "Pages", "com.sun.star.text.textfield.PageCount" },
    { "Paragraphs", "com.sun.star.text.textfield.ParagraphCount" },
    { "RevisionNumber", "com.sun.star.text.textfield.docinfo.Revision" },
    { "Security", NULL },
    { "Subject", "com.sun.star.text.textfield.docinfo.Subject" },
    { "Template", "com.sun.star.text.textfield.TemplateName" },
    { "Title", "com.sun.star.text.textfield.docinfo.Title" },
    { "TotalEditingTime", "com.sun.star.text.textfield.docinfo.EditTime" },
    { "Words", "com.sun.star.text.textfield.WordCount" },
    { NULL, NULL }
};

uno::Reference< text::XTextField > SwVbaFields::Create_Field_DocProperty( const OUString& _text ) throw (uno::RuntimeException)
{
    OUString aDocProperty;
    SwVbaReadFieldParams aReadParam( _text );
    long nRet;
    while( -1 != ( nRet = aReadParam.SkipToNextToken() ))
    {
        switch( nRet )
        {
            case -2:
                if( aDocProperty.isEmpty() )
                    aDocProperty = aReadParam.GetResult();
                break;
            case '*':
                //Skip over MERGEFORMAT
                aReadParam.SkipToNextToken();
                break;
        }
    }
    aDocProperty = comphelper::string::remove(aDocProperty, '"');
    OSL_TRACE("SwVbaFields::Create_Field_DocProperty, the document property name is %s ",OUStringToOString( aDocProperty, RTL_TEXTENCODING_UTF8 ).getStr() );
    if( aDocProperty.isEmpty() )
    {
        throw uno::RuntimeException();
    }

    bool bCustom = true;
    OUString sFieldService;
    // find the build in document properties
    for( const DocPropertyTable* pTable = aDocPropertyTables; pTable->sDocPropertyName != NULL; pTable++ )
    {
        if( aDocProperty.equalsIgnoreAsciiCaseAscii( pTable->sDocPropertyName ) )
        {
            if( pTable->sFieldService != NULL )
                sFieldService = OUString::createFromAscii(pTable->sFieldService);
            bCustom = false;
            break;
        }
    }

    if( bCustom )
    {
        sFieldService = "com.sun.star.text.textfield.docinfo.Custom";
    }
    else if( sFieldService.isEmpty() )
    {
        throw uno::RuntimeException("Not implemented" );
    }

    uno::Reference< text::XTextField > xTextField( mxMSF->createInstance( sFieldService ), uno::UNO_QUERY_THROW );

    if( bCustom )
    {
        uno::Reference< beans::XPropertySet > xProps( xTextField, uno::UNO_QUERY_THROW );
        OUString sDocPropertyName( aDocProperty );
        xProps->setPropertyValue("Name", uno::makeAny( sDocPropertyName ) );
    }

    return xTextField;
}

uno::Reference< container::XEnumeration > SAL_CALL
SwVbaFields::createEnumeration() throw (uno::RuntimeException)
{
    uno::Reference< container::XEnumerationAccess > xEnumerationAccess( m_xIndexAccess, uno::UNO_QUERY_THROW );
    return xEnumerationAccess->createEnumeration();
}

// ScVbaCollectionBaseImpl
uno::Any
SwVbaFields::createCollectionObject( const uno::Any& aSource )
{
    return lcl_createField( mxParent, mxContext, mxModel, aSource );
}

sal_Int32 SAL_CALL SwVbaFields::Update() throw (uno::RuntimeException, std::exception)
{
    sal_Int32 nUpdate = 1;
    try
    {
        uno::Reference< text::XTextFieldsSupplier > xSupp( mxModel, uno::UNO_QUERY_THROW );
        uno::Reference< util::XRefreshable > xRef( xSupp->getTextFields(), uno::UNO_QUERY_THROW );
        xRef->refresh();
        nUpdate = 0;
    }
    catch(const uno::Exception&)
    {
        nUpdate = 1;
    }
    return nUpdate;
}

// XHelperInterface
OUString
SwVbaFields::getServiceImplName()
{
    return OUString("SwVbaFields");
}

// XEnumerationAccess
uno::Type SAL_CALL
SwVbaFields::getElementType() throw (uno::RuntimeException)
{
    return  cppu::UnoType<word::XField>::get();
}

uno::Sequence<OUString>
SwVbaFields::getServiceNames()
{
    static uno::Sequence< OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = "ooo.vba.word.Fields";
    }
    return aServiceNames;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
