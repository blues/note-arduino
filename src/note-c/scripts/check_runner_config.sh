#!/bin/bash

function env_var_defined() {
    [ -v $1 ] || echo "Environment variable '$1' not set."
}

function check_all() {
    env_var_defined "NOTEHUB_CLIENT_ID"
    env_var_defined "NOTEHUB_CLIENT_SECRET"
    env_var_defined "NOTEHUB_PROJECT_UID"
    env_var_defined "NOTEHUB_PRODUCT_UID"
    env_var_defined "MD5SRV_PORT"
    env_var_defined "MD5SRV_ADDRESS"
    env_var_defined "MD5SRV_TOKEN"
    env_var_defined "SWAN_SERIAL"
    env_var_defined "STLINK_PROGRAMMER_PORT"

    # these are defined in the workflow itself, but good to check in case of errors causing them to be unset
    env_var_defined "NOTEHUB_API"
    env_var_defined "NOTEHUB"
    env_var_defined "PIO_PROJECT_DIR"
    env_var_defined "NOTEHUB_PROXY_ROUTE_LABEL"
    env_var_defined "NOTEHUB_PROXY_ROUTE_ALIAS"
    env_var_defined "NOTEHUB_HTTP_ROUTE_LABEL"


}

errors=$(check_all)
if [ -n "$errors" ]; then
    echo "$errors"   # quoted to preserve newlines
    echo "There are configuration errors. See the log above for details."
    exit 1
fi

exit 0
