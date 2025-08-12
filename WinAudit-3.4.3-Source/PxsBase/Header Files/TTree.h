///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Template Tree Class Header
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

#ifndef PXSBASE_TTREE_H_
#define PXSBASE_TTREE_H_

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
#include "PxsBase/Header Files/BoundsException.h"
#include "PxsBase/Header Files/FunctionException.h"
#include "PxsBase/Header Files/SystemException.h"

// 6. Forwards

///////////////////////////////////////////////////////////////////////////////////////////////////
// Interface
///////////////////////////////////////////////////////////////////////////////////////////////////

template< class T >
class TTree
{
    public:
        // Default constructor
        TTree();

        // Copy constructor
        TTree( const TTree& oTTree );

        // Destructor
        ~TTree();

        // Assignment operator
        TTree& operator= ( const TTree& oTTree );

        // Methods
        void     AppendChild( const T& Element );
        void     AppendSibling( const T& Element );
        const T* GetFirstChild();
        const T* GetLastChild();
        const T* GetNextSibling();
        const T* GetParent();
        const T* GetPreviousSibling();
        void     RemoveAll();
        void     SetGrowBy( size_t growBy );
        void     SetRoot( const T& Element );

    protected:
        // Methods

        // Data members

    private:
        // Hold a node
        typedef struct _TYPE_NODE
        {
            DWORD  depth;
            T      Element;

            _TYPE_NODE():depth( 0 ),
                         Element()
            {
            }
        } TYPE_NODE;

        // Methods
        void       GrowNodesArray();

