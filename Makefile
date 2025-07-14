MODULES = pg_iot
EXTENSION = pg_iot 
DATA = pg_iot--0.0.1.sql

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
