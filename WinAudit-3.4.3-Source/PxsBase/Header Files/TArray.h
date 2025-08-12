///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Template Array Class Header - Inclusion Model
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

#ifndef PXSBASE_TARRAY_H_
#define PXSBASE_TARRAY_H_

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
#include "PxsBase/Header Files/MemoryException.h"

// 6. Forwards

///////////////////////////////////////////////////////////////////////////////////////////////////
// Interface
///////////////////////////////////////////////////////////////////////////////////////////////////

template< class T >
class TArray
{
    public:
        // Default constructor
        TArray();

        // Copy constructor
        TArray( const TArray& oTArray );

        // Destructor
        ~TArray();

        // Assignment operator
        TArray& operator= ( const TArray& oTArray );

        // Methods
        size_t      Add( const T& Element );
        void        Append( const TArray& NewArray );
        void        Copy( const TArray& NewArray );
        const T&    Get( size_t index ) const;
        T*          GetPtr( size_t index ) const;
        size_t      GetSize() const;
        void        Insert( size_t index, const T& Element );
        void        Remove( size_t index );
        void        RemoveAll();
        void        Set( size_t index, const T& Element );
        void        SetSize( size_t newSize );

    protected:
        // Methods

        // Data members
        T*      m_pArray;       // Pointer to array of elements
        size_t  m_size;         // Number of elements in array

    private:
        // Methods

