///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Tab Window Class Header
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

#ifndef PXSBASE_TAB_WINDOW_H_
#define PXSBASE_TAB_WINDOW_H_

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
#include "PxsBase/Header Files/TList.h"
#include "PxsBase/Header Files/Window.h"

// 6. Forwards

///////////////////////////////////////////////////////////////////////////////////////////////////
// Interface
///////////////////////////////////////////////////////////////////////////////////////////////////

class TabWindow : public Window
{
    public:
        // Default constructor
        TabWindow();

        // Destructor
        ~TabWindow();

        // Methods
        DWORD   Append( bool allowClose,
                        Window* pWindow,
                        const String& Name, const String& ToolTip, HBITMAP hBitmap );
        void    DoLayout() override;
        int     GetMaxTabWidth() const;
        size_t  GetNumberTabs() const;
        DWORD   GetSelectedTabID() const;
        DWORD   GetTabIDFromIndex( size_t index );
        void    GetSelectedTabName( String* pName );
        Window* GetSelectedTabWindow();
        void    GetTabsBounds( RECT* pBounds );
        Window* GetWindowFromIndex( size_t index );
        Window* GetWindowFromTabID( DWORD tabID );
        int     GetPreferredTabStripHeight() const;
        void    RemoveTab( DWORD tabID );
        void    SetCloseBitmaps( WORD closeID, WORD closeOnID );
        void    SetMaxTabWidth( int maxTabWidth );
        void    SetNameAndToolTip( DWORD tabID, const String& Name, const String& ToolTip );
        void    SetSelectedTabID( DWORD tabID );
        void    SetTabColours( COLORREF tabGradient1,
                               COLORREF tabGradient2, COLORREF tabHiLiteBackground );
        void    SetTabBitmap( DWORD tabID, WORD resourceID );
        void    SetTabStripHeight( int tabStripHeight );
        void    SetTabVisible( DWORD tabID, bool visible );
        void    SetTabWidth( int tabWidth );

    protected:
        typedef struct _TYPE_TAB
        {
            bool    visible;
            bool    allowClose;
            WORD    bitmapResourceID;   // resource ID of hBitmap
            DWORD   tabID;
            RECT    bounds;
            Window* pWindow;
            String  ToolTip;
            String  Name;
            HBITMAP hBitmap;

            _TYPE_TAB():visible( true ),
                        allowClose( false ),
                        bitmapResourceID( 0 ),      // 0 = none
                        tabID( 0 ),                 // 0 = none
                        bounds(),                   // non-op
                        pWindow( nullptr ),
                        ToolTip(),
                        Name(),
                        hBitmap( nullptr )
            {
                memset( &bounds, 0, sizeof ( bounds ) );
            }
        } TYPE_TAB;

        // Data members
        int      HORIZ_TAB_GAP;         // pseudo-constant
        int      INTERNAL_LEADING;      // pseudo-constant
        int      BASELINE_HEIGHT;       // pseudo-constant
        int      MIN_TAB_WIDTH;         // pseudo-constant
        int      MIN_TAB_HEIGHT;        // pseudo-constant

        // Methods
        void    MouseLButtonDownEvent( const POINT& point, WPARAM keys ) override;
        void    MouseLButtonUpEvent( const POINT& point ) override;
        void    MouseMoveEvent( const POINT& point, WPARAM keys ) override;
        void    MouseRButtonDownEvent( const POINT& point ) override;
        void    NotifyEvent( const NMHDR* pNmhdr ) override;
        void    PaintEvent( HDC hdc ) override;
        void    TimerEvent( UINT_PTR timerID ) override;

    private:
        // Copy constructor - not allowed
        TabWindow( const TabWindow& oTabWindow );

        // Assignment operator - not allowed
        TabWindow& operator= ( const TabWindow& oTabWindow );

        // Methods
        void    DrawTabBitmapAndText( HDC hDC, const TYPE_TAB* pTab );
        void    DrawTabCloseBitmap( HDC hDC, const TYPE_TAB* pTab );
        void    DrawTabShape( HDC hDC, const TYPE_TAB* pTab );
        int     EvaluateTabWidth( HDC hDC, const TYPE_TAB* pTab );
   TYPE_TAB*    GetTabFromTabID( DWORD tabID );
        void    LayoutPages( const RECT& bounds );
        DWORD   MouseInTabID( const POINT& point );

        // Data members
        int      m_padding;
        int      m_tabWidth;
        int      m_maxTabWidth;
        int      m_tabStripHeight;
        UINT     m_timerID;
        DWORD    m_tabCounter;
        DWORD    m_selectedTabID;
        DWORD    m_mouseInTabID;
        HBITMAP  m_hBitmapClose;
        HBITMAP  m_hBitmapCloseOn;
        COLORREF m_crTabGradient1;
        COLORREF m_crTabGradient2;
        COLORREF m_crTabHiLiteBackground;
        TList< TYPE_TAB > m_Tabs;
};

#endif  // PXSBASE_TAB_WINDOW_H_
