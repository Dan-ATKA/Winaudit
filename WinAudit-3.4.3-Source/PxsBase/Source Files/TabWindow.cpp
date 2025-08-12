///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Tab Window Class Implementation
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

///////////////////////////////////////////////////////////////////////////////
// Remarks
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////

// 1. Own Interface
#include "PxsBase/Header Files/TabWindow.h"

// 2. C System Files

// 3. C++ System Files

// 4. Other Libraries

// 5. This Project
#include "PxsBase/Header Files/Formatter.h"
#include "PxsBase/Header Files/ParameterException.h"
#include "PxsBase/Header Files/SystemException.h"
#include "PxsBase/Header Files/ToolTip.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////////////////////////

// Default Constructor
TabWindow::TabWindow()
          :Window(),
           HORIZ_TAB_GAP( 4 ),
           INTERNAL_LEADING( 2 ),
           BASELINE_HEIGHT( 1 ),
           MIN_TAB_WIDTH( 24 ),                 // Allow room for the close button
           MIN_TAB_HEIGHT( BASELINE_HEIGHT + 24 ),
           m_padding( 4 ),
           m_tabWidth( 0 ),
           m_maxTabWidth( 250 ),
           m_tabStripHeight( 0 ),               // 0 implies preferred layout height
           m_timerID( 1001 ),
           m_tabCounter( 0 ),
           m_selectedTabID( DWORD_MAX ),        // -1, i.e none selected
           m_mouseInTabID( DWORD_MAX ),         // -1, i.e mouse not in a tab
           m_hBitmapClose( nullptr ),
           m_hBitmapCloseOn( nullptr ),
           m_crTabGradient1( CLR_INVALID ),
           m_crTabGradient2( CLR_INVALID ),
           m_crTabHiLiteBackground( CLR_INVALID ),
           m_Tabs()

{
    // Creation parameters
    m_CreateStruct.cx    = 100;
    m_CreateStruct.cy    = 20;
    m_CreateStruct.style = WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE;

    // Properties
    try
    {
        SetBackground( GetSysColor( COLOR_BTNFACE ) );
        m_crTabHiLiteBackground = GetSysColor( COLOR_3DHILIGHT );
    }
    catch ( const Exception& e )
    {
        PXSLogException( e, __FUNCTION__ );
    }
}

// Copy Constructor - not allowed so no implementation

