// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2013 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#ifndef PX_PHYSICS_EXTENSIONS_DEFAULT_PVD_ERROR_CALLBACK_H
#define PX_PHYSICS_EXTENSIONS_DEFAULT_PVD_ERROR_CALLBACK_H

#include "foundation/PxErrorCallback.h"

#ifndef PX_DOXYGEN
namespace physx
{
#endif

	/**
	\brief An implementation of the error callback to send error message to PVD

	To send error message to PVD, just take this class as the base class
	of user's own error callback, call setVisualDebugger when PxVisualDebugger is available.

	User can also implement their own pvd error callback based on the default implementation.
	*/

	class PxVisualDebugger;
	class MessageArray;

	class PxDefaultPvdErrorCallback : public PxErrorCallback
	{
	public:
		PxDefaultPvdErrorCallback( PxAllocatorCallback& inAllocator, PxVisualDebugger* inDebugger = NULL );
		virtual ~PxDefaultPvdErrorCallback();

		/**
		\brief Set the PxVisualDebugger.

		Call this function when Physx Visual Debugger is connected and PxVisualDebugger is available.

		\param[in] inDebugger the PxVisualDebugger instance, returned by PxPhysics::getVisualDebugger().

		@see PxVisualDebugger, PxPhysics::getVisualDebugger().
		*/
		virtual void setVisualDebugger( PxVisualDebugger* inDebugger );

		virtual void reportError(PxErrorCode::Enum inErrorCode, 
			const char* inErrorMsg, const char* inFileName, int inLineNum);

		/**
		\brief Clear the message list.

		If visual debugger connection is not available, this callback will store error message in list,
		so it can send the lists when visual debugger is connected.
		*/
		void clearMessageHistory();

	protected:		
		PxAllocatorCallback&					mAllocator;
		PxVisualDebugger*						mDebugger;		
		PxDefaultPvdErrorCallback&	operator=( const PxDefaultPvdErrorCallback& another );
		PxDefaultPvdErrorCallback( const PxDefaultPvdErrorCallback& another );
		

	private:
		MessageArray*	createMessageArray();
		void			releaseMessageArray(MessageArray*);

		MessageArray*							mErrorMsgs;
	};

#ifndef PX_DOXYGEN
} // namespace physx
#endif

#endif
