///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Tree View Item Class Header
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright 1987-2022 PARMAVEX SERVICES
//
// Licensed under the European Union Public Licence (EUPL), Version 1.1 or -
// as soon they will be approved by the European Commission - subsequent
// versions of the EUPL (the "Licence"). You may not use this work except in
// compliance with the Licence. You may obtain a copy of the Licence at:
//
// http://ec.europa.eu/idabc/eupl
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Licence is distributed on an "AS IS" basis,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the Licence for the specific language governing permissions and
// limitations under the Licence. This source code is free software. It
// must not be sold, leased, rented, sub-licensed or used for any form of
// monetary recompense whatsoever. This notice must not be removed or altered
// from this source distribution.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PXSBASE_TREE_VIEW_ITEM_H_
#define PXSBASE_TREE_VIEW_ITEM_H_

///////////////////////////////////////////////////////////////////////////////////////////////////
// Remarks
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////

// 1. Own Interface
#include "PxsBase/Header Files/PxsBase.h"

// 2. C System Files

// 3. C++ System Files

// 4. Other Libraries

// 5. This Project
#include "PxsBase/Header Files/StringT.h"

// 6. Forwards

///////////////////////////////////////////////////////////////////////////////////////////////////
// Interface
///////////////////////////////////////////////////////////////////////////////////////////////////

class TreeViewItem
{
    public:
        // Default constructor
        TreeViewItem();

        // Copy constructor
        TreeViewItem( const TreeViewItem& oTreeViewItem );

        // Destructor
        ~TreeViewItem();

        // Operators
        TreeViewItem& operator= ( const TreeViewItem& oTreeViewItem );

        // Methods
        HBITMAP GetBitmap() const;
        WORD    GetDepth() const;
        bool    GetIsExpanded() const;
        bool    GetIsNode() const;
        bool    GetIsSelected() const;
        LPCWSTR GetLabel() const;
        LPCWSTR GetStringData() const;
        void    Reset();
        void    SetBitmap( LPCWSTR pszBitmapName, COLORREF transparent, COLORREF background );
        void    SetExpanded( bool isExpanded );
        void    SetDepth( WORD depth );
        void    SetIsNode( bool isNode );
        void    SetIsSelected( bool isSelected );
        void    SetLabel( const String& Label );
        void    SetStringData( const String& StringData );

    protected:
        // Methods

        // Data members

    private:
        // Methods

        // Data members
        bool     m_isExpanded;
        bool     m_isNode;
        bool     m_isSelected;
        WORD     m_depth;
        HBITMAP  m_hBitmap;
        String   m_Label;
        String   m_StringData;
};

#endif  // PXSBASE_TREE_VIEW_ITEM_H_
