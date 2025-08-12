///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Tree View Item Class Implementation
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

///////////////////////////////////////////////////////////////////////////////////////////////////
// Remarks
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////

// 1. Own Interface
#include "PxsBase/Header Files/TreeViewItem.h"

// 2. C System Files

// 3. C++ System Files

// 4. Other Libraries

// 5. This Project
#include "PxsBase/Header Files/Exception.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////////////////////////

// Default constructor. Keep values in sync with the Reset method
TreeViewItem::TreeViewItem()
             :m_isExpanded( false ),
              m_isNode( false ),
              m_isSelected( false ),
              m_depth( 0 ),
              m_hBitmap( nullptr ),
              m_Label(),
              m_StringData()
{
}

// Copy constructor
TreeViewItem::TreeViewItem( const TreeViewItem& oTreeViewItem )
             :TreeViewItem()
{
    *this = oTreeViewItem;
}

// Destructor
TreeViewItem::~TreeViewItem()
{
    // Clean up
    try
    {
        Reset();
    }
    catch ( const Exception& e )
    {
        PXSLogException( e, __FUNCTION__ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Operators
///////////////////////////////////////////////////////////////////////////////////////////////////


// Assignment operator
TreeViewItem& TreeViewItem::operator= ( const TreeViewItem& oTreeViewItem )
{
    if ( this == &oTreeViewItem ) return *this;

    m_isExpanded = oTreeViewItem.m_isExpanded;
    m_isNode     = oTreeViewItem.m_isNode;
    m_isSelected = oTreeViewItem.m_isSelected;
    m_depth      = oTreeViewItem.m_depth;
    m_Label      = oTreeViewItem.m_Label;
    m_StringData = oTreeViewItem.m_StringData;

    // Free any existing bitmap then copy
    if ( m_hBitmap )
    {
        DeleteObject( m_hBitmap );
        m_hBitmap = nullptr;
    }

    if ( oTreeViewItem.m_hBitmap )
    {
       m_hBitmap = (HBITMAP)CopyImage( oTreeViewItem.m_hBitmap, IMAGE_BITMAP, 0, 0, 0 );
    }

    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Public Methods
///////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================================================//
//  Description:
//      Get the bitmap for this tree view item
//
//  Parameters:
//      None
//
//  Returns:
//      Handle to the bitmap
//===============================================================================================//
HBITMAP TreeViewItem::GetBitmap() const
{
    return m_hBitmap;
}

//===============================================================================================//
//  Description:
//      Get the indent level for this tree view item
//
//  Parameters:
//      None
//
//  Returns:
//      WORD representing the depth level
//===============================================================================================//
WORD TreeViewItem::GetDepth() const
{
    return m_depth;
}

//===============================================================================================//
//  Description:
//      Get if the this tree view item is expanded, applies to nodes only.
//
//  Parameters:
//      None
//
//  Returns:
//      true if node expanded, else false
//===============================================================================================//
bool TreeViewItem::GetIsExpanded() const
{
    return m_isExpanded;
}

//===============================================================================================//
//  Description:
//      Get if the this tree view item is a node.
//
//  Parameters:
//      None
//
//  Returns:
//      true if this item is a node, else it is a leaf
//===============================================================================================//
bool TreeViewItem::GetIsNode() const
{
    return m_isNode;
}

//===============================================================================================//
//  Description:
//      Get if the this tree view item is selected.
//
//  Parameters:
//      None
//
//  Returns:
//      true if this item is selected
//===============================================================================================//
bool TreeViewItem::GetIsSelected() const
{
    return m_isSelected;
}

//===============================================================================================//
//  Description:
//      Get the labe; for this tree view item
//
//  Parameters:
//      None
//
//  Returns:
//      Constant string pointer
//===============================================================================================//
LPCWSTR TreeViewItem::GetLabel() const
{
    return m_Label.c_str();
}

//===============================================================================================//
//  Description:
//      Get the string data associated with this tree view item
//
//  Parameters:
//      None
//
//  Returns:
//      Constant string pointer
//===============================================================================================//
LPCWSTR TreeViewItem::GetStringData() const
{
    return m_StringData.c_str();
}

//===============================================================================================//
//  Description:
//      Reset the values of this class
//
//  Parameters:
//      None
//
//  Returns:
//      void
//===============================================================================================//
void TreeViewItem::Reset()
{
    m_isExpanded = false;
    m_isNode     = false;
    m_isSelected = false;
    m_depth      = 0;
    m_Label      = PXS_STRING_EMPTY;
    m_StringData = PXS_STRING_EMPTY;

    // Delete the bitmap
    if ( m_hBitmap )
    {
        DeleteObject( m_hBitmap );
        m_hBitmap = nullptr;
    }
}

//===============================================================================================//
//  Description:
//      Set the bitmap for this tree view item
//
//  Parameters:
//      pszBitmapName - pointer to resource ID for the bitmap
//      transparent   - nominated transparent colour
//      background    - background colour onto which the bitmap will be drawn
//
//  Returns:
//      void
//===============================================================================================//
void TreeViewItem::SetBitmap( LPCWSTR pszBitmapName, COLORREF transparent, COLORREF background )
{
    // Release any existing bitmap
    if ( m_hBitmap )
    {
        DeleteObject( m_hBitmap );
        m_hBitmap = nullptr;
    }

    if ( pszBitmapName == nullptr )
    {
        return;     // Nothing to do
    }

    m_hBitmap = LoadBitmap( GetModuleHandle( nullptr ), pszBitmapName );
    if ( m_hBitmap )
    {
        // Replace colour to make the bitmap transparent
        if ( ( transparent != CLR_INVALID ) && ( background != CLR_INVALID ) )
        {
            PXSReplaceBitmapColour( m_hBitmap, transparent, background );
        }
    }
}

//===============================================================================================//
//  Description:
//      Set if the this tree view item is expanded, applies to nodes only.
//
//  Parameters:
//      isExpanded - true if node expanded, else false
//
//  Returns:
//      void
//===============================================================================================//
void TreeViewItem::SetExpanded( bool isExpanded )
{
    m_isExpanded = isExpanded;
}

//===============================================================================================//
//  Description:
//      Set the depth level for this tree view item
//
//  Parameters:
//      depth - the depth level
//
//  Returns:
//      void
//===============================================================================================//
void TreeViewItem::SetDepth( WORD depth )
{
    m_depth = depth;
}

//===============================================================================================//
//  Description:
//      Set if the this tree view item is a node.
//
//  Parameters:
//      isNode - true if this item is a node otherwise it is a leaf
//
//  Returns:
//      void
//===============================================================================================//
void TreeViewItem::SetIsNode( bool isNode )
{
    m_isNode = isNode;
}

//===============================================================================================//
//  Description:
//      Set if the this tree view item is selected.
//
//  Parameters:
//      isSelected - true if this item is selected
//
//  Returns:
//      void
//===============================================================================================//
void TreeViewItem::SetIsSelected( bool isSelected )
{
    m_isSelected = isSelected;
}

//===============================================================================================//
//  Description:
//      Set the label for this tree view item
//
//  Parameters:
//      Label - the caption string
//
//  Returns:
//      void
//===============================================================================================//
void TreeViewItem::SetLabel( const String& Label )
{
    m_Label = Label;
}

//===============================================================================================//
//  Description:
//      Set the data string for this tree view item
//
//  Parameters:
//      StringData - the data string
//
//  Returns:
//      void
//===============================================================================================//
void TreeViewItem::SetStringData( const String& StringData )
{
    m_StringData = StringData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Protected Methods
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Private Methods
///////////////////////////////////////////////////////////////////////////////////////////////////
