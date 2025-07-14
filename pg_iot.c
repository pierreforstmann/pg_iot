/*-------------------------------------------------------------------------
 *  
 * pg_iot is a PostgreSQL extension which allows to set table as
 * insert-only table: only INSERT and SELECT statements are allowed,
 * UPDATE and DELETE statements are forbidden.
 *  
 * This program is open source, licensed under the PostgreSQL license.
 * For license terms, see the LICENSE file.
 *          
 * Copyright (c) 2025, Pierre Forstmann.
 *            
 *-------------------------------------------------------------------------
*/
#include "postgres.h"
#include "fmgr.h"
#include "executor/executor.h"
#include "nodes/pg_list.h"
#include "catalog/pg_class.h"
#include "catalog/namespace.h"
#include "utils/rel.h"
#include "access/relation.h"
#include "utils/lsyscache.h"
#include "postgres_ext.h"
#include "utils/builtins.h"
#include "executor/spi.h"

PG_MODULE_MAGIC;

static ExecutorStart_hook_type prev_ExecutorStart = NULL;
static void iot_executor_hook(QueryDesc *queryDesc, int eflags);

PG_FUNCTION_INFO_V1(pg_iot_set);

/*
 * set table as insert-only table 
 */
Datum pg_iot_set(PG_FUNCTION_ARGS)
{

	char *schema_name;
        char *table_name;

	StringInfoData buf_insert;
	Oid argtypes[2] = { TEXTOID, TEXTOID };
	SPIPlanPtr plan_ptr;
	Datum values[2];
	int ret_code;

        schema_name = PG_GETARG_CSTRING(0);
        table_name = PG_GETARG_CSTRING(1);

	initStringInfo(&buf_insert);
        appendStringInfo(&buf_insert, "INSERT into iot.tables values($1, $2)");

        SPI_connect();

        plan_ptr = SPI_prepare(buf_insert.data, 2, argtypes);
	values[0] = CStringGetTextDatum(schema_name);
	values[1] = CStringGetTextDatum(table_name);
	ret_code = SPI_execute_plan(plan_ptr, values, NULL, false, 0);
        if (ret_code != SPI_OK_INSERT)
                  elog(ERROR, "INSERT INTO iot.tables failed");

	SPI_finish();

	PG_RETURN_BOOL(true);
}

/*
 * is_iot
 */
static bool is_iot(const char *schema_name, const char *table_name) 
{
	StringInfoData 	buf_select;
	Oid argtypes[2] = { TEXTOID, TEXTOID };
	SPIPlanPtr plan_ptr;
	Datum values[2];
	int ret_code;
	bool result;

	initStringInfo(&buf_select);
	appendStringInfo(&buf_select, 
			 "SELECT namespace, relname FROM iot.tables WHERE namespace = $1 and relname = $2");
        SPI_connect();

	plan_ptr = SPI_prepare(buf_select.data, 2, argtypes);
        values[0] = CStringGetTextDatum(schema_name);
        values[1] = CStringGetTextDatum(table_name);
	ret_code = SPI_execute_plan(plan_ptr, values, NULL, false, 0);

	if (ret_code != SPI_OK_SELECT)
		elog(ERROR, "SELECT FROM iot.tables failed");
	if (SPI_processed == 1)
	{
		result = true;
	}
	else 
	{
		result = false; 
	}

	SPI_finish();
	return result;
}

/*
 * _PG_init
 */

void _PG_init(void)
{
    prev_ExecutorStart = ExecutorStart_hook;
    ExecutorStart_hook = iot_executor_hook;
}

/*
 * iot_executor_hook
 */
static void iot_executor_hook(QueryDesc *queryDesc, int eflags)
{
    if (queryDesc->operation == CMD_UPDATE || queryDesc->operation == CMD_DELETE)
    {
        List *rtable = queryDesc->plannedstmt->rtable;
        ListCell *lc;

        foreach(lc, rtable)
        {
            RangeTblEntry *rte = (RangeTblEntry *) lfirst(lc);
            if (rte->rtekind == RTE_RELATION)
            {
                Oid relid = rte->relid;
                Relation rel = relation_open(relid, AccessShareLock);

                const char *relname = RelationGetRelationName(rel);
                const char *schemaname = get_namespace_name(RelationGetNamespace(rel));

                if (is_iot(schemaname, relname))
                {
                    relation_close(rel, AccessShareLock);
                    ereport(ERROR,
                            (errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
                             errmsg("pg_iot: UPDATE and DELETE operations are disabled on table \"%s.%s\"", schemaname, relname)));
                }

                relation_close(rel, AccessShareLock);
            }
        }
    }

    if (prev_ExecutorStart)
        prev_ExecutorStart(queryDesc, eflags);
    else
        standard_ExecutorStart(queryDesc, eflags);
}

