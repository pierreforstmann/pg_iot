MODULES = pg_iot
EXTENSION = pg_iot 
DATA = pg_iot--0.0.1.sql

# make installcheck to run automatic test
REGRESS_OPTS =  --temp-instance=/tmp/5555 --port=5555 --temp-config pg_iot.conf
REGRESS = test

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

