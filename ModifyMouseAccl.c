/*	File:		ModifyMouseAccl.c		Description:				Snippet to demonstrate how one uses the CursorDevice Manager to change the 				acceleration curve setting associated with the standard Apple mouse.					You can select an extremely sensitive or a tablet-like acceleration setting				by setting the NOACCLCURVE define appropriately.	Author:		RK	Copyright: 	Copyright: � 1984-1999 by Apple Computer, Inc.				all rights reserved.		Disclaimer:	You may incorporate this sample code into your applications without				restriction, though the sample code has been provided "AS IS" and the				responsibility for its operation is 100% yours.  However, what you are				not permitted to do is to redistribute the source as "DSC Sample Code"				after having made changes. If you're going to re-distribute the source,				we require that you make it clear in the source that the code was				descended from Apple Sample Code, but that you've made changes.		Change History (most recent first):				6/22/99 Updated for Metrowerks Codewarrior Pro 2.1*/#include <Types.h>#include <CursorDevices.h>#include "CrsrDev.h"#include <stdio.h>#include <LowMem.h>#define NOACCLCURVE		0	// set to 0 for a really zippy mouse							// set to 1 for a tablet style mouse - one with no acceleration#define mapTrue			0xFFFF#define acclRsrcType	'accl'#define k200dpiMouseID	'@200'	// the following points are from the ROM sources for the "@200"	// 200 dpi mouse accl setting for the upper curve	// leave the original values alone so that we can restore the 	// setting	// here are the original values	AcclPoint	gOrigAcclPt[9] =    {{0x0000713B, 0x00006000},								 {0x00044EC5, 0x00108000},								 {0x000C0000, 0x005F0000},							     {0x0016EC4F, 0x008B0000},							     {0x001D3B14, 0x00948000},							     {0x00227627, 0x00960000},							     {0x00246276, 0x00960000},								 {0x00260000, 0x00960000},								 {0x00280000, 0x00960000}};	// here are the replacement values for test	// modify these on a trial and error basis.#if NOACCLCURVEAcclPoint	gReplaceAcclPt[9] = {{0x00010000, 0x00010000},								 {0x00010000, 0x00010000},								 {0x00010000, 0x00010000},								 {0x00010000, 0x00010000},								 {0x00010000, 0x00010000},								 {0x00010000, 0x00010000},								 {0x00010000, 0x00010000},								 {0x00010000, 0x00010000},								 {0x00010000, 0x00010000}};#elseAcclPoint	gReplaceAcclPt[9] = {{0x00000400, 0x00006000},								 {0x00000800, 0x00108000},								 {0x00000C00, 0x00600000},							     {0x00001000, 0x00A00000},							     {0x00002000, 0x00C08000},							     {0x00004000, 0x01000000},							     {0x00008000, 0x02000000},								 {0x0000C000, 0x04000000},								 {0x00010000, 0x08000000}};#endifCursorDevicePtr	GetMouseCursorDeviceRec(UInt32 devID);AcclRsrcHandle	GetAcclRsrc(UInt32 devID);OSErr ModifySecond200dpiTable(AcclRsrcHandle acclRsrcHndl, 						AcclPoint *acclPtr, short numPts);main(){	CursorDevicePtr	cdPtr;	AcclRsrcHandle	acclRsrcHndl, saveHndl;	Fixed			accel = 0x00010000;	Size			hsize;	OSErr			err;		cdPtr = GetMouseCursorDeviceRec((UInt32)k200dpiMouseID);	if (cdPtr == NULL)	{		// device not found		printf("\nDevice record for 200 dpi mouse not found");		return 1;	}		acclRsrcHndl = GetAcclRsrc((UInt32)k200dpiMouseID);	if (acclRsrcHndl == NULL)	{		// device not found		printf("\naccl resource for 200 dpi mouse not found");		return 1;	}			// save a copy of the handle	hsize = GetHandleSize((Handle)acclRsrcHndl);	if (hsize < 0)	{		printf("\nerror getting handle size");		return 1;	}	saveHndl = (AcclRsrcHandle)NewHandle(hsize);		if (saveHndl == nil)	{		printf("\nerror allocating the save handle");		return 1;	}		// copy contents of the handle	BlockMove((Ptr)*acclRsrcHndl, (Ptr)*saveHndl, hsize);			// check whether there are two tables for the 200 dpi accl resource	if ((*acclRsrcHndl)->numAcclTables <= 1)	{		// device not recognized		printf("\naccl resource for 200 dpi mouse not recognized");		return 1;	}		// modify 200 dpi table using modified values	err = ModifySecond200dpiTable(acclRsrcHndl, (AcclPoint*)gReplaceAcclPt,								sizeof(gReplaceAcclPt)/sizeof(AcclPoint));	if (err != noErr)	{		// error adding modified values		printf("\nerror adding modified values - %d", err);		return 1;	}			// the resource has been modified but we need to set the		// acceleration to have the modified accl values take effect	CursorDeviceSetAcceleration(cdPtr, accel);		printf("\nthe modified accl field has been installed");	printf("\nclick the mouse when finished");	printf("\n");	printf("\n");		while (!Button())			;//Do Nothing		// restore original accl values				// copy contents of the handle	BlockMove((Ptr)*saveHndl, (Ptr)*acclRsrcHndl, hsize);		// the resource has been modified but we need to set the		// acceleration to have the modified accl values take effect	CursorDeviceSetAcceleration(cdPtr, accel);	return 0;}OSErr ModifySecond200dpiTable(AcclRsrcHandle acclRsrcHndl, AcclPoint *acclPtr,								short numPts){	AcclTablePtr	acclTablePtr;	ResType			rType;	short			sizeTable1, sizeTable2;	//short			resSave, sysRefNum;	short			rID;	OSErr			err;	Str255			rName;		// get the resource number for the accl resource	GetResInfo((Handle)acclRsrcHndl, &rID, &rType, rName);	if (err = ResError())		return err;				// get the size of the first table	sizeTable1 = (*acclRsrcHndl)->acclTable[0].numAcclPoints;	sizeTable1--;	// subtract off the size of the first entry			// set the acclTablePtr to point to the second table	acclTablePtr = (AcclTablePtr)((long)&(*acclRsrcHndl)->acclTable[1] + 							sizeTable1 * sizeof(AcclPoint));									// get the number of accl points in the second table	sizeTable2 = acclTablePtr->numAcclPoints;				// set sizeTable2 to the lesser of itself and numPts	sizeTable2 = sizeTable2 > numPts ? numPts : sizeTable2;		BlockMove((Ptr)acclPtr, (Ptr)&acclTablePtr->acclPoints,					sizeTable2 * sizeof(AcclPoint));		return err;			} /*	GetMouseCursorDeviceRec finds a CursorDevice record with a device ID matching	the devID input parameter*/CursorDevicePtr	GetMouseCursorDeviceRec(UInt32 devID){    CursorDevicePtr cdPtr;    Boolean			done = false;    cdPtr = NULL;    while (done == false)    {    	CursorDeviceNextDevice(&cdPtr);    	if (cdPtr->devID == (OSType)devID)	// is this the record that we want    		done = true;    	else if (cdPtr->nextCursorDevice == NULL)	// is this the last record in the chain    	{    		done = true;    		cdPtr = NULL;    	}//		else	look at the next record    }    return cdPtr;}AcclRsrcHandle	GetAcclRsrc(UInt32 devID){	AcclRsrcHandle	acclHndl;	short			numAcclRsrc;	short			i;	Boolean			done = false;	UInt8			romMapSave;		romMapSave = LMGetROMMapInsert();	// get the state of the ROM Map Insert;	LMSetROMMapInsert(mapTrue);			// map the ROM into the resource chain	numAcclRsrc = CountResources(acclRsrcType);		i = 0;		// initialize our index		while (done == false)	{		if (i >= numAcclRsrc)	// is the index past the number of 'accl' resources		{			done = true;		// let's bail			acclHndl = NULL;	// set result		}		else		{			i++;	// increment the counter since we start at 0			acclHndl = (AcclRsrcHandle)GetIndResource(acclRsrcType, i);						if ((*acclHndl)->acclClass == (UInt32)devID)			{				done = true;			}		}	}		LMSetROMMapInsert(romMapSave);			// restore ROM Map insert state	return acclHndl;}