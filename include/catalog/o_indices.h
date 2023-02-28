/*-------------------------------------------------------------------------
 *
 * o_indices.h
 * 		Declarations for orioledb indices system tree.
 *
 * Copyright (c) 2021-2022, Oriole DB Inc.
 *
 * IDENTIFICATION
 *	  contrib/orioledb/include/catalog/o_indices.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef __O_INDICES_H__
#define __O_INDICES_H__

#include "orioledb.h"

#include "catalog/o_tables.h"
#include "tuple/format.h"

typedef struct
{
	ORelOids	indexOids;
	OIndexType	indexType;
	ORelOids	tableOids;
	OXid		createOxid;
	NameData	name;
	bool		primaryIsCtid;
	OCompress	compress;
	/* number of fields added using INCLUDE command explicitly */
	/* pkey fields added implicitly in o_o_define_index_validate not counted */
	uint16		nIncludedFields;
	uint16		nLeafFields;
	/*
	 * Number of leaf fields during index creation.  Use this to keep
	 * OTupleFixedFormatSpec stable when new fields are added to the primary
	 * index.
	 */
	uint16		nLeafFieldsInitial;
	uint16		nNonLeafFields;

	/*
	 * TOAST index: number of pkey fields, excluding included fields, including 2
	 * fields: attnum and offset.
	 *
	 * Primary index: number of unique fields in index.
	 *
	 * Unique index: numbfer of fields, excluding included fields and pkey
	 * fields.
	 *
	 * Regular index: number of all fields.
	 */
	uint16		nUniqueFields;
	/* non-TOAST index: field amount, excluding included and pkey fields */
	/* TOAST index: pkey field amount, excluding included fields */
	uint16		nKeyFields;
	/* size of primaryFieldsAttnums */
	uint16		nPrimaryFields;
	/* where primary key fields located in index tuple */
	AttrNumber	primaryFieldsAttnums[INDEX_MAX_KEYS];
	OTableField *leafFields;
	OTableIndexField *nonLeafFields;
	List	   *predicate;		/* list of Expr */
	char	   *predicate_str;
	List	   *expressions;	/* list of Expr */
	MemoryContext index_mctx;
} OIndex;

/* callback for o_indices_foreach_oids() */
typedef void (*OIndexOidsCallback) (OIndexType type, ORelOids treeOids, ORelOids tableOids, void *arg);

extern OIndex *make_o_index(OTable *table, OIndexNumber ixNum);
extern void o_index_fill_descr(OIndexDescr *descr, OIndex *oIndex);
extern void free_o_index(OIndex *o_index);
extern bool o_indices_add(OTable *table, OIndexNumber ixNum, OXid oxid,
						  CommitSeqNo csn);
extern bool o_indices_del(OTable *table, OIndexNumber ixNum, OXid oxid,
						  CommitSeqNo csn);
extern OIndex *o_indices_get(ORelOids oids, OIndexType type);
extern bool o_indices_update(OTable *table, OIndexNumber ixNum,
							 OXid oxid, CommitSeqNo csn);
extern bool o_indices_find_table_oids(ORelOids indexOids, OIndexType type,
									  CommitSeqNo csn, ORelOids *tableOids);
extern void o_indices_foreach_oids(OIndexOidsCallback callback, void *arg);

#endif
