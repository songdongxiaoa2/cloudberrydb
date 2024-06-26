import time

from gppylib.commands import base
from gppylib.db import dbconn
import pg

FTS_PROBE_QUERY = 'SELECT pg_catalog.gp_request_fts_probe_scan()'

class SegmentReconfigurer:
    def __init__(self, logger, worker_pool, timeout):
        self.logger = logger
        self.pool = worker_pool
        self.timeout = timeout

    def _trigger_fts_probe(self, dburl):
        conn = pg.connect(dbname=dburl.pgdb,
                host=dburl.pghost,
                port=dburl.pgport,
                opt=None,
                user=dburl.pguser,
                passwd=dburl.pgpass,
                )
        conn.query(FTS_PROBE_QUERY)
        conn.close()

    def reconfigure(self):
        # issue a distributed query to make sure we pick up the fault
        # that we just caused by shutting down segments
        self.logger.info("Triggering segment reconfiguration")
        dburl = dbconn.DbURL()
        self._trigger_fts_probe(dburl)
        start_time = time.time()
        while True:
            time.sleep(1)
            try:
                # Empty block of 'BEGIN' and 'END' won't start a distributed transaction,
                # execute a DDL query to start a distributed transaction.
                # so the primaries'd better be up
                conn = dbconn.connect(dburl)
                conn.cursor().execute('CREATE TEMP TABLE temp_test(a int)')
                conn.cursor().execute('COMMIT')
            except Exception as e:
                # Should close conn here
                # Otherwise, the postmaster will be blocked by abort transaction
                conn.close()
                now = time.time()
                if now < start_time + self.timeout:
                    continue
                else:
                    raise RuntimeError("Mirror promotion did not complete in {0} seconds.".format(self.timeout))
            else:
                conn.close()
                break