        // Data members
        size_t      m_growBy;
        size_t      m_idxCurrent;
        size_t      m_numAllocated;
        size_t      m_size;
        TYPE_NODE*  m_pNodes;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Template List Class Implementation
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////////////////////////

// Default constructor
template< class T >
TTree< T >::TTree()
           :m_growBy( 1 ),
            m_idxCurrent( 0 ),
            m_numAllocated( 0 ),
            m_size( 0 ),
            m_pNodes( nullptr )
{
}

// Copy constructor
template< class T >
TTree< T >::TTree( const TTree& oTTree )
           :TTree()
{
    *this = oTTree;
}

// Destructor
template< class T >
TTree< T >::~TTree()
{
    try
    {
        RemoveAll();
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
template< class T >
TTree< T >& TTree< T >::operator= ( const TTree< T >& oTTree )
{
    size_t     i    = 0;
    TYPE_NODE* pNew = nullptr;

    if ( this == &oTTree ) return *this;

    // Bounds check as want to copy the nodes
    if ( oTTree.m_size > oTTree.m_numAllocated )
    {
        throw BoundsException( L"oTTree.m_size > oTTree.m_numAllocated", __FUNCTION__ );
    }

    // Allocate new nodes in exception safe manner
    if ( oTTree.m_numAllocated && oTTree.m_pNodes )
    {
        pNew = new TYPE_NODE[ oTTree.m_numAllocated ];
        if ( pNew == nullptr )
        {
            throw MemoryException( __FUNCTION__ );
        }

        // Copy in any existing nodes. The bounds m_size < numAllocated were checked above.
        try
        {
            for ( i = 0; i < oTTree.m_size; i++ )
            {
                pNew[ i ] = oTTree.m_pNodes[ i ];   // can throw
            }
        }
        catch ( const Exception& )
        {
            delete pNew;
            throw;
        }
    }

    // Replace at class scope
    delete [] m_pNodes;
    m_growBy       = oTTree.m_growBy;
    m_idxCurrent   = oTTree.m_idxCurrent;
    m_numAllocated = oTTree.m_numAllocated;
    m_size         = oTTree.m_size;
    m_pNodes       = pNew;

    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Public Methods
///////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================================================//
//  Description:
//      Append a child element to the current one and set the current position on it.
//
//  Parameters:
//      Element - the child element to add
//
//  Remarks:
//      For example, currently on A then insert X between F and G at same depth as B and E
//                  BEFORE        AFTER
//      depth:      0 1 2 3 4     0 1 2 3 4
//      index: 0 Root          Root
//             1     +A            +A
//             2       +B            +B
//             3         +C            +C
//             4           +D            +D
//             5       +E            +E
//             6         +F            +F
//             7     +G              +X
//             8       +H          +G
//             9                     +H
//
//  Returns:
//      void
//===============================================================================================//
template< class T >
void TTree< T >::AppendChild( const T& Element )
{
    DWORD  parentDepth, childDepth;
    size_t i, idxAppend;

    // Must have already created the root node
    if ( ( m_pNodes == nullptr ) || ( m_numAllocated == 0 ) )
    {
        throw FunctionException( L"m_pNodes/m_numAllocated", __FUNCTION__ );
    }

    // Grow the array if required
    if ( m_size >= m_numAllocated )
    {
        GrowNodesArray();
    }

    // Double check
    if ( ( m_pNodes == nullptr ) || ( m_size >= m_numAllocated ) )
    {
        throw SystemException( ERROR_INVALID_DATA, L"m_pNodes/m_size", __FUNCTION__ );
    }

    if ( m_idxCurrent >= m_size )
    {
        throw BoundsException( L"m_idxCurrent", __FUNCTION__ );
    }
    parentDepth = m_pNodes[ m_idxCurrent ].depth;
    childDepth  = PXSAddUInt32( parentDepth, 1 );

    // Get the position for the new node, 'F' in the above example
    i = PXSAddSizeT( m_idxCurrent, 1 );
    while ( ( i < m_size ) && ( m_pNodes[ i ].depth > parentDepth ) )
    {
        i++;
    }
    idxAppend = i;

    // Shuffle up the subsequent nodes, G and H in the above example
    i = m_size;
    while ( ( i > 0 ) && ( i >= idxAppend ) )
    {
        m_pNodes[ i ] = m_pNodes[ i - 1 ];
        i--;
    }
    m_size = PXSAddSizeT( m_size, 1 );

    // Set the appended node
    m_pNodes[ idxAppend ].depth   = childDepth;
    m_pNodes[ idxAppend ].Element = Element;
    m_idxCurrent = idxAppend;
}

//===============================================================================================//
//  Description:
//      Append a sibling element after the current one and set the current position on it.
//
//  Parameters:
//      Element - the sibling element
//
//  Remarks:
//      For example, currently on B then insert X between D and E at same depth as B and E
//                  BEFORE        AFTER
//      depth:      0 1 2 3 4     0 1 2 3 4
//      index: 0 Root          Root
//             1     +A            +A
//             2       +B            +B
//             3         +C            +C
//             4           +D            +D
//             5       +E            +E
//             6         +F            +F
//             7     +G              +X
//             8       +H          +G
//             9                     +H
//
//  Returns:
//      void
//===============================================================================================//
template< class T >
void TTree< T >::AppendSibling( const T& Element )
{
    const T* pParent;

    pParent = GetParent();      // i.e. 'A' in the above example
    if ( pParent == nullptr )
    {
        PXSLogAppError( L"The current node has no parent so cannot append a sibling." );
        throw FunctionException( L"pParent", __FUNCTION__ );
    }
    AppendChild( Element );
}

//===============================================================================================//
//  Description:
//      Get the first child element of the current one and set the current position on it.
//
//  Parameters:
//      none
//
//  Returns:
//      pointer to the first child element otherwise NULL if none
//===============================================================================================//
template< class T >
const T* TTree< T >::GetFirstChild()
{
    size_t i, parentDepth;

    if ( ( m_pNodes == nullptr ) || ( m_size == 0 ) )
    {
        return nullptr;
    }

    if ( m_idxCurrent >= m_size )
    {
        throw BoundsException( L"m_idxCurrent", __FUNCTION__ );
    }
    parentDepth = m_pNodes[ m_idxCurrent ].depth;

    // The first child, if there is one, will be the node immediately after the current
    i = PXSAddSizeT( m_idxCurrent, 1 );
    if ( ( i < m_size ) && ( m_pNodes[ i ].depth > parentDepth ) )
    {
        m_idxCurrent = i;
        return &m_pNodes[ m_idxCurrent ].Element;
    }

    return nullptr;
}

//===============================================================================================//
//  Description:
//      Get the last child element of the current one and set the current position on it.
//
//  Parameters:
//      none
//
//  Remarks:
//      For example, currently on A then last child is G
//      depth:      0 1 2 3 4
//      index: 0 Root
//             1     +A
//             2       +B
//             3         +C
//             4           +D
//             5       +E
//             6         +F
//             7       +G
//             8         +H
//             9      +I
//
//  Returns:
//      pointer to the last child element otherwise NULL if none
//===============================================================================================//
template< class T >
const T* TTree< T >::GetLastChild()
{
    DWORD  parentDepth, childDepth;
    size_t i, idxLast = PXS_MINUS_ONE;

    if ( ( m_pNodes == nullptr ) || ( m_size == 0 ) )
    {
        return nullptr;
    }

    if ( m_idxCurrent >= m_size )
    {
        throw BoundsException( L"m_idxCurrent", __FUNCTION__ );
    }
    parentDepth = m_pNodes[ m_idxCurrent ].depth;
    childDepth  = PXSAddUInt32( parentDepth, 1 );

    // Walk the nodes
    i = PXSAddSizeT( m_idxCurrent, 1 );
    while ( ( i < m_size ) && ( m_pNodes[ i ].depth > parentDepth ) )
    {
        // Is this a direct child?
        if ( m_pNodes[ i ].depth == childDepth )
        {
            idxLast = i;
        }
        i++;
    };

    if ( idxLast < m_size )
    {
        m_idxCurrent = idxLast;
        return  &m_pNodes[ m_idxCurrent ].Element;
    }

    return nullptr;
}

//===============================================================================================//
//  Description:
//      Get the next sibling element of the current one and set the current position on it.
//
//  Parameters:
//      none
//
//  Remarks
//      For example, if currently on B then the next sibling is E
//      depth:      0 1 2 3 4
//      index: 0 Root
//             1     +A
//             2       +B
//             3         +C
//             4           +D
//             5       +E
//             6         +F
//             7       +G
//             8     +H
//             9       +I
//
//  Returns:
//      pointer to the next sibling element otherwise NULL if none
//===============================================================================================//
template< class T >
const T* TTree< T >::GetNextSibling()
{
    size_t   i, depth;
    const T* pSibling = nullptr;

    if ( ( m_pNodes == nullptr ) || ( m_size == 0 ) )
    {
        return nullptr;
    }

    if ( m_idxCurrent >= m_size )
    {
        throw BoundsException( L"m_idxCurrent", __FUNCTION__ );
    }
    depth = m_pNodes[ m_idxCurrent ].depth;

    // The next sibling, if there is one, will be the next node with the same parent
    i = PXSAddSizeT( m_idxCurrent, 1 );
    while ( ( i < m_size ) && ( m_pNodes[ i ].depth >= depth ) && ( pSibling == nullptr ) )
    {
        // Is sibling?
        if ( m_pNodes[ i ].depth == depth )
        {
            m_idxCurrent = i;
            pSibling     = &m_pNodes[ m_idxCurrent ].Element;
        }
        i++;
    }

    return pSibling;
}

//===============================================================================================//
//  Description:
//      Get the parent element of the current one and set the current position on it.
//
//  Parameters:
//      none
//
//  Remarks
//      For example, if currently on E then the parent is A
//      depth:      0 1 2 3 4
//      index: 0 Root
//             1     +A
//             2       +B
//             3         +C
//             4           +D
//             5       +E
//             6         +F
//             7       +G
//             8     +H
//             9       +I
//
//  Returns:
//      pointer to the parent element otherwise NULL if none
//===============================================================================================//
template< class T >
const T* TTree< T >::GetParent()
{
    size_t   i, depth;
    const T* pParent = nullptr;

    if ( ( m_pNodes == nullptr ) || ( m_size == 0 ) )
    {
        return nullptr;
    }

    if ( m_idxCurrent >= m_size )
    {
        throw BoundsException( L"m_idxCurrent", __FUNCTION__ );
    }
    depth = m_pNodes[ m_idxCurrent ].depth;

    if ( m_idxCurrent == 0 )
    {
        return nullptr;     // on root so no parent
    }

    // Back up until find the parent
    i = m_idxCurrent;
    do
    {
        i--;
        if ( m_pNodes[ i ].depth < depth )
        {
            m_idxCurrent = i;
            pParent      = &m_pNodes[ m_idxCurrent ].Element;
        }
    } while ( ( i > 0 ) && ( pParent == nullptr ) );

    return pParent;
}

//===============================================================================================//
//  Description:
//      Get the previous sibling of the current element and set the current position on it.
//
//  Parameters:
//      none
//
//  Remarks
//      For example, if currently on E then the previous sibling is B
//      depth:      0 1 2 3 4
//      index: 0 Root
//             1     +A
//             2       +B
//             3         +C
//             4           +D
//             5       +E
//             6         +F
//             7       +G
//             8     +H
//             9       +I
//
//  Returns:
//      pointer to the previous sibling node otherwise NULL if none
//===============================================================================================//
template< class T >
const T* TTree< T >::GetPreviousSibling()
{
    size_t   i, depth;
    const T* pSibling = nullptr;

    if ( ( m_pNodes == nullptr ) || ( m_size == 0 ) )
    {
        return nullptr;
    }

    if ( m_idxCurrent >= m_size )
    {
        throw BoundsException( L"m_idxCurrent", __FUNCTION__ );
    }
    depth = m_pNodes[ m_idxCurrent ].depth;

    if ( m_idxCurrent == 0 )
    {
        return nullptr;     //  no previous nodes
    }

    // Back up the nodes. Note, no need to get to i = 0 as this is the root which has no siblings
    i = m_idxCurrent - 1;
    while ( ( i > 0 ) && ( m_pNodes[ i ].depth >= depth ) && ( pSibling == nullptr ) )
    {
        // Is sibling?
        if ( m_pNodes[ i ].depth == depth )
        {
            m_idxCurrent = i;
            pSibling     = &m_pNodes[ m_idxCurrent ].Element;
        }
        i--;
    }

    return pSibling;
}

//===============================================================================================//
//  Description:
//      Remove all the nodes
//
//  Parameters:
//      none
//
//  Returns:
//      void
//===============================================================================================//
template< class T >
void TTree< T >::RemoveAll()
{
    delete [] m_pNodes;
    m_pNodes       = nullptr;
    m_idxCurrent   = 0;
    m_numAllocated = 0;
    m_size         = 0;
}

//===============================================================================================//
//  Description:
//      Set the number of nodes to grow the nodes array when need to allocate new nodes
//
//  Parameters:
//      growBy - the number of nodes to grow the nodes array when need to allocate new nodes
//
//  Returns:
//      void
//===============================================================================================//
template< class T >
void TTree< T >::SetGrowBy( size_t growBy )
{
    if ( growBy == 0 )
    {
        throw ParameterException( L"growBy", __FUNCTION__ );
    }
    m_growBy = growBy;
}

//===============================================================================================//
//  Description:
//      Set the root element then set the current position on it.
//
//  Parameters:
//      none
//
//  Remarks:
//      Must call this method first to get the tree going.
//      Must not be called if there are nodes in the tree. Use RemoveAll to purge any nodes then
//      call this method.
//
//  Returns:
//      void
//===============================================================================================//
template< class T >
void TTree< T >::SetRoot( const T& Element )
{
    size_t newSize;

    if ( m_size )
    {
        throw FunctionException( L"m_size", __FUNCTION__ );
    }

    // Grow array if required
    newSize = PXSAddSizeT( m_size, 1 );
    if ( newSize > m_numAllocated )
    {
        GrowNodesArray();
    }

    // Double check
    if ( ( m_pNodes == nullptr ) || ( newSize < m_numAllocated ) )
    {
        throw SystemException( ERROR_INVALID_DATA, L"m_pNodes/newSize", __FUNCTION__ );
    }
    m_pNodes[ 0 ].depth   = 0;       // i.e top-level
    m_pNodes[ 0 ].Element = Element;
    m_idxCurrent = 0;
    m_size       = newSize;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Protected Methods
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Private Methods
///////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================================================//
//  Description:
//      Grow the nodes array
//
//  Parameters:
//      none
//
//  Returns:
//      void
//===============================================================================================//
template< class T >
void TTree< T >::GrowNodesArray()
{
    size_t     i = 0, numAllocated;
    TYPE_NODE* pNew;

    // Allocate new nodes
    if ( m_growBy == 0 )
    {
        throw ParameterException( L"m_growBy", __FUNCTION__ );
    }

    numAllocated = PXSAddSizeT( m_numAllocated, m_growBy );
    if ( numAllocated < m_size )
    {
        // Shouldn't get here as always growing the array so m_size
        throw BoundsException( L"numAllocated < m_size", __FUNCTION__ );
    }

    pNew = new TYPE_NODE[ numAllocated ];
    if ( pNew == nullptr )
    {
        throw MemoryException( __FUNCTION__ );
    }

    // Copy in any exisiting nodes. The bounds m_size < numAllocated were checked above.
    if ( m_pNodes )
    {
        for ( i = 0; i < m_size; i++ )
        {
            pNew[ i ] = m_pNodes[ i ];
        }
    }

    // Replace, note the current node and size remain unchanged
    delete [] m_pNodes;
    m_pNodes       = pNew;
    m_numAllocated = numAllocated;
}

#endif  // PXSBASE_TTREE_H_