// Destructor
TabWindow::~TabWindow()
{
    TYPE_TAB* pTab = nullptr;

    // Delete the 'close' images
    if ( m_hBitmapClose )
    {
        DeleteObject( m_hBitmapClose );
    }

    if ( m_hBitmapCloseOn )
    {
        DeleteObject( m_hBitmapCloseOn );
    }

    // Ensure the timer has been killed
    if ( m_hWindow )
    {
        KillTimer( m_hWindow, m_timerID );  // Call is OK if no timer
    }

    // Free the bitmaps
    try
    {
        if ( m_Tabs.IsEmpty() == false )
        {
            m_Tabs.Rewind();
            do
            {
                pTab = m_Tabs.GetPointer();
                if ( pTab->hBitmap )
                {
                    DeleteObject( pTab->hBitmap );
                    pTab->hBitmap = nullptr;
                }
            } while ( m_Tabs.Advance() );
        }
    }
    catch ( const Exception& e )
    {
        PXSLogException( e, __FUNCTION__ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Operators
///////////////////////////////////////////////////////////////////////////////////////////////////

// Assignment operator - not allowed so no implementation

///////////////////////////////////////////////////////////////////////////////////////////////////
// Public Methods
///////////////////////////////////////////////////////////////////////////////////////////////////
//===============================================================================================//
//  Description:
//      Append a tab, set to invisible if it is not the selected tab
//
//  Parameters:
//      allowClose - indicates the tab can be closed
//      pWindow     - the window to display when the tab is selected,
//      Name        - the text to display on the tab
//      ToolTip     - tool tip text
//      hBitmap     - bitmap image to show on tab, can be NULL
//
//  Returns:
//      unique numerical identifier for the added tab, "tabID"
//===============================================================================================//
DWORD TabWindow::Append( bool allowClose,
                         Window* pWindow,
                         const String& Name, const String& ToolTip, HBITMAP hBitmap )
{
    TYPE_TAB  Tab;
    TYPE_TAB* pTab;

    if ( m_hWindow == nullptr )
    {
        throw FunctionException( L"m_hWindow", __FUNCTION__ );
    }

    if ( pWindow == nullptr )
    {
        throw ParameterException( L"pWindow", __FUNCTION__ );
    }

    // Make sure not adding the same window twice
    if ( m_Tabs.IsEmpty() == false )
    {
        m_Tabs.Rewind();
        do
        {
            if ( pWindow == m_Tabs.Get().pWindow )
            {
                throw SystemException( ERROR_OBJECT_ALREADY_EXISTS, L"pWindow", __FUNCTION__ );
            }
        } while ( m_Tabs.Advance() );
    }

    // Append a tab structure
    m_tabCounter = PXSAddUInt32( m_tabCounter, 1 );     // i.e. a unique id
    m_Tabs.Append( Tab );
    pTab = m_Tabs.GetPointer();     // Avoid a copy operation
    pTab->visible    = true;
    pTab->allowClose = allowClose ? true : false;
    pTab->tabID      = m_tabCounter;
    pTab->pWindow    = pWindow;
    pTab->Name       = Name;
    pTab->ToolTip    = ToolTip;
    pTab->hBitmap    = hBitmap;
    if ( pTab->hBitmap )
    {
        pTab->hBitmap = (HBITMAP)CopyImage( hBitmap, IMAGE_BITMAP, 0, 0, 0 );
    }

    // Select the tab after doing a layout so when it
    // becomes visible it is already at the correct location
    DoLayout();
    SetSelectedTabID( m_tabCounter );
    Repaint();

    return m_tabCounter;
}

//===============================================================================================//
//  Description:
//      Do the layout, position the tabs and the associated pages
//
//  Parameters:
//      None
//
//  Remarks:
//      Keep the layout of a tab in sync with PaintEvent
//
//  Returns:
//      Void
//===============================================================================================//
void TabWindow::DoLayout()
{
    int        bmHideHeight = 0;
    HGDIOBJ    hOldFont     = nullptr;
    RECT       clientRect   = { 0, 0, 0, 0 }, rect = { 0, 0, 0, 0 };
    BITMAP     bmHide;
    TEXTMETRIC tm;

    if ( m_hWindow == nullptr )
    {
        return;     // Nothing to do
    }
    GetClientRect( m_hWindow, &clientRect );

    if ( m_Tabs.IsEmpty() )
    {
        return;     // Nothing to do
    }

    // Evaluate the tab height
    HDC hDC = GetDC( m_hWindow );
    if ( hDC == nullptr )
    {
        return;
    }
    HFONT hFont = m_Font.GetHandle();
    if ( hFont )
    {
        hOldFont = SelectObject( hDC, hFont );
    }
    memset( &tm, 0, sizeof ( tm ) );
    GetTextMetrics( hDC, &tm );
    int tabHeight  = tm.tmHeight + tm.tmExternalLeading;
    tabHeight     += ( 2 * m_padding );
    tabHeight      = PXSMaxInt( tabHeight, MIN_TAB_HEIGHT );

    // Account for a possbile close button
    if ( m_hBitmapHide )
    {
        memset( &bmHide, 0, sizeof ( bmHide ) );
        if ( GetObject( m_hBitmapHide, sizeof ( bmHide ), &bmHide ) )
        {
            bmHideHeight = bmHide.bmHeight;
        }
    }

    // Catch exceptions as need to release DC
    try
    {
        int xPos = 0;
        int yPos = INTERNAL_LEADING;
        if ( bmHideHeight )
        {
            yPos = bmHideHeight;
        }

        m_Tabs.Rewind();
        do
        {
            TYPE_TAB* pTab = m_Tabs.GetPointer();
            SetRect( &pTab->bounds, -1, -1, -1, -1 );   // i.e. off screen
            if ( pTab->visible )
            {
                int tabWidth = EvaluateTabWidth( hDC, pTab );

                // Set the tab's bounds
                pTab->bounds.left   = xPos;
                pTab->bounds.right  = xPos + tabWidth;
                pTab->bounds.top    = yPos;
                pTab->bounds.bottom = yPos + tabHeight;
                if ( IsRightToLeftReading() )
                {
                    pTab->bounds.right = clientRect.right - xPos;
                    pTab->bounds.left  = pTab->bounds.right - tabWidth;
                }
                xPos += tabWidth;
            }
        } while ( m_Tabs.Advance() );
        yPos += tabHeight;

        // Layout the the pages in the remaining vertical space
        if ( yPos < m_tabStripHeight )
        {
            SetRect( &rect, 0, m_tabStripHeight, clientRect.right, clientRect.bottom );
        }
        else
        {
            SetRect( &rect, 0, yPos, clientRect.right, clientRect.bottom );
        }
        LayoutPages( rect );

        // Set the close button if there is one
        if ( m_hBitmapHide )
        {
            PositionHideBitmap();
        }
    }
    catch ( const Exception& )  // Ignore
    {}

    // Reset DC
    if ( hOldFont )
    {
        SelectObject( hDC, hOldFont );
    }
    ReleaseDC( m_hWindow, hDC );
    Repaint();
}

//===============================================================================================//
//  Description:
//      Get the maximum allowed width of the tabs in pixels
//
//  Parameters:
//      none
//
//  Returns:
//      signed integer of maximum width, 0 if is there is no limit.
//===============================================================================================//
int TabWindow::GetMaxTabWidth() const
{
    return m_maxTabWidth;
}

//===============================================================================================//
//  Description:
//      Get the number of tabs
//
//  Parameters:
//      none
//
//  Returns:
//      size_t
//===============================================================================================//
size_t TabWindow::GetNumberTabs() const
{
    return m_Tabs.GetLength();
}

//===============================================================================================//
//  Description:
//      Get the tab identifier at the specified position in the list of tabs.
//
//  Parameters:
//      index - the zero based index position
//
//  Returns:
//      the tab identifier, -1 if not found.
//===============================================================================================//
DWORD TabWindow::GetTabIDFromIndex( size_t index )
{
    DWORD  tabID   = DWORD_MAX;
    size_t counter = 0;

    if ( m_Tabs.IsEmpty() )
    {
        return DWORD_MAX;
    }

    m_Tabs.Rewind();
    do
    {
        if ( index == counter )
        {
            tabID = m_Tabs.GetPointer()->tabID;
        }
        counter = PXSAddSizeT( counter, 1 );
    } while ( ( tabID == DWORD_MAX ) && m_Tabs.Advance() );

    return tabID;
}

//===============================================================================================//
//  Description:
//      Get preferred tab strip height
//
//  Parameters:
//      None
//
//  Returns:
//      zero based selected tab number, -1 if none
//===============================================================================================//
int TabWindow::GetPreferredTabStripHeight() const
{
    int     height = 0;
    HDC     hDC;
    HFONT   hFont;
    HGDIOBJ hOldFont = nullptr;
    TEXTMETRIC tm;

    if ( m_hWindow == nullptr )
    {
        return MIN_TAB_HEIGHT;
    }

    hDC = GetDC( m_hWindow );
    if ( hDC == nullptr )
    {
        return MIN_TAB_HEIGHT;
    }

    try
    {
        hFont = m_Font.GetHandle();
    }
    catch ( const Exception& )
    {
        ReleaseDC( m_hWindow, hDC );
        throw;
    }

    if ( hFont )
    {
        hOldFont = SelectObject( hDC, hFont );
    }
    memset( &tm, 0, sizeof ( tm ) );
    GetTextMetrics( hDC, &tm );
    if ( hOldFont )
    {
        SelectObject( hDC, hOldFont );
    }
    ReleaseDC( m_hWindow, hDC );

    height = PXSAddInt32( tm.tmHeight, tm.tmExternalLeading );
    height = PXSAddInt32( height, m_padding        );
    height = PXSAddInt32( height, m_padding        );
    height = PXSMaxInt(   height, MIN_TAB_HEIGHT   );
    height = PXSAddInt32( height, INTERNAL_LEADING );
    height = PXSAddInt32( height, BASELINE_HEIGHT  );

    return height;
}

//===============================================================================================//
//  Description:
//      Get the identifier of the selected tab
//
//  Parameters:
//      None
//
//  Returns:
//      the identifier, -1 if no selected
//===============================================================================================//
DWORD TabWindow::GetSelectedTabID() const
{
    return m_selectedTabID;
}

//===============================================================================================//
//  Description:
//      Get the name of the selected tab
//
//  Parameters:
//      pName - string object to receive the tab's name
//
//  Returns:
//      void, output is zero length if no tab is selected
//===============================================================================================//
void TabWindow::GetSelectedTabName( String* pName )
{
    TYPE_TAB* pTab;

    if ( pName == nullptr )
    {
        throw ParameterException( L"pName", __FUNCTION__ );
    }
    pName->Zero();

    pTab = GetTabFromTabID( m_selectedTabID );
    if ( pTab )
    {
        *pName = pTab->Name;
    }
}

//===============================================================================================//
//  Description:
//      Get the window of the selected tab
//
//  Parameters:
//      None
//
//  Returns:
//      pointer to the tab's window (aka page), NULL if no tab is selected
//===============================================================================================//
Window* TabWindow::GetSelectedTabWindow()
{
    return GetWindowFromTabID( m_selectedTabID );
}

//===============================================================================================//
//  Description:
//      Get the bounds occupied by all of the tabs
//
//  Parameters:
//      pBounds - receives the bounds
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::GetTabsBounds( RECT* pBounds )
{
    if ( pBounds == nullptr )
    {
        throw ParameterException( L"pBounds", __FUNCTION__ );
    }
    memset( pBounds, 0, sizeof ( RECT ) );

    if ( m_Tabs.IsEmpty() )
    {
        return;
    }

    // As all tabs are on one line, the bounds are those of the last visible tab
    m_Tabs.End();
    *pBounds = m_Tabs.Get().bounds;
}

//===============================================================================================//
//  Description:
//      Get the window specified index postion in the list
//
//  Parameters:
//      tabID - the tab's zero-based postion in the list of tabs
//
//  Returns:
//      pointer to the window, else NULL
//===============================================================================================//
Window* TabWindow::GetWindowFromIndex( size_t index )
{
    DWORD tabID;

    tabID = GetTabIDFromIndex( index );
    if ( tabID == DWORD_MAX )
    {
        return nullptr;
    }

    return GetWindowFromTabID( tabID );
}

//===============================================================================================//
//  Description:
//      Get the window associated with a tab
//
//  Parameters:
//      tabID - the tab's identifier
//
//  Returns:
//      pointer to the window, else NULL
//===============================================================================================//
Window* TabWindow::GetWindowFromTabID( DWORD tabID )
{
    TYPE_TAB* pTab;

    pTab = GetTabFromTabID( tabID );
    if ( pTab )
    {
        return pTab->pWindow;
    }

    return nullptr;
}

//===============================================================================================//
//  Description:
//      Remove the specified tab
//
//  Parameters:
//      tabID - the tab's identifier
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::RemoveTab( DWORD tabID )
{
    bool      found = false;
    TYPE_TAB* pTab  = nullptr;

    // -1 signifies no identifier
    if ( m_Tabs.IsEmpty() || ( tabID == DWORD_MAX ) )
    {
        return;     // Nothing to do
    }

    // Locate the tab
    m_Tabs.Rewind();
    do
    {
        pTab = m_Tabs.GetPointer();
        if ( tabID == pTab->tabID )
        {
            found = true;
            if ( pTab->hBitmap )
            {
                DeleteObject( pTab->hBitmap );
                pTab->hBitmap = nullptr;
            }
            m_Tabs.Remove();
        }
    } while ( ( found == false ) && m_Tabs.Advance() );

    // Set the curent tab as the selected one
    if ( m_Tabs.IsEmpty() )
    {
        return;     // Nothing to do
    }
    SetSelectedTabID( m_Tabs.Get().tabID );
    DoLayout();
}

//===============================================================================================//
//  Description:
//      Set the bitmaps used for the "close" image
//
//  Parameters:
//      closeID   - resource id of the default close bitmap
//      closeOnID - resource id of the highlighted close bitmap
//
//  The images should be small, no more that 16x16pixels. They are loaded
//  from the executable.
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::SetCloseBitmaps( WORD closeID, WORD closeOnID )
{
    HMODULE hModule = GetModuleHandle( nullptr );

    // Clean up
    if ( m_hBitmapClose )
    {
        DeleteObject( m_hBitmapClose );
        m_hBitmapClose = nullptr;
    }

    if ( m_hBitmapCloseOn )
    {
        DeleteObject( m_hBitmapCloseOn );
        m_hBitmapCloseOn = nullptr;
    }

    // Load
    if ( closeID )
    {
        m_hBitmapClose = LoadBitmap( hModule, MAKEINTRESOURCE( closeID ) );
        if ( m_hBitmapClose == nullptr )
        {
            throw SystemException( GetLastError(), L"LoadBitmap", __FUNCTION__ );
        }
    }

    if ( closeOnID )
    {
        m_hBitmapCloseOn = LoadBitmap( hModule, MAKEINTRESOURCE( closeOnID ) );
        if ( m_hBitmapCloseOn == nullptr )
        {
            throw SystemException( GetLastError(), L"LoadBitmap", __FUNCTION__ );
        }
    }
}

//===============================================================================================//
//  Description:
//      Set the maximum allowed width of the tabs in pixels
//
//  Parameters:
//      maxTabWidth - Maximum allowed width of the the tabs in pixels.
//                    Useful if the tab name is excessively long and
//                    do not want a very wide tab.
//                    Set to 0 if want no restriction on tab width limit
//                    in which case the tab will expand to fit its contents
//  Returns:
//      void
//===============================================================================================//
void TabWindow::SetMaxTabWidth( int maxTabWidth )
{
    // See if there is something to do
    if ( m_maxTabWidth != maxTabWidth )
    {
        m_maxTabWidth = PXSMaxInt( 0, maxTabWidth );
        DoLayout();
    }
}

//===============================================================================================//
//  Description:
//      Set the specified tab's name and tool tip
//
//  Parameters:
//      tabID   - the tab's identifier
//      Name    - the name/caption to show on the tab
//      ToolTip - tool tip text to show on mouse over
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::SetNameAndToolTip( DWORD tabID, const String& Name, const String& ToolTip )
{
    TYPE_TAB* pTab;

    pTab = GetTabFromTabID( tabID );
    if ( pTab )
    {
        pTab->Name    = Name;
        pTab->ToolTip = ToolTip;
    }
}

//===============================================================================================//
//  Description:
//      Set the specified tab as the selected one
//
//  Parameters:
//      tabID - the tab's identifier
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::SetSelectedTabID( DWORD tabID )
{
    TYPE_TAB* pTab;

    pTab = GetTabFromTabID( tabID );
    if ( pTab == nullptr )
    {
        throw ParameterException( L"tabID", __FUNCTION__ );
    }
    m_selectedTabID = tabID;

    // Set the focus to the tab's page
    if ( pTab->pWindow && pTab->pWindow->GetHwnd() )
    {
        SetFocus( pTab->pWindow->GetHwnd() );
        pTab->pWindow->Repaint();
    }
    DoLayout();
    Repaint();
}

//===============================================================================================//
//  Description:
//      Set the tab's bitmap using the specified resource id
//
//  Parameters:
//      tabID      - the tab's identifier
//      resourceID - the resoure id should be a 16x16 bitmap
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::SetTabBitmap( DWORD tabID, WORD resourceID )
{
    TYPE_TAB* pTab;

    pTab = GetTabFromTabID( tabID );
    if ( pTab == nullptr )
    {
        return;     // Nothing to do
    }

    if ( pTab->bitmapResourceID == resourceID )
    {
        return;     // Nothing to do
    }

    if ( pTab->hBitmap )
    {
        DeleteObject( pTab->hBitmap );
    }
    pTab->hBitmap = static_cast<HBITMAP>( LoadImage( GetModuleHandle(nullptr),
                                          MAKEINTRESOURCE( resourceID ),
                                          IMAGE_BITMAP, 16, 16, 0 ) );
    pTab->bitmapResourceID = resourceID;

    // Will avoid Repaint() as UpdateWindow() takes a lot of processing
    InvalidateRect( m_hWindow, &pTab->bounds, FALSE );
}

//===============================================================================================//
//  Description:
//      Set the high light colour used by the tab when it is selected
//
//  Parameters:
//      tabGradient1        - graident colour 1
//      tabGradient2        - graident colour 2
//      tabHiLiteBackground - the high light colour
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::SetTabColours( COLORREF tabGradient1,
                               COLORREF tabGradient2, COLORREF tabHiLiteBackground )
{
    m_crTabGradient1        = tabGradient1;
    m_crTabGradient2        = tabGradient2;
    m_crTabHiLiteBackground = tabHiLiteBackground;
}

//===============================================================================================//
//  Description:
//      Set if tab strip height
//
//  Parameters:
//      tabStripHeight - the tab strip heigth, use zero for default height
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::SetTabStripHeight( int tabStripHeight )
{
    m_tabStripHeight = tabStripHeight;
}

//===============================================================================================//
//  Description:
//      Set if the specified tab is visible
//
//  Parameters:
//      tabID   - the tab's identifier
//      visible - true if tab is visible, else false
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::SetTabVisible( DWORD tabID, bool visible )
{
    DWORD     selectTabID = DWORD_MAX;      // -1 implies no tab selected
    TYPE_TAB* pTab;

    pTab = GetTabFromTabID( tabID );
    if ( pTab == nullptr )
    {
        return;     // Nothing to do
    }

    // Set the tab
    if ( visible )
    {
        selectTabID = tabID;    // Make the visible tab the selected one
    }
    else
    {
        if ( tabID == m_selectedTabID )
        {
            // Hiding the selected tab so select the one previous to it that is visible.
            // Note, at this point the m_tabs list is positioned on tabID.
            while ( ( selectTabID != DWORD_MAX ) && m_Tabs.Previous() )
            {
                if ( m_Tabs.Get().visible == true )
                {
                    selectTabID = m_Tabs.Get().tabID;
                }
            }

            // If there are no visible previous tabs then look forward.
            if ( selectTabID == DWORD_MAX )
            {
                m_Tabs.Rewind();
                do
                {
                    if ( m_Tabs.Get().visible == true )
                    {
                        selectTabID = m_Tabs.Get().tabID;
                    }
                } while (  ( selectTabID != DWORD_MAX ) && m_Tabs.Advance() );
            }
        }
    }
    pTab->visible = visible;
    SetSelectedTabID( selectTabID );

    // Set the associated window
    if ( pTab->pWindow )
    {
        pTab->pWindow->SetVisible( visible );
    }
    DoLayout();
}

//===============================================================================================//
//  Description:
//      Set the tab width of all of the tabs
//
//  Parameters:
//      tabWidth - the tab width, use zero for default height
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::SetTabWidth( int tabWidth )
{
    if ( tabWidth < MIN_TAB_WIDTH )
    {
        tabWidth = MIN_TAB_WIDTH;
    }
    else if ( tabWidth > m_maxTabWidth )
    {
        tabWidth = m_maxTabWidth;
    }

    if ( tabWidth != m_tabWidth )
    {
        m_tabWidth = tabWidth;
        DoLayout();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Protected Methods
///////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================================================//
//  Description:
//      Handle the mouse left button down event
//
//  Parameters:
//      point - POINT where mouse was left clicked in the client area
//      keys  - which virtual keys are down
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::MouseLButtonDownEvent( const POINT& point, WPARAM /* keys */ )
{
    POINT    screenPos = point;
    DWORD    tabID;
    WPARAM   wParam;
    ToolTip* pTip;

    if ( m_hWindow == nullptr )
    {
        return;
    }

    // Forward event to tool tip
    pTip = ToolTip::GetInstance();
    if ( pTip )
    {
        // Need screen coordinates
        ClientToScreen( m_hWindow, &screenPos );
        pTip->MouseButtonDown( screenPos );
    }

    // Forward to listener
    if ( m_hWndAppMessageListener == nullptr )
    {
        return;     // No listener
    }

    tabID = MouseInTabID( point );
    if ( tabID == DWORD_MAX )  // -1 means not in a tab
    {
        return;     // Nothing to notify about
    }
    SetSelectedTabID( tabID );
    wParam = MAKEWPARAM( PXS_APP_MSG_ITEM_SELECTED, 0 );
    SendMessage( m_hWndAppMessageListener, WM_COMMAND, wParam, (LPARAM)m_hWindow );
}

//===============================================================================================//
//  Description:
//      Handle the mouse left button up event
//
//  Parameters:
//      point - POINT where mouse was left clicked in the client area
//      keys  - which virtual keys are down
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::MouseLButtonUpEvent( const POINT& point )
{
    RECT   bounds = { 0, 0, 0, 0 };
    BITMAP bmClose;

    if ( m_hWindow == nullptr )
    {
        return;
    }

    if ( m_hWndAppMessageListener == nullptr )
    {
        return;     // No listener
    }

    // Determine if clicked on the window's hide button
    if ( IsPointInHideButton( point ) )
    {
        SendMessage( m_hWndHideListener, PXS_APP_MSG_HIDE_WINDOW, 0, (LPARAM)m_hWindow );
        return;
    }

    // Determine if clicked on a tab's close bitmap. If so will close it provided
    // there is more than one tab
    if ( m_Tabs.GetLength() <= 1 )
    {
        return;
    }
    DWORD tabID = MouseInTabID( point );
    if ( tabID == DWORD_MAX )  // -1 means not in a tab
    {
        return;     // Nothing to notify about
    }

    TYPE_TAB* pTab = GetTabFromTabID( tabID );
    if ( pTab == nullptr )
    {
        return;     // Nothing to do
    }

    if ( ( m_hBitmapClose == nullptr ) || ( pTab->allowClose == false ) )
    {
        return;     // Not showing a bitmap
    }
    memset( &bounds , 0, sizeof ( bounds ) );
    memset( &bmClose, 0, sizeof ( bmClose ) );
    GetObject( m_hBitmapClose, sizeof ( bmClose ), &bmClose );

    // Set bounds
    if ( IsRightToLeftReading() )
    {
        bounds.left = pTab->bounds.left + HORIZ_TAB_GAP;
    }
    else
    {
        bounds.left = pTab->bounds.right - bmClose.bmWidth - HORIZ_TAB_GAP;
    }
    bounds.right  = bounds.left + bmClose.bmWidth;
    bounds.top    = pTab->bounds.top + ( ( pTab->bounds.bottom -
                                           pTab->bounds.top    -
                                           bmClose.bmHeight    ) / 2 );
    bounds.bottom = bounds.top  + bmClose.bmHeight;

    if ( PtInRect( &bounds, point ) )
    {
        // Send a close message
        WPARAM wParam = MAKEWPARAM( PXS_APP_MSG_REMOVE_ITEM, 0 );
        SendMessage( m_hWndAppMessageListener, WM_COMMAND, wParam, (LPARAM)m_hWindow );
    }
}

//===============================================================================================//
//  Description:
//      Handle WM_MOUSEMOVE event.
//
//  Parameters:
//      point - a POINT specifying the cursor position in the client area
//      keys  - which virtual keys are down
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::MouseMoveEvent( const POINT& point, WPARAM /* keys */ )
{
    // Class scope data checks
    if ( m_hWindow == nullptr )
    {
        return;
    }
    SetCursor( LoadCursor( nullptr, IDC_ARROW ) );

    // If this window is not contained in the foreground window
    // then ignore rest of processing
    if ( IsChild( GetForegroundWindow(), m_hWindow )  == 0 )
    {
        return;
    }

    // Determine if the mouse is inside a tab, if so show its tool tip
    DWORD tabID = MouseInTabID( point );
    if ( tabID != DWORD_MAX )
    {
        SetTimer( m_hWindow, m_timerID, 100, nullptr );
        TYPE_TAB* pTab = GetTabFromTabID( tabID );
        ToolTip*  pTip = ToolTip::GetInstance();
        if ( pTab && pTip && pTab->ToolTip.GetLength() )
        {
            // Redraw, in case the tab has a close button
            Repaint();
            if ( tabID != m_mouseInTabID )
            {
                // Ha! Make a copy of the tab's bounds to avoid changing them
                // when call RectangleToScreen otherwise the tab will follow
                // the mouse around the screen.
                RECT tipBounds = pTab->bounds;
                RectangleToScreen( &tipBounds );
                pTip->Show( pTab->ToolTip, tipBounds );
            }
        }
    }

    // Handle redraw the close button
    if ( m_hBitmapHide )
    {
        if ( ( m_bHideWindowTimer == false ) && PtInRect( &m_recHideBitmap, point ) )
        {
            SetTimer( m_hWindow, HIDE_WINDOW_TIMER_ID, 100, nullptr );
            m_bHideWindowTimer = true;
        }
        InvalidateRect( m_hWindow, &m_recHideBitmap, FALSE );
    }

    // Store for the next mouse event
    if ( tabID != m_mouseInTabID )
    {
        m_mouseInTabID = tabID;
        Repaint();
    }
}

//===============================================================================================//
//  Description:
//      Handle the WM_RBUTTONDOWN event
//
//  Parameters:
//      point - POINT where mouse was right clicked in the client area
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::MouseRButtonDownEvent( const POINT& point )
{
    POINT screenPos = point;
    ToolTip* pTip   = nullptr;

    // Forward event to tool tip
    pTip = ToolTip::GetInstance();
    if ( pTip )
    {
        // Need screen coordinates
        ClientToScreen( m_hWindow, &screenPos );
        pTip->MouseButtonDown( screenPos );
    }
}

//===============================================================================================//
//  Description:
//      Handle WM_NOTIFY event.
//
//  Parameters:
//      pNmhdr - pointer to a NMHDR structure
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::NotifyEvent( const NMHDR* pNmhdr )
{
    // Forward to the event listener if there is one
    if ( m_hWndAppMessageListener )
    {
        SendMessage( m_hWndAppMessageListener, WM_NOTIFY, 0, (LPARAM)pNmhdr );
    }
}

//===============================================================================================//
//  Description:
//      Paint event handler
//
//  Parameters:
//      hdc - Handle to device context
//
//  Remarks:
//      This method and DoLayout need to be kept in sync.
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::PaintEvent( HDC hdc )
{
    StaticControl Static;

    if ( hdc == nullptr )
    {
        return;
    }
    DrawBackground( hdc );

    // Draw the tabs
    size_t numTabs = m_Tabs.GetLength();
    if ( numTabs )
    {
        m_Tabs.Rewind();
        do
        {
            TYPE_TAB* pTab = m_Tabs.GetPointer();
            if ( pTab->visible )
            {
                DrawTabShape( hdc, pTab );
                DrawTabBitmapAndText( hdc, pTab );
                if ( pTab->allowClose && ( numTabs > 1 ) )
                {
                    DrawTabCloseBitmap( hdc, pTab );
                }
            }
        } while ( m_Tabs.Advance() );
    }

    // Draw the base line
    SIZE size    = { 0, 0 };
    RECT bounds  = { 0, 0, 0, 0 };
    GetClientSize( &size );
    GetTabsBounds( &bounds );
    bounds.left   = 0;
    bounds.top    = bounds.bottom - BASELINE_HEIGHT;
    bounds.right  = size.cx;
    Static.SetBounds( bounds );
    Static.SetShape( PXS_SHAPE_RECTANGLE );
    Static.SetShapeColour( m_crForeground );
    Static.Draw( hdc );

    // Draw the border for the tab strip
    if ( ( m_uBorderStyle != PXS_SHAPE_NONE ) && m_tabStripHeight )
    {
        bounds.top    = 0;
        bounds.bottom = m_tabStripHeight - 1;
        Static.Reset();
        Static.SetBounds( bounds );
        Static.SetShape( m_uBorderStyle );
        Static.SetShapeColour( GetSysColor( COLOR_3DDKSHADOW ) );
        Static.Draw( hdc );
    }

    // Draw the close button if there is one
    if ( m_hBitmapHide )
    {
        DrawHideBitmap( hdc );
    }
}

//===============================================================================================//
//  Description:
//      Handle WM_TIMER event.
//
//  Parameters:
//      timerID - The identifier of the timer that fired this event
//
//  Remarks:
//      Timer events are triggered when the mouse enters the window
//
//  Returns:
//      void.
//===============================================================================================//
void TabWindow::TimerEvent( UINT_PTR timerID )
{
    POINT cursorPos = { 0, 0 };

    GetCursorPos( &cursorPos );
    ScreenToClient( m_hWindow, &cursorPos );
    m_mouseInTabID = MouseInTabID( cursorPos );
    if ( m_mouseInTabID == DWORD_MAX )      // i.e mouse is no longer in a tab
    {
        KillTimer( m_hWindow, timerID );
        if ( timerID == HIDE_WINDOW_TIMER_ID )
        {
            m_bHideWindowTimer = false;
        }
        Repaint();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Private Methods
///////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================================================//
//  Description:
//      Draw the tab's text
//
//  Parameters:
//      hDC  - handle to the device context
//      pTab - pointer to the tab
//
//  Remarks:
//      Always draw the close button last
//      Layout: gap | bitmap | gap | text | gap | close image | gap
//
//  Returns:
//      Zero-based tab number the pointer is in. If the pointer is not in a
//      tab returns DWORD_MAX
//
//===============================================================================================//
void TabWindow::DrawTabBitmapAndText( HDC hDC, const TYPE_TAB* pTab )
{
    RECT    bounds     = { 0, 0, 0, 0 };
    UINT    textFormat = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
    HGDIOBJ hOldFont   = nullptr;
    BITMAP  bmBitmap, bmClose;
    StaticControl Static;

    if ( ( hDC == nullptr ) || ( pTab == nullptr ) )
    {
        return;
    }
    int tabHeight = pTab->bounds.bottom - pTab->bounds.top;

    // Bitmaps
    memset( &bmClose, 0, sizeof ( bmClose ) );
    if ( m_hBitmapClose )
    {
        GetObject( m_hBitmapClose, sizeof ( bmClose ), &bmClose );
    }

    memset( &bmBitmap, 0, sizeof ( bmBitmap ) );
    if ( pTab->hBitmap )
    {
        GetObject( pTab->hBitmap, sizeof ( bmBitmap ), &bmBitmap );
    }

    // Determine if there is enough room for the bitmaps
    int requiredWidth = HORIZ_TAB_GAP + bmBitmap.bmWidth;
    if ( pTab->allowClose )
    {
        requiredWidth += bmClose.bmWidth + HORIZ_TAB_GAP;
    }

    if ( requiredWidth > ( pTab->bounds.right - pTab->bounds.left ) )
    {
        return;
    }

    // Draw bitmap
    bool rightToLeft = IsRightToLeftReading();
    if ( bmBitmap.bmWidth )
    {
        if ( rightToLeft )
        {
            bounds.left = pTab->bounds.right - bmBitmap.bmWidth - HORIZ_TAB_GAP;
        }
        else
        {
            bounds.left = pTab->bounds.left + HORIZ_TAB_GAP;
        }
        bounds.right = bounds.left + bmBitmap.bmWidth;
        bounds.top    = pTab->bounds.top + ( tabHeight - bmBitmap.bmHeight )/2;
        bounds.bottom = bounds.top + bmBitmap.bmHeight;
        Static.SetBitmap( pTab->hBitmap, CLR_INVALID );
        Static.SetBounds( bounds );
        Static.SetPadding( 0 );
        Static.Draw( hDC );
    }

    // Text area
    bounds = pTab->bounds;
    if ( rightToLeft )
    {
        textFormat  |= ( DT_RIGHT | DT_RTLREADING );
        bounds.left  = pTab->bounds.left  + HORIZ_TAB_GAP + bmClose.bmWidth  + HORIZ_TAB_GAP;
        bounds.right = pTab->bounds.right - HORIZ_TAB_GAP - bmBitmap.bmWidth - HORIZ_TAB_GAP;
    }
    else
    {
        bounds.left  = pTab->bounds.left  + HORIZ_TAB_GAP + bmBitmap.bmWidth + HORIZ_TAB_GAP;
        bounds.right = pTab->bounds.right - HORIZ_TAB_GAP - bmClose.bmWidth  - HORIZ_TAB_GAP;
    }

    if ( bounds.right <= bounds.left )
    {
        return;     // No room to draw the text
    }

    HFONT hFont = m_Font.GetHandle();
    if ( hFont )
    {
        hOldFont = SelectObject( hDC, hFont );
    }
    int    oldBkMode = SetBkMode( hDC, TRANSPARENT );
    size_t length    = pTab->Name.GetLength();
    if ( length )
    {
        int nLength = PXSCastSizeTToInt32( length );
        DrawText( hDC, pTab->Name.c_str(), nLength, &bounds, textFormat );
    }

    // Reset DC
    if ( oldBkMode ) SetBkMode( hDC, oldBkMode );
    if ( hOldFont  ) SelectObject( hDC, hOldFont );
}

//===============================================================================================//
//  Description:
//      Draw the close bitmap on the tab
//
//  Parameters:
//      hDC  - handle to the device context
//      pTab - pointer to the tab
//
//  Returns:
//     void
//===============================================================================================//
void TabWindow::DrawTabCloseBitmap( HDC hDC, const TYPE_TAB* pTab )
{
    RECT   bounds;
    POINT  cursor = { 0, 0 };
    BITMAP bmClose;
    StaticControl Static;

    if ( ( m_hBitmapClose == nullptr ) || ( hDC == nullptr ) || ( pTab == nullptr ) )
    {
        return;     // Nothing to do
    }
    bounds = pTab->bounds;

    // Will assume the default and "on" are the same size
    memset( &bmClose, 0, sizeof ( bmClose ) );
    if ( GetObject( m_hBitmapClose, sizeof ( bmClose ), &bmClose ) == 0 )
    {
        return;
    }

    // Set bounds
    if ( IsRightToLeftReading() )
    {
        bounds.left += HORIZ_TAB_GAP;
    }
    else
    {
        bounds.left = bounds.right - bmClose.bmWidth - HORIZ_TAB_GAP;
    }
    bounds.right  = bounds.left + bmClose.bmWidth;
    bounds.top    = bounds.top  + (bounds.bottom-bounds.top-bmClose.bmHeight)/2;
    bounds.bottom = bounds.top  + bmClose.bmHeight;

    // Highlighted image of selected tab on mouse over
    if ( ( GetCursorPos( &cursor )  ) &&
         ( ScreenToClient( m_hWindow, &cursor ) ) &&
         ( PtInRect( &bounds, cursor ) ) )
    {
        Static.SetBitmap( m_hBitmapCloseOn, PXS_COLOUR_WHITE );
    }
    else
    {
        Static.SetBitmap( m_hBitmapClose, PXS_COLOUR_WHITE );
    }
    Static.SetBounds( bounds );
    Static.SetPadding( 0 );
    Static.Draw( hDC );
}

//===============================================================================================//
//  Description:
//      Draw the specified tab
//
//  Parameters:
//      hDC  - handle to the device context
//      pTab - pointer to the tab
//
//  Returns:
//     void
//===============================================================================================//
void TabWindow::DrawTabShape( HDC hDC, const TYPE_TAB* pTab )
{
    bool  rtlReading;
    RECT  bounds;
    DWORD shape = PXS_SHAPE_TAB;
    StaticControl Static;

    if ( ( hDC == nullptr ) || ( pTab == nullptr ) )
    {
        return;
    }

    if ( pTab->tabID == m_selectedTabID )
    {
        Static.SetBackground( m_crTabHiLiteBackground );
    }
    else
    {
        Static.SetBackgroundGradient( m_crTabGradient1, m_crTabGradient2, true );
    }

    bounds     = pTab->bounds;
    rtlReading = IsRightToLeftReading();
    if ( pTab->tabID == m_mouseInTabID )
    {
        bounds.bottom -= BASELINE_HEIGHT;
        if ( rtlReading )
        {
            shape = PXS_SHAPE_TAB_DOTTED_RTL;
        }
        else
        {
            shape = PXS_SHAPE_TAB_DOTTED;
        }
    }
    else if ( rtlReading )
    {
        shape = PXS_SHAPE_TAB_RTL;
    }
    Static.SetShape( shape );
    Static.SetShapeColour( m_crForeground );
    Static.SetBounds( bounds );
    Static.Draw( hDC );
}

//===============================================================================================//
//  Description:
//      Evaluate the preferred with of a tab in pixels
//
//  Parameters:
//      hDC       - the device context onto which the tab is to be drawn
//      pTab      - pointer to the tab
//
//  Remarks:
//      Layout is (LTR): gap | bitmap | gap | text | gap | close image | gap
//
//  Returns:
//      width of the tab in pixels
//===============================================================================================//
int TabWindow::EvaluateTabWidth( HDC hDC, const TYPE_TAB* pTab )
{
    // If a width was specified, use that
    if ( m_tabWidth > 0 )
    {
        return PXSMinInt( m_tabWidth, m_maxTabWidth );
    }

    if ( ( hDC == nullptr ) || ( pTab == nullptr ) )
    {
        throw ParameterException( L"hDC/pTab", __FUNCTION__ );
    }

    // Bitmap image
    BITMAP bmImage;
    int    tabWidth = HORIZ_TAB_GAP;
    memset( &bmImage, 0, sizeof ( bmImage ) );
    if ( pTab->hBitmap )
    {
        memset( &bmImage, 0, sizeof ( bmImage ) );
        if ( GetObject( pTab->hBitmap, sizeof ( bmImage ), &bmImage ) )
        {
            tabWidth = PXSAddInt32( tabWidth, bmImage.bmWidth );
        }
    }
    tabWidth = PXSAddInt32( tabWidth, HORIZ_TAB_GAP );

    // Add the width of the text
    size_t len = pTab->Name.GetLength();
    if ( len )
    {
        int  nLength  = PXSCastSizeTToInt32( len );
        SIZE sizeText = { 0, 0 };
        if ( GetTextExtentPoint32( hDC, pTab->Name.c_str(), nLength, &sizeText ) )
        {
            tabWidth = PXSAddInt32( tabWidth, sizeText.cx );
        }
    }
    tabWidth = PXSAddInt32( tabWidth, HORIZ_TAB_GAP );

    // Add space for the close image even if do not show it
    if ( m_hBitmapClose )
    {
        memset( &bmImage, 0, sizeof ( bmImage ) );
        if ( GetObject( m_hBitmapClose, sizeof ( bmImage ), &bmImage ))
        {
            tabWidth = PXSAddInt32( tabWidth, bmImage.bmWidth );
        }
    }
    tabWidth = PXSAddInt32( tabWidth, HORIZ_TAB_GAP );

    // Limit the tab width. A non-positive value for maximum implies no limit
    if ( tabWidth < MIN_TAB_WIDTH )
    {
        tabWidth = MIN_TAB_WIDTH;
    }

    if ( m_maxTabWidth > 0 )
    {
        tabWidth = PXSMinInt( tabWidth, m_maxTabWidth );
    }
    return tabWidth;
}

//===============================================================================================//
//  Description:
//      Get the tab corresponding to the specified identifier
//
//  Parameters:
//      tabID - the tab's identifier
//
//  Returns:
//      pointer to the tab, NULL if not found
//===============================================================================================//
TabWindow::TYPE_TAB* TabWindow::GetTabFromTabID( DWORD tabID )
{
    TYPE_TAB* pTab = nullptr;

    // -1 indicates no identifier
    if ( m_Tabs.IsEmpty() || ( tabID == DWORD_MAX ) )
    {
        return nullptr;
    }

    m_Tabs.Rewind();
    do
    {
        if ( tabID == m_Tabs.GetPointer()->tabID )
        {
            pTab = m_Tabs.GetPointer();
        }
    } while ( ( pTab == nullptr ) && m_Tabs.Advance() );

    return pTab;
}

//===============================================================================================//
//  Description:
//      Layout the pages belonging to the tabs
//
//  Parameters:
//      bounds - the bounds for the pages
//
//  Returns:
//      void
//===============================================================================================//
void TabWindow::LayoutPages( const RECT& bounds )
{
    HWND      hWndPage = nullptr;
    TYPE_TAB* pTab     = nullptr;

    if ( m_Tabs.IsEmpty() )
    {
        return;     // Nothing to do
    }

    m_Tabs.Rewind();
    do
    {
        pTab = m_Tabs.GetPointer();
        if ( pTab->pWindow )
        {
            hWndPage = pTab->pWindow->GetHwnd();
            if ( hWndPage )
            {
                // Bring the page of the selected to the top
                if ( pTab->tabID == m_selectedTabID )
                {
                    SetWindowPos( hWndPage,
                                  nullptr,      // = HWND_TOP,
                                  bounds.left,
                                  bounds.top,
                                  bounds.right  - bounds.left,
                                  bounds.bottom - bounds.top, SWP_SHOWWINDOW );
                }
                else
                {
                    ShowWindow( hWndPage, SW_HIDE );
                    MoveWindow( hWndPage,
                                bounds.left,
                                bounds.top,
                                bounds.right  - bounds.left,
                                bounds.bottom - bounds.top, FALSE );
                }
            }
        }
    } while ( m_Tabs.Advance() );
}

//===============================================================================================//
//  Description:
//      Get the identifier of the tab the cursor is in.
//
//  Parameters:
//      point - POINT structure containing mouse coordinates in the client area
//
//  Returns:
//      The tab identifier other wise -1 if the pointer is not in a tab
//===============================================================================================//
DWORD TabWindow::MouseInTabID( const POINT& point )
{
    DWORD tabID = DWORD_MAX;       // -1 means not in a tab

    if ( m_Tabs.IsEmpty() )
    {
        return DWORD_MAX;   // Nothing to do
    }

    // Get tab's bounds and see if cursor is inside
    m_Tabs.Rewind();
    do
    {
        TYPE_TAB* pTab   = m_Tabs.GetPointer();
        RECT      bounds = { 0, 0, 0, 0 };
        if ( PtInRect( &pTab->bounds, point ) )
        {
            tabID = pTab->tabID;
        }
    } while ( ( tabID == DWORD_MAX ) && m_Tabs.Advance() );

    return tabID;
}
