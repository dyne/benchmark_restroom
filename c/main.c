/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2022, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
/* <DESC>
 * Download many files in parallel, in the same thread.
 * </DESC>
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <curl/curl.h>

#define MAX_PARALLEL 20 /* number of simultaneous transfers */
#define NUM_URLS 2500

static size_t write_cb(char *data, size_t n, size_t l, void *userp)
{
  /* take care of the data here, ignored in this example */
//printf("%s\n", data);
  (void)data;
  (void)userp;
  return n*l;
}

char *urls[] = {
	"http://localhost:25000/api/zenswarm-oracle-verify-ecdsa",
	"http://localhost:25001/api/zenswarm-oracle-verify-ecdsa",
	"http://localhost:25002/api/zenswarm-oracle-verify-ecdsa",
	"http://localhost:25003/api/zenswarm-oracle-verify-ecdsa",
	"http://localhost:25004/api/zenswarm-oracle-verify-ecdsa"
};

static void add_transfer(CURLM *cm, int i)
{
struct curl_slist *list = NULL;

  list = curl_slist_append(list, "Content-Type:application/json");
  list = curl_slist_append(list, "Accept:application/json");
  CURL *eh = curl_easy_init();
  curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, write_cb);
  curl_easy_setopt(eh, CURLOPT_URL, urls[i % 4]);
  curl_easy_setopt(eh, CURLOPT_PRIVATE, urls[i % 4]);
  curl_easy_setopt(eh, CURLOPT_POSTFIELDS, "{\"data\":{\"asset\":{\"data\":{\"houses\":[{\"name\":\"Harry\",\"team\":\"Gryffindor\"},{\"name\":\"Draco\",\"team\":\"Slytherin\"}],\"number\":42}},\"ecdsa signature\":{\"r\":\"TOmfgO7blPNnuODhAtIBYWQEOqsv6vVhknLSbcDb0Dc=\",\"s\":\"WirTGe4s1mbpCJ4N1y0nPwvi+q6bYAdDJq+q3EMDFGM=\"},\"ecdsa_public_key\":\"BGrFCQHQ9D3Nh7hN1xCubXrRUjJl/Uvg+76kdfY4pJRhezxREKQFSEvsghRCiavo5mQhnwfQ79oz03obR4FfdVc=\"},\"keys\":{}}");
  curl_easy_setopt(eh, CURLOPT_HTTPHEADER, list);
  curl_multi_add_handle(cm, eh);
}

int main(void)
{
  CURLM *cm;
  CURLMsg *msg;
  unsigned int transfers = 0;
  int msgs_left = -1;
  int still_alive = 1;

  curl_global_init(CURL_GLOBAL_ALL);
  cm = curl_multi_init();

  /* Limit the amount of simultaneous connections curl should allow: */
  curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, (long)MAX_PARALLEL);

  for(transfers = 0; transfers < MAX_PARALLEL; transfers++)
    add_transfer(cm, transfers);

  do {
    curl_multi_perform(cm, &still_alive);

    while((msg = curl_multi_info_read(cm, &msgs_left))) {
      if(msg->msg == CURLMSG_DONE) {
        char *url;
        CURL *e = msg->easy_handle;
        curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &url);
        //fprintf(stderr, "R: %d - %s <%s>\n",
        //        msg->data.result, curl_easy_strerror(msg->data.result), url);
        curl_multi_remove_handle(cm, e);
        curl_easy_cleanup(e);
      }
      else {
        fprintf(stderr, "E: CURLMsg (%d)\n", msg->msg);
      }
      if(transfers < NUM_URLS)
        add_transfer(cm, transfers++);
    }
    if(still_alive)
      curl_multi_wait(cm, NULL, 0, 1000, NULL);

  } while(still_alive || (transfers < NUM_URLS));

  curl_multi_cleanup(cm);
  curl_global_cleanup();

  return EXIT_SUCCESS;
}
