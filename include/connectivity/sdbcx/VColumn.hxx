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
#ifndef INCLUDED_CONNECTIVITY_SDBCX_VCOLUMN_HXX
#define INCLUDED_CONNECTIVITY_SDBCX_VCOLUMN_HXX

#include <com/sun/star/sdbcx/XDataDescriptorFactory.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <comphelper/IdPropArrayHelper.hxx>
#include <cppuhelper/compbase2.hxx>
#include <cppuhelper/implbase1.hxx>
#include <connectivity/CommonTools.hxx>
#include <comphelper/broadcasthelper.hxx>
#include <connectivity/sdbcx/VDescriptor.hxx>
#include <connectivity/dbtoolsdllapi.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>

namespace connectivity
{
    namespace sdbcx
    {
        class OColumn;
        typedef ::comphelper::OIdPropertyArrayUsageHelper<OColumn> OColumn_PROP;

        typedef ::cppu::WeakComponentImplHelper2< ::com::sun::star::container::XNamed,
                                                  ::com::sun::star::lang::XServiceInfo> OColumnDescriptor_BASE;
        typedef ::cppu::ImplHelper1< ::com::sun::star::sdbcx::XDataDescriptorFactory > OColumn_BASE;


        class OOO_DLLPUBLIC_DBTOOLS OColumn :
                                    public comphelper::OBaseMutex,
                                    public OColumn_BASE,
                                    public OColumnDescriptor_BASE,
                                    public OColumn_PROP,
                                    public ODescriptor
        {
        protected:
            OUString        m_TypeName;
            OUString        m_Description;
            OUString        m_DefaultValue;

            sal_Int32       m_IsNullable;
            sal_Int32       m_Precision;
            sal_Int32       m_Scale;
            sal_Int32       m_Type;

            bool            m_IsAutoIncrement;
            bool            m_IsRowVersion;
            bool            m_IsCurrency;

            OUString       m_CatalogName;
            OUString       m_SchemaName;
            OUString       m_TableName;

            using OColumnDescriptor_BASE::rBHelper;
            virtual ::cppu::IPropertyArrayHelper* createArrayHelper( sal_Int32 _nId) const SAL_OVERRIDE;
            virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper() SAL_OVERRIDE;

            virtual ~OColumn();
        public:
            virtual void    SAL_CALL acquire() throw() SAL_OVERRIDE;
            virtual void    SAL_CALL release() throw() SAL_OVERRIDE;

            OColumn(    bool _bCase);
            OColumn(    const OUString& _Name,
                        const OUString& _TypeName,
                        const OUString& _DefaultValue,
                        const OUString& _Description,
                        sal_Int32       _IsNullable,
                        sal_Int32       _Precision,
                        sal_Int32       _Scale,
                        sal_Int32       _Type,
                        bool            _IsAutoIncrement,
                        bool            _IsRowVersion,
                        bool            _IsCurrency,
                        bool            _bCase,
                        const OUString& _CatalogName,
                        const OUString& _SchemaName,
                        const OUString& _TableName);

            DECLARE_SERVICE_INFO();
            //XInterface
            virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            //XTypeProvider
            virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw(::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            // ODescriptor
            virtual void construct() SAL_OVERRIDE;
            // ::cppu::OComponentHelper
            virtual void SAL_CALL disposing() SAL_OVERRIDE;
            // XPropertySet
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            // XNamed
            virtual OUString SAL_CALL getName(  ) throw(::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            virtual void SAL_CALL setName( const OUString& aName ) throw(::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
            // XDataDescriptorFactory
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL createDataDescriptor(  ) throw(::com::sun::star::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        };
    }
}

#endif // INCLUDED_CONNECTIVITY_SDBCX_VCOLUMN_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
