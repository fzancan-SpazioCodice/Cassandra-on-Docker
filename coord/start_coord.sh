#!/usr/bin/env /bin/bash

source functions.inc

cd "$(dirname $0)" || exit 127

envup

create_network "$CASSANDRA_NETWORK_NAME" "$CONTAINERS_NETWORK_SUBNET"

docker-compose up ${CASSANDRA_RUN_DETACHED}

exit 0