        // Data members
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Template Array Class Implementation
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////////////////////////

// Default constructor
template< class T >
TArray< T >::TArray()
           :m_pArray( nullptr ),
            m_size( 0 )
{
}

// Copy constructor
template< class T >
TArray< T >::TArray( const TArray& oTArray )
            :TArray()

{
    *this = oTArray;
}

// Destructor
template< class T >
TArray< T >::~TArray()
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
TArray< T >& TArray< T >::operator= ( const TArray& oTArray )
{
    // Disallow self-assignment
    if ( this == &oTArray ) return *this;

    Copy( oTArray );

    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Public Methods
///////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================================================//
//  Description:
//      Add an element to the end of the array
//
//  Parameters:
//      Element - the element to add
//
//  Returns:
//      the zero based index of the new element, i.e. the old size.
//===============================================================================================//
template< class T >
size_t TArray< T >::Add( const T& Element )
{
    size_t idx = m_size;

    // "Insert" at the end
    Insert( m_size, Element );
    return idx;
}

//===============================================================================================//
//  Description:
//      Append an array to the end of this array
//
//  Parameters:
//      NewArray - the array to append
//
//  Returns:
//      void
//===============================================================================================//
template< class T >
void TArray< T >::Append( const TArray& NewArray )
{
   // Cannot append an array to itself
    if ( this == &NewArray )
    {
        throw FunctionException( L"this = NewArray", __FUNCTION__ );
    }
    size_t newSize   = PXSAddSizeT( m_size, NewArray.m_size );
    T*     pNewArray = new T[ newSize ];
    if ( pNewArray == nullptr )
    {
         throw MemoryException( __FUNCTION__ );
    }

    try
    {
        for ( size_t i = 0; i < newSize; i++ )
        {
            if ( i < m_size )
            {
                pNewArray[ i ] = m_pArray[ i ];
            }
            else
            {
                pNewArray[ i ] = NewArray.m_pArray[ i - m_size ];
            }
        }
    }
    catch ( const Exception& )
    {
        delete[] pNewArray;
        throw;
    }
    delete[] m_pArray;
    m_pArray = pNewArray;
    m_size   = newSize;
}

//===============================================================================================//
//  Description:
//      Copy the contents of the specified array into this array
//
//  Parameters:
//      NewArray - the array to copy
//
//  Returns:
//      void
//===============================================================================================//
template< class T >
void TArray< T >::Copy ( const TArray& NewArray )
{
    RemoveAll();
    Append( NewArray );
}

//===============================================================================================//
//  Description:
//      Get the element at the specified index
//
//  Parameters:
//      index - the zero-based index to get
//
//  Returns:
//      Reference to the element
//===============================================================================================//
template< class T >
const T& TArray< T >::Get( size_t index ) const
{
    if ( index >= m_size )
    {
        throw BoundsException( L"index", __FUNCTION__ );
    }

    return m_pArray[ index ];
}

//===============================================================================================//
//  Description:
//      Get a pointer to the element at the specified index
//
//  Parameters:
//      index - the zero-based index to get
//
//  Returns:
//      Pointer to the element
//===============================================================================================//
template< class T >
T* TArray< T >::GetPtr( size_t index ) const
{
    if ( index >= m_size )
    {
        throw BoundsException( L"index", __FUNCTION__ );
    }

    return &m_pArray[ index ];
}

//===============================================================================================//
//  Description:
//      Get the size of the array
//
//  Parameters:
//      None
//
//  Returns:
//      The size of the array
//===============================================================================================//
template< class T >
size_t TArray< T >::GetSize() const
{
    return m_size;
}

//===============================================================================================//
//  Description:
//      Insert an element into the array
//
//  Parameters:
//      index   - the zero-based index of where to insert the element
//      element - the element to insert
//
//  Remarks:
//      Can insert at the end.
//
//  Returns:
//      void
//===============================================================================================//
template< class T >
void TArray< T >::Insert( size_t index, const T& Element )
{
    // Allow index == m_size so can insert at end
    if ( index > m_size )
    {
        throw BoundsException( L"index", __FUNCTION__ );
    }

    size_t newSize   = PXSAddSizeT( m_size, 1 );
    T*     pNewArray = new T[ newSize ];
    if ( pNewArray == nullptr )
    {
        throw MemoryException( __FUNCTION__ );
    }

    try
    {
        for ( size_t i = 0; i < newSize; i++ )
        {
            if ( i < index )
            {
                pNewArray[ i ] = m_pArray[ i ];
            }
            else if ( i == index )
            {
                pNewArray[ index ] = Element;
            }
            else
            {
                pNewArray[ i + 1 ] = m_pArray[ i ];
            }
        }
    }
    catch ( const Exception& )
    {
        delete[] pNewArray;
        throw;
    }
    delete[] m_pArray;
    m_pArray = pNewArray;
    m_size   = newSize;
}

//===============================================================================================//
//  Description:
//      Remove the element at the specified index from the array
//
//  Parameters:
//      index - the zero-based index of the element to remove
//
//  Remarks:
//      Ignores out-of-bounds errors as the element does not exist.
//
//  Returns:
//      void
//===============================================================================================//
template< class T >
void TArray< T >::Remove( size_t index )
{
    // If out-of-bound will return without error
    if ( m_size == 0 ) return;
    if ( index >= m_size ) return;

    // Set new size
    size_t newSize = m_size - 1;     // From above m_size > 0
    if ( newSize == 0 )
    {
        RemoveAll();
        return;
    }

    T* pNewArray = new T[ newSize ];
    if ( pNewArray == nullptr )
    {
        throw MemoryException( __FUNCTION__ );
    }

    try
    {
        // Skip the element to be removed and copy in rest
        for ( size_t i = 0; i < newSize; i++ )
        {
            if ( i < index )
            {
                pNewArray[ i ] = m_pArray[ i ];
            }
            else if ( i > index )
            {
                pNewArray[ i ] = m_pArray[ i + 1 ];
            }
        }
    }
    catch ( const Exception& )
    {
        delete[] pNewArray;
        throw;
    }
    delete[] m_pArray;
    m_pArray = pNewArray;
    m_size   = newSize;
}

//===============================================================================================//
//  Description:
//      Remove all the elements from the array
//
//  Parameters:
//      None
//
//  Returns:
//      void
//===============================================================================================//
template< class T >
void TArray< T >::RemoveAll()
{
    delete[] m_pArray;
    m_pArray = nullptr;
    m_size   = 0;
}

//===============================================================================================//
//  Description:
//      Set the element at the specified index in the array
//
//  Parameters:
//      index - the zero-based index of the element to set
//
//  Returns:
//      void
//===============================================================================================//
template< class T >
void TArray< T >::Set( size_t index, const T& Element )
{
    if ( index >= m_size )
    {
        throw BoundsException( L"index", __FUNCTION__ );
    }
    m_pArray[ index ] = Element;
}

//===============================================================================================//
//  Description:
//      Set the size of the array
//
//  Parameters:
//      newSize - the new size of the array
//
//  Remarks:
//      Preserves any existing array elements up to newSize
//
//  Returns:
//      void
//===============================================================================================//
template< class T >
void TArray< T >::SetSize( size_t newSize )
{
    if ( newSize == 0 )
    {
        RemoveAll();
        return;
    }
    T* pNewArray = new T[ newSize ];
    if ( pNewArray == nullptr )
    {
        throw MemoryException( __FUNCTION__ );
    }

    try
    {
        // Test if need to copy existing elements
        if ( m_pArray == nullptr )
        {
            m_pArray = pNewArray;
            m_size   = newSize;
        }
        else
        {
            // Array already exists so fill the new array with old data, do
            // not run past end of either the new or old arrays.
            size_t i = 0;
            while ( ( i < newSize ) && ( i < m_size ) )
            {
                pNewArray[ i ] = m_pArray[ i ];   // Assignment
                i++;
            }
            delete[] m_pArray;
            m_pArray = pNewArray;
            m_size   = newSize;
        }
    }
    catch ( const Exception& )
    {
        delete[] pNewArray;
        throw;
    }
}

/*
template< class T >
void TArray< T >::SetSize( size_t newSize )
{
    if ( newSize == 0 )
    {
        RemoveAll();
        return;
    }
    T* pNewArray = new T[ newSize ];
    if ( pNewArray == nullptr )
    {
        throw MemoryException( __FUNCTION__ );
    }

    try
    {
        // Test if need to copy existing elements
        if ( m_pArray == nullptr )
        {
            m_pArray = pNewArray;
            m_size   = newSize;
        }
        else
        {
            // Array already exists so fill the new array with old data, do
            // not run past end of either the new or old arrays.
            size_t numElements = PXSMinSizeT( newSize, m_size );
            for ( size_t i = 0; i < numElements; i++ )
            {
                pNewArray[ i ] = m_pArray[ i ];   // Assignment
            }
            delete[] m_pArray;
            m_pArray = pNewArray;
            m_size   = newSize;
        }
    }
    catch ( const Exception& )
    {
        delete[] pNewArray;
        throw;
    }
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////
// Protected Methods
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Private Methods
///////////////////////////////////////////////////////////////////////////////////////////////////


#endif  // PXSBASE_TARRAY_H_
