export test_dir=test_files
export nonexist_dir=doesnotexist
export token=abc
export invalid_token=def
export port=8123
export address=0.0.0.0
export md5url=http://$address:$port
export venv=../../../venv
export startTimeout=${MD5SRV_TIMEOUT:-3}
export clean_test_dir=true

bats_require_minimum_version 1.10.0

# NB: on OSX, export BATS_LIB_PATH=/usr/local/lib
bats_load_library "bats-support"
bats_load_library "bats-assert"

assert [ -n "$address" ]
assert [ -n "$port" ]
assert [ -n "$md5url" ]
assert [ -n "$token" ]

# don't want these environment variables defined as these interfere with the tests
assert [ -z "$MD5SRV_PORT" ]
assert [ -z "$MD5SRV_ADDRESS" ]
assert [ -z "$MD5SRV_TOKEN" ]

# The -f curl flag is used to distinguish between a communication error and a http server error

function teardown() {
    stopMD5Server
}


function cleanTestDir() {
    ! $clean_test_dir || rm -rf $test_dir
    [ -d $test_dir ] || mkdir $test_dir
}

function _waitForMD5Server() {
    # even though a 4xx error is returned, curl will return a success response if it contacts the webserver
    result=500
    until [ $result -lt 500 ]; do
        sleep 0.1
        result=`curl -s $md5url -o /dev/null -w "%{http_code}"`
    done
}

function waitForMD5ServerWithTimeout() {
    export -f _waitForMD5Server
    export address=${1:-$address}
    export port=${2:-$port}
    export md5url=http://$address:$port
    timeout $startTimeout bash -c "_waitForMD5Server"
}

function startMD5Server() {
    cleanTestDir
    refute curl -s $md5url     # ensure the server is not already running
    python3 md5srv.py --dir $test_dir --token $token --save --address $address --port $port 3>&- &
    export md5_pid=$!
    waitForMD5ServerWithTimeout
}

function stopMD5Server() {
    pid=${1:-$md5_pid}
    unset md5_pid
    [ -z "$pid" ] || kill $pid
}

function assert_json() {
    assert_equal $(jq -r ".$1"  <<< "$output") $2
}

@test "Should not fail when the target directory does not exist and --save is not present" {
    python3 md5srv.py --dir $nonexist_dir --address $address --port $port &
    md5_pid=$!
    waitForMD5ServerWithTimeout
    run curl -s -f -X PUT $md5url/t1 -d abc
    stopMD5Server
    assert_success
    assert_json md5 900150983cd24fb0d6963f7d28e17f72
    assert_json length 3
}

@test "Fails when the target directory does not exist and --save is present" {
    run timeout $startTimeout python3 md5srv.py --dir $nonexist_dir --token $token --save --port $port
    assert_output --partial "not exist"
}

@test "Should not save files without an access token" {
    cleanTestDir
    run timeout $startTimeout python3 md5srv.py --dir $test_dir --save --port $port
    assert_output --partial "Token must be provided"
}

@test "Can be run with default arguments" {
    # Note that this runs the server on the default address and port, which may not be the same
    # as the primary address and port used
    python3 md5srv.py &
    waitForMD5ServerWithTimeout "0.0.0.0" "8080"
    run timeout 5 curl -s -f -X PUT http://0.0.0.0:8080/t1 -d abc
    kill $!
    assert_success
}

@test "Can be run with default arguments taking default values from the environment" {
    # scope setting the environment variables. I thought they would not live beyond the
    # test but they seem to affect later tests without scoping.
    function scopeed() {
        ND5SRV_PORT=8234
        MD5SRV_TOKEN=1234
        ND5SRV_ADDRESS=127.0.0.1
        python3 md5srv.py &
        pid=$!
        waitForMD5ServerWithTimeout "$MD5SRV_ADDRESS" "$MD5SRV_PORT"
        run timeout 5 curl -s -f -X PUT http://0.0.0.0:8080/default-args-env -d abc -H "X-Access-Token: $MD5SRV_TOKEN"
        kill $pid
        assert_success
    }
    $scoped
}


@test "Should allow web.put without --save no file is created" {
    cleanTestDir
    refute curl -s $md5url
    python3 md5srv.py --dir $test_dir --address $address --port $port 3>&- &
    md5_pid=$!
    waitForMD5ServerWithTimeout
    run curl -s -X PUT $md5url/t1 -d abc

    assert_success
    assert_json md5 900150983cd24fb0d6963f7d28e17f72
    assert_json length 3
    assert [ ! -e $test_dir/payload.bin ]
}

@test "web.put with no access token should fail with 401 Unauthorized" {
    startMD5Server
    # get just the http code
    http_code=`curl -s -o /dev/null -w "%{http_code}" -X PUT $md5url/noaccess?chunk=0 -d abc`
    assert_equal "$http_code" "401"

    run curl -s -X PUT $md5url/noaccess?chunk=0 -d abc
    stopMD5Server
    assert_success
    assert_json err Unauthorized
}

