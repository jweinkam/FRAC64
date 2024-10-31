#if !defined(LCMN_AUTOWIN32OBJECT_H)
#define LCMN_AUTOWIN32OBJECT_H
/* Start Header ------------------------------------------------------------

               Creo Products Inc.
               Burnaby B.C. Canada
               V5G 4M1

    Copyright (C) 1998 Creo Products Inc. Reproduction or disclosure of this
    file or its contents without the prior written consent of Creo Products
    Inc. is prohibited.

    File Name: LCMN_AutoWin32Object.h

    Purpose  : Declares "smart handle" template class for Win32 objects.  This
               class is NOT thread safe.

    Platform : Windows NT 4.0, Intel and Alpha

    Compiler : Visual C++ 4.2, Intel and Alpha

    Framework: none specifically required

    Package  : LCMN -- minimalistic low-low level common facilities.

    Project  : Araxi

- End Header --------------------------------------------------------------*/

/*
#ifndef PLTF_DEFS_H
#include "PLTF_defs.h"
#endif

#ifndef PLTF_WIN32_H
#include "PLTF_win32.h"
#endif

#ifndef LCMN_ASSERT_H
#include "LCMN_assert.h"
#endif
*/

// For some odd reason, many MFC projects don't end up including WINSVC.H, so do it now to ensure that SC_HANDLE is defined
#include <winsvc.h>

template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle>
            class LCMN_AutoWin32Object
{
public:
    
    // Default constructor, creates an invalid value
    LCMN_AutoWin32Object();

    // Normal constructor doubles as default constructor.
    // Initialize self with a raw object
    explicit LCMN_AutoWin32Object
    (
        ObjectType object
    );

    // Copy constructor transfers ownership of raw object to self from other.
    // Note argument needs to be non-const in this case.
    LCMN_AutoWin32Object
    (
        LCMN_AutoWin32Object &other 
    );

    // Close the underlying raw object if owned.
    virtual ~LCMN_AutoWin32Object();

    // Assignment operator transfers ownership to self, replacing prior.
    // Note argument needs to be non-const in this case.
    LCMN_AutoWin32Object& 
    operator= 
    ( 
        LCMN_AutoWin32Object&   other
    );

    // Accessor allows use as if a plain raw object. Use with care.
    operator ObjectType () const;

    // Equality operator
    bool operator==
    (
        const ObjectType&   rhs
    ) const;

    // Inequality operator
    bool operator!=
    (
        const ObjectType&   rhs
    ) const;

    // Same accessor as normal named member function.
    ObjectType 
    get() const;

    // Obsolete member, use get() -- implemented for downward compatibility.
    ObjectType 
    handle() const;

    // Do I represent a valid object?
    bool 
    isValid() const;

    // Relinquish ownership of the underlying object, so that self's destructor
    // will not close it.
    ObjectType 
    release();

    // Make self contain a different raw object, relinquishing prior if owned.
    // This interface is an alternative to overloaded operator =.
    void 
    set
    (
        ObjectType h
    );

    // Obsolete member, use set() -- implemented for downward compatability.
    void 
    setHandle
    (
        ObjectType h
    );

    // Close the underlying raw object.
    // Returns false if an error occurs closing the underlying raw object.
    void 
    close();

protected:

    // Make self contain a different raw object, relinquishing prior if owned.
    // Returns false if previous contents could not be closed without error.
    void 
    reset
    ( 
        ObjectType h
    );

    // Is the input a valid object?
    bool 
    isValidObject
    (
        const ObjectType&
    ) const;

private:

    ObjectType 					m_object; 			// the underlying raw object

}; // class LCMN_AutoWin32Object

// Invalid handle value
const HANDLE LCMN_INVALID_HANDLE = INVALID_HANDLE_VALUE;

// Default constructor, creates an invalid value
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle>
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::LCMN_AutoWin32Object() : 
m_object(NULL) 
{
    if ( useInvalidHandle )
    {
        // m_object can be of any type.  In order to 
        // ensure that this template always compiles,
        // type cast m_object BEFORE assigning.
        // BEWARE : if the type of m_object isn't
        // HANDLE, this could have disasterous results.
       	*( (HANDLE*)(&m_object) ) = INVALID_HANDLE_VALUE;
    }
}

// Normal constructor doubles as default constructor.
// Initialize self with a raw object
//
// explicit 
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle>
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::LCMN_AutoWin32Object
(
    ObjectType object
) : 
m_object(object) 
{}

// Copy constructor transfers ownership of raw object to self from other.
// Note argument needs to be non-const in this case.
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle>
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::LCMN_AutoWin32Object
(
    LCMN_AutoWin32Object &other 
) : 
m_object(other.release()) 
{}

// Close the underlying raw object if owned.
//
// virtual 
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle>
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::~LCMN_AutoWin32Object()
{
    close();
}


// Assignment operator transfers ownership to self, replacing prior.
// Note argument needs to be non-const in this case.
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle> LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>& 
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::operator= 
( 
    LCMN_AutoWin32Object&   other
)
{
    if (this != &other) // avoid assignment to self!
    {
        reset(other.release());
    }
    return *this;
}


// Equality operator
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle> bool 
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::operator==
(
    const ObjectType&   rhs
) const
{
    // return true if the objects are equal, of if this object
    // is invalid and the input is NULL.  This takes care of the
    // HANDLE case where invalid handles cna be either NULL or
    // INVALID_
    return m_object == rhs || (!isValid() && !isValidObject(rhs));
}


