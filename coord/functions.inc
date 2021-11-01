# Colors
red="\033[0;31m"
green="\033[0;32m"
yellow="\033[0;33m"
reset="\033[0m"

function log_error {
  echo -e "${red}[ERR ]${reset} ${1}"
}

function log_info {
  echo -e "${green}[INFO]${reset} ${1}"
}

function log_warn {
  echo -e "${yellow}[WARN]${reset} ${1}"
}

envup() {
  local file=$([ -z "$1" ] && echo ".env" || echo ".env-$1")

  if [ -f $file ]; then
    echo "Reading environment from $file"
    tmpfile=$(mktemp /tmp/cod.$(date +%s%N))
    grep -vE "^(#.*|\s*)$" $file  > $tmpfile
    while read line; do 
      export "$line"; 
    done < $tmpfile
  else
    echo "No $file file found" 1>&2
    return 1
  fi
}

# This function creates the docker network for Cassandra containers
create_network() {
  
  log_info "Creating network $1 (subnet $2) ..."

  found=$(docker network ls | grep -c "$1")

  if [ "$found" -eq 0 ]; then
    docker network create \
      --attachable \
      --driver bridge \
      --subnet "$2" \
      "$1" || log_error "Cannot create Docker network $1 with subnet $2"
  else
    log_warn "Docker network $1 already existing"
  fi

  log_info "... done."
}

# Start the coordination node
start_coord() {
  log_info "Starting the coordination node ..."
  docker run ${CASSANDRA_RUN_DETACHED} -it --name cassandra_coord \
    --cidfile /tmp/cassandra_coord.cid \
    --network cassandra-net \
    --publish ${CASSANDRA_COORD_INC_HOST_PORT}:${CASSANDRA_COORD_INC_CONTAINER_PORT},${CASSANDRA_COORD_INC_SSL_HOST_PORT}:${CASSANDRA_COORD_INC_SSL_CONTAINER_PORT},${CASSANDRA_COORD_JMX_HOST_PORT}:${CASSANDRA_COORD_JMX_CONTAINER_PORT},${CASSANDRA_COORD_CQL_HOST_PORT}:${CASSANDRA_COORD_CQL_CONTAINER_PORT},${CASSANDRA_COORD_SSL_HOST_PORT}:${CASSANDRA_COORD_SSL_CONTAINER_PORT},${CASSANDRA_COORD_THRIFT_HOST_PORT}:${CASSANDRA_COORD_THRIFT_CONTAINER_PORT} \
    --env  
    cassandra:$CASSANDRA_TAG

  log_info "... done."
}