@test "web.put with invalid access token should fail with 403 Forbidden" {
    startMD5Server

    # get just the http code
    http_code=`curl -s -o /dev/null -w "%{http_code}"  -X PUT -H "X-Access-Token: $invalid_token" $md5url/invalid?chunk=0 -d abc`
    assert_equal "$http_code" "403"

    run curl -s -X PUT -H "X-Access-Token: $invalid_token" $md5url/invalid?chunk=0 -d abc
    stopMD5Server
    assert_success
    assert_json err "Forbidden"
}

@test "non-chunked web.put and web.get with saved data should return the md5 and length" {
    startMD5Server
    curl -v -s -f -H "X-Access-Token: $token" -X PUT $md5url/nonchunked -d abc
    run curl -s -f -H "X-Access-Token: $token" -X GET $md5url/nonchunked
    stopMD5Server
    assert_success
    assert_json md5 900150983cd24fb0d6963f7d28e17f72
    assert_json length 3
    assert [ -d $test_dir/nonchunked ]
    assert [ -f $test_dir/nonchunked/payload.bin ]
}

@test "chunked web.put and web.get with saved data should return the md5 and length of all chunks" {
    startMD5Server
    curl -s -f -H "X-Access-Token: $token" -X PUT $md5url/chunked?chunk=0 -d abc
    curl -s -f -H "X-Access-Token: $token" -X PUT $md5url/chunked?chunk=1 -d def
    run curl -s -f -H "X-Access-Token: $token" -X GET $md5url/chunked
    stopMD5Server
    assert_success
    assert_json md5 e80b5017098950fc58aad83c8c14978e
    assert_json length 6
    assert [ -d $test_dir/chunked ]
    assert [ -f $test_dir/chunked/payload00000.bin ]
    assert [ -f $test_dir/chunked/payload00001.bin ]
}

@test "non-chunked web.put repeats should fail when the file already exists" {
    startMD5Server
    run curl -s -f -H "X-Access-Token: $token" -X PUT $md5url/nonchunked-fail -d abc
    stopMD5Server   # ensures the server is stopped before assertions exit the test
    assert_success
    assert_json md5 900150983cd24fb0d6963f7d28e17f72
    assert [ -f $test_dir/nonchunked-fail/payload.bin ]

    clean_test_dir=false
    startMD5Server
    assert [ -f $test_dir/nonchunked-fail/payload.bin ]
    run curl -s --fail-with-body -H "X-Access-Token: $token" -X PUT $md5url/nonchunked-fail -d abc
    stopMD5Server
    assert_failure
    assert_output --partial "already exists"
    assert_json code 409
}

@test "chunked web.put repeats should fail when the file already exists" {
    startMD5Server
    run curl -s -f -H "X-Access-Token: $token" -X PUT $md5url/nonchunked-fail?chunk=0 -d abc
    stopMD5Server   # ensures the server is stopped before assertions exit the test
    assert_success
    assert_json md5 900150983cd24fb0d6963f7d28e17f72
    assert [ -f $test_dir/nonchunked-fail/payload00000.bin ]

    export clean_test_dir=false
    startMD5Server
    assert [ -f $test_dir/nonchunked-fail/payload00000.bin ]
    run curl -s --fail-with-body -H "X-Access-Token: $token" -X PUT $md5url/nonchunked-fail?chunk=0 -d abc
    stopMD5Server
    assert_failure
    assert_output --partial "already exists"
    assert_json code 409
}

@test "Can post a note which is decoded and the payload saved to the server" {
    assert [ -e "note.json" ]
    startMD5Server
    run curl -s -X POST $md5url/addnote?note=1 -d "@note.json" -H "X-Access-Token: $token" -H "Content-Type: application/json"
    stopMD5Server
    assert_success
    assert_json md5 f54095bd47b357cbd57ace4b4da76eaa
    assert_json length 10230
    assert [ -e $test_dir/Random_1234/payload.bin ]
}

@test "Can post a chunk and then retrieve it" {
    assert [ -z "$MD5SRV_PORT" ]
    startMD5Server
    content=abcdef
    content_length=6
    timeout 5 curl -s -X POST $md5url/getchunk?chunk=0 -d "$content" -H "X-Access-Token: $token"
    run timeout 5 curl -s -X GET -D headers.txt $md5url/getchunk?chunk=0 -d "$content" -H "X-Access-Token: $token"
    stopMD5Server

    assert_success
    assert_output "${content}"
    assert [ -e $test_dir/getchunk/payload00000.bin ]
    assert [ -e headers.txt ]

    run bash -c 'cat headers.txt | grep "Content-Type" | xargs'
    assert_success
    assert_output --partial "Content-Type: application/octet-stream"

    run bash -c 'cat headers.txt | grep "Content-Length" | xargs'
    assert_success
    assert_output --partial "Content-Length: $content_length"

    rm headers.txt
}