// Inequality operator
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle> bool 
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::operator!=
(
    const ObjectType&   rhs
) const
{
    return !operator==(rhs);
}


// Relinquish ownership of the underlying object, so that self's destructor
// will not close it.
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle> ObjectType 
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::release()
{
    ObjectType oldObject = m_object;

	if ( useInvalidHandle )
	{
        	*( (HANDLE*)(&m_object) ) = INVALID_HANDLE_VALUE;
	}
	else
	{
		m_object = NULL;
    }
	return oldObject;
}


// Accessor allows use as if a plain raw object. Use with care.
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle> 
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::operator ObjectType () const 
{ 
    return m_object; 
}

// Same accessor as normal named member function.
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle> ObjectType 
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::get() const 
{ 
    return m_object; 
}

// Obsolete member, use get() -- implemented for downward compatibility.
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle> ObjectType 
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::handle() const 
{ 
    return m_object; 
}

// Do I represent a valid object?
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle> bool 
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::isValid() const
{
    return isValidObject(m_object);
}

// Is the input a valid object?
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle> bool 
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::isValidObject
(
    const ObjectType&   object
) const
{
	if ( ( object == NULL ) || ( useInvalidHandle && object==INVALID_HANDLE_VALUE ) )
	{
			return false;
	}
	return true;
}

// Make self contain a different raw object, relinquishing prior if owned.
// This interface is an alternative to overloaded operator =.
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle> void 
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::set
(
    ObjectType h
)
{
    (void)reset(h);
}


// Obsolete member, use set() -- implemented for downward compatability.
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle> void 
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::setHandle
(
    ObjectType h
)
{
    (void)reset(h);
}


// Make self contain a different raw object, relinquishing prior if owned.
// Returns false if previous contents could not be closed without error.
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle> void 
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::reset
( 
    ObjectType h
)
{
    close();
    m_object = h;
    //return bOk;
}


// Close the underlying raw object.
// Returns false if an error occurs closing the underlying raw object.
template<class ObjectType, class ReturnType, ReturnType( (WINAPI *CloseObject)  )(ObjectType), BOOL useInvalidHandle> void 
LCMN_AutoWin32Object<ObjectType, ReturnType, CloseObject, useInvalidHandle>::close()
{
    // *** this logic is wrong for RegCloseKey - it should return 0 if no error occurred
    //ReturnType bResult;
    if ( isValid() )
    {
        (*CloseObject)(m_object);
        if ( useInvalidHandle )
        {
            // m_object can be of any type.  In order to 
            // ensure that this template always compiles,
            // type cast m_object BEFORE assigning.
            // BEWARE : if the type of m_object isn't
            // HANDLE, this could have disasterous results.
        	*( (HANDLE*)(&m_object) ) = INVALID_HANDLE_VALUE;
        }
		else
		{
			m_object = NULL ;
		}
    }
    //return bResult;
}


// Defined auto objects

// LoadLibrary returns an HMODULE, but FreeLibrary takes an HINSTANCE
// if STRICT is #defined, these are distinctly different types.
typedef LCMN_AutoWin32Object<HINSTANCE, BOOL, &(::FreeLibrary), TRUE> LCMN_AutoLibrary;

// CreateFile() returns INVALID_HANDLE_VALUE (a macro) while OpenMutex() returns NULL.
// NULL will always work as an invalid handle.  However, the internal handle will be
// assigned INVALID_HANDLE_VALUE in the default constructor and when the internal 
// object is closed.  IsValid() will return true if the internal handle is NULL or
// the invalidObject template parameter.
typedef LCMN_AutoWin32Object<HANDLE, BOOL, &(::CloseHandle), TRUE> LCMN_AutoHandle;
inline bool operator==(const HANDLE& lhs, const LCMN_AutoHandle& rhs) { return rhs.operator==(lhs); }
inline bool operator!=(const HANDLE& lhs, const LCMN_AutoHandle& rhs) { return rhs.operator!=(lhs); }

// this is intended for code that opens the event log for writing via RegisterEventSource
typedef LCMN_AutoWin32Object<HANDLE, BOOL, &(::DeregisterEventSource), FALSE> LCMN_AutoEventSource;

// this is intended for code that opens the event log for reading via OpenEventLog
typedef LCMN_AutoWin32Object<HANDLE, BOOL, &(::CloseEventLog), FALSE> LCMN_AutoEventLog;

// this is intended for handles opened using OpenSCManager, OpenService and/or CreateService
typedef LCMN_AutoWin32Object<SC_HANDLE, BOOL, &(::CloseServiceHandle), FALSE> LCMN_AutoServiceHandle;

// *** note that the non-boolean function return type breaks the semantics of the close() method
// if STRICT is #define'd, then HKEY is a distinct type from the more generic HANDLE
typedef LCMN_AutoWin32Object<HKEY, LONG, &(::RegCloseKey), FALSE> LCMN_AutoHKey;

// this is intended for handles opened using ::FindFirstFile
typedef LCMN_AutoWin32Object<HANDLE, BOOL, &(::FindClose), TRUE> LCMN_AutoFindFile;
inline bool operator==(const HANDLE& lhs, const LCMN_AutoFindFile& rhs) { return rhs.operator==(lhs); }
inline bool operator!=(const HANDLE& lhs, const LCMN_AutoFindFile& rhs) { return rhs.operator!=(lhs); }


#endif // LCMN_AUTOWIN32OBJECT_H