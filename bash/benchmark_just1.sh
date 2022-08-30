#!/bin/bash
port=$(($1+25000))
echo "Start test on port $port"
url="http://localhost:$port/api/zenswarm-oracle-verify-ecdsa"
for i in $(seq 1 500); do
	curl -X 'POST' \
  $url \
  -H 'accept: application/json' \
  -H 'Content-Type: application/json' \
  -d '{
  "data":{
	"asset": {
		"data": {
			"houses": [
				{
					"name": "Harry",
					"team": "Gryffindor"
				},
				{
					"name": "Draco",
					"team": "Slytherin"
				}
			],
			"number": 42
		}
	},
	"ecdsa signature": {
		"r": "TOmfgO7blPNnuODhAtIBYWQEOqsv6vVhknLSbcDb0Dc=",
		"s": "WirTGe4s1mbpCJ4N1y0nPwvi+q6bYAdDJq+q3EMDFGM="
	},
	"ecdsa_public_key": "BGrFCQHQ9D3Nh7hN1xCubXrRUjJl/Uvg+76kdfY4pJRhezxREKQFSEvsghRCiavo5mQhnwfQ79oz03obR4FfdVc="
},
  "keys": {}
}'
done 1>/dev/null 2>/dev/null
echo "End test on port $port"
