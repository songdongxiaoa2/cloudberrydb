/*-------------------------------------------------------------------------
 *
 * storagecmds.h
 *	  storage server/user_mapping creation/manipulation commands
 *
 * Copyright (c) 2016-Present Hashdata, Inc.
 *
 *
 * IDENTIFICATION
 *	  src/include/commands/storagecmds.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef STORAGECMDS_H
#define STORAGECMDS_H

#include "catalog/objectaddress.h"
#include "nodes/parsenodes.h"


/* Flags for GetStorageServerExtended */
#define SSV_MISSING_OK	0x01

/* Helper for obtaining username for user mapping */
#define StorageMappingUserName(userid) \
	(OidIsValid(userid) ? GetUserNameFromId(userid, false) : "public")

typedef struct StorageServer
{
	Oid 		serverid;		/* storage server Oid */
	Oid 		owner;			/* storage server owner user Oid */
	char 		*servername;	/* name of the storage server */
	List 		*options;		/* srvoptions as DefElem list */
} StorageServer;

typedef struct StorageUserMapping
{
	Oid		umid;			/* Oid of storage user mapping */
	Oid		userid;			/* local user Oid */
	Oid		serverid;		/* storage server Oid */
	List	   *options;		/* useoptions as DefElem list */
} StorageUserMapping;

extern Oid get_storage_server_oid(const char *servername, bool missing_ok);
extern StorageServer *GetStorageServerExtended(Oid serverid, bits16 flags);
extern StorageServer *GetStorageServer(Oid serverid);
extern StorageServer *GetStorageServerByName(const char *srvname, bool missing_ok);
extern StorageUserMapping *GetStorageUserMapping(Oid userid, Oid serverid);
extern Datum transformStorageGenericOptions(Oid catalogId, Datum oldOptions, List *options);

#endif //STORAGECMDS_H
